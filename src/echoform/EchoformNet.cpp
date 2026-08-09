#include "echoform/EchoformNet.h"

#include <SD_MMC.h>
#include <WiFi.h>

#include "echoform/EchoState.h"
#include "echoform/EchoformAudio.h"
#include "network/WifiConnector.h"

namespace {
inline uint8_t evCode(echoform::EchoEvent e) {
  return static_cast<uint8_t>(e);
}
}  // namespace

namespace {

constexpr char kConfigPath[] = "/echoform.json";
// Session cadence, straight from echoform_net.rs.
constexpr uint32_t kTickMs = 30;
constexpr uint32_t kPingTicks = 5000 / kTickMs;
constexpr uint32_t kReconnectBackoffMs = 2000;
constexpr uint32_t kWifiRetryMs = 5000;
constexpr uint32_t kWifiPerNetworkTimeoutMs = 15000;
constexpr uint32_t kConnectTimeoutMs = 10000;
// If the link goes fully silent (no bytes at all) past this, assume a dead
// peer: 3 keepalive periods.
constexpr uint32_t kSilentLinkMs = 15000;

constexpr uint32_t kTaskStackBytes = 8192;
constexpr BaseType_t kTaskCore = 0;

// Tiny hand-rolled JSON string/number extraction, same style as OtaManager's
// wifi.json parsing (no JSON lib dependency).
String jsonString(const String &json, const char *key) {
  const String needle = String("\"") + key + "\"";
  const int keyIdx = json.indexOf(needle);
  if (keyIdx < 0) return String();
  const int colon = json.indexOf(':', keyIdx + needle.length());
  if (colon < 0) return String();
  const int quoteOpen = json.indexOf('"', colon + 1);
  if (quoteOpen < 0) return String();
  const int quoteClose = json.indexOf('"', quoteOpen + 1);
  if (quoteClose < 0) return String();
  return json.substring(quoteOpen + 1, quoteClose);
}

long jsonNumber(const String &json, const char *key, long fallback) {
  const String needle = String("\"") + key + "\"";
  const int keyIdx = json.indexOf(needle);
  if (keyIdx < 0) return fallback;
  const int colon = json.indexOf(':', keyIdx + needle.length());
  if (colon < 0) return fallback;
  int at = colon + 1;
  while (at < static_cast<int>(json.length()) &&
         (json[at] == ' ' || json[at] == '\t')) {
    ++at;
  }
  long value = 0;
  bool any = false;
  while (at < static_cast<int>(json.length()) && isdigit(json[at])) {
    value = value * 10 + (json[at] - '0');
    any = true;
    ++at;
  }
  return any ? value : fallback;
}

}  // namespace

void EchoformNet::uiSnapshot(String &user, String &assist,
                             String &nowPlaying) {
  if (uiMutex_ == nullptr) return;
  if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(5)) != pdTRUE) return;
  user = uiUser_;
  assist = uiAssist_;
  nowPlaying = uiNowPlaying_;
  xSemaphoreGive(uiMutex_);
}

namespace {
String utf8ToString(const uint8_t *bytes, size_t len) {
  String s;
  s.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    s += static_cast<char>(bytes[i]);
  }
  return s;
}
}  // namespace

void EchoformNet::uiSetUser(const uint8_t *utf8, size_t len) {
  if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(20)) != pdTRUE) return;
  uiUser_ = utf8ToString(utf8, len);
  xSemaphoreGive(uiMutex_);
}

String EchoformNet::finalTranscript() {
  String out;
  if (uiMutex_ != nullptr &&
      xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(5)) == pdTRUE) {
    out = uiUserFinal_;
    xSemaphoreGive(uiMutex_);
  }
  return out;
}

void EchoformNet::uiAppendAssist(const uint8_t *utf8, size_t len) {
  if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(20)) != pdTRUE) return;
  uiAssist_ += utf8ToString(utf8, len);
  if (uiAssist_.length() > 512) {
    uiAssist_.remove(0, uiAssist_.length() - 512);
  }
  xSemaphoreGive(uiMutex_);
}

void EchoformNet::uiClearConversation() {
  if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(20)) != pdTRUE) return;
  uiUser_ = "";
  uiAssist_ = "";
  xSemaphoreGive(uiMutex_);
}

void EchoformNet::uiSetNowPlaying(const uint8_t *utf8, size_t len) {
  if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(20)) != pdTRUE) return;
  uiNowPlaying_ = utf8ToString(utf8, len);
  xSemaphoreGive(uiMutex_);
}

bool EchoformNet::begin(EchoformAudio *audio) {
  audio_ = audio;
  uiMutex_ = xSemaphoreCreateMutex();
  cmdQueue_ = xQueueCreate(8, sizeof(Cmd));
  // Deep enough that a streaming-delta burst between render frames can
  // never crowd out a state-critical event (RequestCompleted was being
  // dropped at depth 16 while the render loop ran slow).
  eventQueue_ = xQueueCreate(64, sizeof(uint8_t));
  const BaseType_t ok = xTaskCreatePinnedToCore(
      &EchoformNet::taskTrampoline, "echoform_net", kTaskStackBytes, this,
      tskIDLE_PRIORITY + 2, nullptr, kTaskCore);
  if (ok != pdPASS) {
    Serial.println("[net] xTaskCreate FAILED");
    return false;
  }
  return true;
}

void EchoformNet::beginUtterance(uint16_t flags) {
  const Cmd cmd{CmdType::Begin, flags};
  xQueueSend(cmdQueue_, &cmd, 0);
}

void EchoformNet::endUtterance() {
  const Cmd cmd{CmdType::End, 0};
  xQueueSend(cmdQueue_, &cmd, 0);
}

void EchoformNet::cancelUtterance() {
  const Cmd cmd{CmdType::Cancel, 0};
  xQueueSend(cmdQueue_, &cmd, 0);
}

void EchoformNet::taskTrampoline(void *arg) {
  static_cast<EchoformNet *>(arg)->taskRun();
  vTaskDelete(nullptr);
}

bool EchoformNet::loadConfigs() {
  // WiFi credentials: reuse the reader's wifi.json loader wholesale.
  OtaManager ota;
  if (ota.loadConfigFromSd()) {
    networks_ = ota.config().networks;
  } else {
    networks_.clear();
  }

  // Relay endpoint.
  relayHost_ = "";
  relayPort_ = 0;
  File f = SD_MMC.open(kConfigPath);
  if (f && !f.isDirectory()) {
    String json;
    json.reserve(f.size());
    while (f.available()) {
      json += static_cast<char>(f.read());
    }
    f.close();
    relayHost_ = jsonString(json, "host");
    relayPort_ = static_cast<uint16_t>(jsonNumber(json, "port", 8125));
    // Optional shared secret for publicly reachable relays (Railway): sent
    // as raw bytes before the Hello frame; the relay's accept gate checks
    // it. Empty = LAN relay, nothing sent.
    relayToken_ = jsonString(json, "token");
  } else if (f) {
    f.close();
  }
  relayConfigured_.store(!relayHost_.isEmpty() && relayPort_ != 0);
  Serial.printf("[net] config: %u wifi network(s), relay %s:%u\n",
                static_cast<unsigned>(networks_.size()),
                relayHost_.isEmpty() ? "<unset>" : relayHost_.c_str(),
                relayPort_);
  return relayConfigured_.load();
}

bool EchoformNet::setRelayEndpoint(const String &host, uint16_t port,
                                   const String &token) {
  File f = SD_MMC.open(kConfigPath, FILE_WRITE);
  if (!f) {
    Serial.println("[net] echoform.json write failed");
    return false;
  }
  if (token.isEmpty()) {
    f.printf("{\"host\": \"%s\", \"port\": %u}\n", host.c_str(), port);
  } else {
    f.printf("{\"host\": \"%s\", \"port\": %u, \"token\": \"%s\"}\n",
             host.c_str(), port, token.c_str());
  }
  f.close();
  reloadRequested_.store(true);
  Serial.printf("[net] relay endpoint set to %s:%u%s\n", host.c_str(), port,
                token.isEmpty() ? "" : " (with auth token)");
  return true;
}

void EchoformNet::taskRun() {
  loadConfigs();
  for (;;) {
    if (reloadRequested_.exchange(false)) {
      loadConfigs();
    }
    if (!relayConfigured_.load()) {
      // Nothing to connect to; idle until SETRELAY arrives.
      linkState_.store(static_cast<uint8_t>(LinkState::NoIp));
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }
    if (WiFi.status() != WL_CONNECTED) {
      linkState_.store(static_cast<uint8_t>(LinkState::NoIp));
      if (networks_.empty()) {
        Serial.println("[net] no wifi networks configured (wifi.json)");
        vTaskDelay(pdMS_TO_TICKS(kWifiRetryMs));
        continue;
      }
      WiFi.mode(WIFI_STA);
      if (!WifiConnector::connect(networks_, kWifiPerNetworkTimeoutMs,
                                  "echoform")) {
        vTaskDelay(pdMS_TO_TICKS(kWifiRetryMs));
        continue;
      }
      // Modem power-save adds hundreds of ms of latency and eventually
      // wedges the association (observed: RTT 11 -> 712 ms, then zombie
      // link). A voice pipeline needs the radio awake.
      WiFi.setSleep(false);
      Serial.printf("[net] wifi up: %s\n", WiFi.localIP().toString().c_str());
    }
    linkState_.store(static_cast<uint8_t>(LinkState::NoRelay));

    WiFiClient client;
    Serial.printf("[net] connecting to relay %s:%u\n", relayHost_.c_str(),
                  relayPort_);
    if (!client.connect(relayHost_.c_str(), relayPort_, kConnectTimeoutMs)) {
      Serial.println("[net] relay connect failed");
      // WiFi.status() can stay WL_CONNECTED on a zombie link (observed
      // after ~3 h: AP association stale, no traffic passes). After a few
      // consecutive failures, tear the link down so the next loop redoes
      // the full WifiConnector dance.
      ++relayConnectFails_;
      if (relayConnectFails_ >= kFailsBeforeWifiReset) {
        Serial.println("[net] repeated failures; resetting wifi link");
        WiFi.disconnect();
        relayConnectFails_ = 0;
      }
      vTaskDelay(pdMS_TO_TICKS(kReconnectBackoffMs));
      continue;
    }
    relayConnectFails_ = 0;
    client.setNoDelay(true);
    linkState_.store(static_cast<uint8_t>(LinkState::SessionLive));
    const char *why = session(client);
    linkState_.store(static_cast<uint8_t>(LinkState::NoRelay));
    sessionDrops_.fetch_add(1);
    lastDropReason_.store(why);
    Serial.printf("[net] session ended: %s\n", why);
    client.stop();
    vTaskDelay(pdMS_TO_TICKS(kReconnectBackoffMs));
  }
}

bool EchoformNet::sendFrame(WiFiClient &client, echo1::MsgType type,
                            uint16_t requestId, uint16_t sequence,
                            uint16_t flags, const uint8_t *payload,
                            size_t payloadLen) {
  const size_t n = echo1::encode(type, requestId, sequence, flags, payload,
                                 payloadLen, frameOut_, sizeof(frameOut_));
  if (n == 0) return false;
  size_t at = 0;
  while (at < n) {
    const size_t written = client.write(frameOut_ + at, n - at);
    if (written == 0) return false;
    at += written;
  }
  return true;
}

bool EchoformNet::pumpMic(WiFiClient &client, uint16_t requestId,
                          uint16_t &outSeq) {
  if (audio_ == nullptr) return true;
  for (;;) {
    const size_t n = audio_->txDrain(pcm_, sizeof(pcm_) / sizeof(pcm_[0]));
    if (n == 0) return true;
    for (size_t i = 0; i < n; ++i) {
      pcmBytes_[i * 2] = static_cast<uint8_t>(pcm_[i] & 0xFF);
      pcmBytes_[i * 2 + 1] = static_cast<uint8_t>(pcm_[i] >> 8);
    }
    outSeq = static_cast<uint16_t>(outSeq + 1);
    if (!sendFrame(client, echo1::MsgType::AudioPcm16, requestId, outSeq, 0,
                   pcmBytes_, n * 2)) {
      return false;
    }
  }
}

bool EchoformNet::handleCmd(WiFiClient &client, const Cmd &cmd,
                            echo1::RequestTracker &tracker, uint32_t ticks,
                            uint16_t &outSeq, const char *&why) {
  uint16_t activeId = 0;
  switch (cmd.type) {
    case CmdType::Begin: {
      if (audio_ != nullptr && !audio_->startCapture()) {
        Serial.println("[net] mic capture bring-up failed; utterance dropped");
        break;
      }
      const uint16_t id = tracker.begin(ticks);
      requestActive_.store(true);
      uiClearConversation();
      outSeq = 0;
      if (!sendFrame(client, echo1::MsgType::BeginUtterance, id, 0, cmd.flags,
                     nullptr, 0)) {
        why = "begin send failed";
        return false;
      }
      Serial.printf("[net] utterance %u begun (flags 0x%X)\n", id, cmd.flags);
      break;
    }
    case CmdType::End: {
      if (tracker.active(activeId)) {
        // Flush whatever PCM is still queued first, then End, then release
        // the I2S port back to the TX-only driver so the reply can play.
        if (!pumpMic(client, activeId, outSeq)) {
          why = "pcm flush failed";
          return false;
        }
        if (!sendFrame(client, echo1::MsgType::EndUtterance, activeId, outSeq,
                       0, nullptr, 0)) {
          why = "end send failed";
          return false;
        }
        if (audio_ != nullptr) {
          audio_->captureTeardown();
        }
        Serial.printf("[net] utterance %u ended\n", activeId);
        postAvatarEvent(evCode(echoform::EchoEvent::RequestStarted));
      }
      break;
    }
    case CmdType::Cancel: {
      if (tracker.active(activeId)) {
        sendFrame(client, echo1::MsgType::CancelRequest, activeId, 0, 0,
                  nullptr, 0);
        tracker.cancel();
        requestActive_.store(false);
        if (audio_ != nullptr) {
          audio_->captureTeardown();
        }
      }
      break;
    }
  }
  return true;
}

const char *EchoformNet::session(WiFiClient &client) {
  // -- auth prefix (public relays only) --
  if (!relayToken_.isEmpty()) {
    if (client.write(reinterpret_cast<const uint8_t *>(relayToken_.c_str()),
                     relayToken_.length()) != relayToken_.length()) {
      return "auth token send failed";
    }
  }
  // -- handshake --
  uint8_t payload[8];
  echo1::Hello hello{echo1::kVersion,
                     static_cast<uint16_t>(echo1::kMaxPayload)};
  const size_t helloLen = hello.encodeTo(payload, sizeof(payload));
  if (!sendFrame(client, echo1::MsgType::Hello, 0, 0, 0, payload, helloLen)) {
    return "hello send failed";
  }

  parser_.reset();
  // 30 s request timeout in 30 ms ticks, from echoform_net.rs.
  echo1::RequestTracker tracker(30000 / kTickMs);
  bool helloAcked = false;
  uint32_t ticks = 0;
  uint16_t outSeq = 0;
  uint32_t lastTickMs = millis();
  uint32_t lastInboundMs = millis();
  sessionTracker_ = &tracker;
  sessionTicks_ = &ticks;

  const char *why = nullptr;
  while (why == nullptr) {
    if (reloadRequested_.load()) {
      why = "config reload requested";
      break;
    }
    if (!client.connected()) {
      why = "socket closed";
      break;
    }
    // -- inbound bytes --
    while (client.available() > 0) {
      const int n = client.read(readBuf_, sizeof(readBuf_));
      if (n <= 0) break;
      lastInboundMs = millis();
      parser_.extend(readBuf_, static_cast<size_t>(n));
      echo1::FrameHeader header;
      const uint8_t *body = nullptr;
      size_t bodyLen = 0;
      while (parser_.poll(header, body, bodyLen)) {
        handleFrame(client, header, body, bodyLen, helloAcked);
      }
      if (parser_.stats.badCrc > 16) {
        why = "persistent CRC failures";
      }
    }
    if (why != nullptr) break;
    // -- commands --
    Cmd cmd;
    while (cmdQueue_ != nullptr && xQueueReceive(cmdQueue_, &cmd, 0) == pdTRUE) {
      if (!handleCmd(client, cmd, tracker, ticks, outSeq, why)) break;
    }
    if (why != nullptr) break;
    // -- periodic work (30 ms multiplex tick) --
    const uint32_t nowMs = millis();
    if (nowMs - lastTickMs >= kTickMs) {
      lastTickMs = nowMs;
      ++ticks;
      uint16_t activeId = 0;
      if (tracker.active(activeId) && audio_ != nullptr &&
          audio_->capturing()) {
        if (!pumpMic(client, activeId, outSeq)) {
          why = "pcm send failed";
          break;
        }
      }
      if (tracker.tick(ticks)) {
        Serial.println("[net] request timed out");
        requestActive_.store(false);
        postAvatarEvent(evCode(echoform::EchoEvent::Timeout));
      }
      if (ticks % kPingTicks == 0) {
        pingSentMs_ = nowMs;
        if (!sendFrame(client, echo1::MsgType::Ping, 0, 0, 0, nullptr, 0)) {
          why = "ping failed";
          break;
        }
      }
      if (nowMs - lastInboundMs > kSilentLinkMs) {
        why = "silent link";
        break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
  sessionTracker_ = nullptr;
  sessionTicks_ = nullptr;
  requestActive_.store(false);
  return why;
}

void EchoformNet::handleFrame(WiFiClient &client,
                              const echo1::FrameHeader &header,
                              const uint8_t *body, size_t bodyLen,
                              bool &helloAcked) {
  using echo1::MsgType;
  if (!helloAcked) {
    if (header.msgType == MsgType::HelloAck) {
      echo1::Hello ack{};
      const bool ok = echo1::Hello::parse(body, bodyLen, ack) &&
                      echo1::negotiate(echo1::kVersion, ack.protocolVersion) != 0;
      if (!ok) {
        Serial.println("[net] version negotiation failed");
        client.stop();
        return;
      }
      helloAcked = true;
      // Session config: all-defaults (empty strings = relay defaults),
      // captions + conversation memory on, mic channel left.
      echo1::SessionConfig cfg;
      uint8_t sessionPayload[64];
      const size_t n = cfg.encodeTo(sessionPayload, sizeof(sessionPayload));
      if (!sendFrame(client, MsgType::ConfigureSession, 0, 0, 0,
                     sessionPayload, n)) {
        Serial.println("[net] configure send failed");
        client.stop();
        return;
      }
      Serial.println("[net] session configured");
      postAvatarEvent(evCode(echoform::EchoEvent::ProviderConnected));
    }
    return;
  }
  // Non-request frames first (echoform_net.rs dispatch): keepalive and the
  // settings extension live outside the conversation request tracker.
  switch (header.msgType) {
    case MsgType::Pong: {
      const uint32_t rtt = millis() - pingSentMs_;
      lastRttMs_.store(rtt > 9999 ? 9999 : rtt);
      pongCount_.fetch_add(1);
      return;
    }
    case MsgType::Ping:
      return;  // relay keepalive; nothing required of us
    case MsgType::AmbientFeatures:
    case MsgType::EmotionHint:
    case MsgType::Usage:
      return;  // ignore on arrival (docs/ECHOFORM.md); M3+ may consume some
    case MsgType::NowPlaying: {
      String track;
      track.reserve(bodyLen);
      for (size_t i = 0; i < bodyLen; ++i) {
        track += static_cast<char>(body[i]);
      }
      Serial.printf("[net] now playing: %s\n",
                    track.isEmpty() ? "<nothing>" : track.c_str());
      uiSetNowPlaying(body, bodyLen);
      return;
    }
    default:
      break;
  }

  // Conversation frames: validate against the active request.
  if (sessionTracker_ == nullptr || sessionTicks_ == nullptr) return;
  switch (sessionTracker_->check(header, *sessionTicks_)) {
    case echo1::SequenceCheck::Stale:
      return;  // late frames after cancel/completion: drop silently
    case echo1::SequenceCheck::Gap:
      Serial.println("[net] sequence gap (resynced)");
      break;
    case echo1::SequenceCheck::Ok:
      break;
  }

  switch (header.msgType) {
    case MsgType::TranscriptPartial:
    case MsgType::TranscriptFinal: {
      String text;
      text.reserve(bodyLen);
      for (size_t i = 0; i < bodyLen; ++i) {
        text += static_cast<char>(body[i]);
      }
      Serial.printf("[net] %s: %s\n",
                    header.msgType == MsgType::TranscriptFinal ? "you said"
                                                               : "hearing",
                    text.c_str());
      uiSetUser(body, bodyLen);
      if (header.msgType == MsgType::TranscriptFinal) {
        if (xSemaphoreTake(uiMutex_, pdMS_TO_TICKS(20)) == pdTRUE) {
          uiUserFinal_ = utf8ToString(body, bodyLen);
          xSemaphoreGive(uiMutex_);
        }
        finalCount_.fetch_add(1);
      }
      postAvatarEvent(evCode(
          header.msgType == MsgType::TranscriptFinal
              ? echoform::EchoEvent::TranscriptFinal
              : echoform::EchoEvent::TranscriptPartial));
      break;
    }
    case MsgType::ModelStatus: {
      if (bodyLen >= 1) {
        modelStatus_.store(body[0]);
        static const char *kNames[] = {"idle", "transcribing", "thinking",
                                       "synthesising", "streaming"};
        Serial.printf("[net] status: %s\n",
                      body[0] <= 4 ? kNames[body[0]] : "?");
      }
      break;
    }
    case MsgType::TextDelta: {
      // Assistant text stream; print raw so the bench reads the reply.
      for (size_t i = 0; i < bodyLen; ++i) {
        Serial.write(body[i]);
      }
      uiAppendAssist(body, bodyLen);
      // Coalesce delta events: they only pump activity/osc, so one per
      // ~100 ms carries the same signal without flooding the queue.
      {
        static uint32_t sLastDeltaPostMs = 0;
        const uint32_t now = millis();
        if (now - sLastDeltaPostMs >= 100) {
          sLastDeltaPostMs = now;
          postAvatarEvent(evCode(echoform::EchoEvent::ProviderDelta));
        }
      }
      break;
    }
    case MsgType::SpeechPcm16: {
      if (audio_ != nullptr && bodyLen >= 2) {
        // Reinterpret little-endian pairs in place (ESP32 is LE).
        audio_->pushSpeech(reinterpret_cast<const int16_t *>(body),
                           bodyLen / 2);
      }
      break;
    }
    case MsgType::Completed: {
      sessionTracker_->complete();
      requestActive_.store(false);
      // The relay's trailing ModelStatus(idle) frame arrives after
      // complete() marks the request done, so the tracker drops it as
      // stale and the chip would say SPEAKING forever. Reset here.
      modelStatus_.store(0);
      if (audio_ != nullptr) {
        audio_->speechStreamEnded();
      }
      Serial.println("\n[net] request completed");
      postAvatarEvent(evCode(echoform::EchoEvent::RequestCompleted));
      break;
    }
    case MsgType::Error: {
      uint16_t code = 0;
      if (bodyLen >= 3) {
        code = static_cast<uint16_t>(body[0] | (body[1] << 8));
      }
      Serial.printf("[net] relay error %u\n", code);
      sessionTracker_->cancel();
      requestActive_.store(false);
      modelStatus_.store(0);
      postAvatarEvent(evCode(echoform::EchoEvent::NetworkError));
      break;
    }
    default:
      break;
  }
}
