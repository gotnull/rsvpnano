#pragma once

#include <Arduino.h>

#include <atomic>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "echoform/Echo1.h"
#include "network/OtaManager.h"

class EchoformAudio;

// ECHO1 relay client (docs/ECHOFORM.md M1): WiFi bring-up plus a TCP session
// to the echoform relay, transcribed from rusty-nail's echoform_net.rs
// session shape - Hello -> HelloAck -> ConfigureSession, 30 ms multiplex
// tick, 5 s keepalive Ping, reconnect with 2 s backoff. Runs entirely on its
// own FreeRTOS task; the render loop reads the published atomics only.
//
// Endpoint config: /echoform.json on the SD card,
//   {"host": "192.168.1.10", "port": 8125}
// (no compile-time endpoint baking - that was a rusty-nail convention).
class EchoformNet {
 public:
  // Link state, same meaning as rusty-nail's NET_STATE: 0 = no IP,
  // 1 = IP up but no relay session, 2 = relay session live.
  enum class LinkState : uint8_t { NoIp = 0, NoRelay = 1, SessionLive = 2 };

  // Spawns the worker task. SD must be mounted (config lives there).
  // audio may be null (M1 bench mode: no utterances).
  bool begin(EchoformAudio *audio = nullptr);

  // -- utterance commands (any task; queued to the net task) --
  // BeginUtterance flags: bit0 wake candidate, bit1 trusted follow-up.
  void beginUtterance(uint16_t flags);
  void endUtterance();
  void cancelUtterance();
  bool requestActive() const { return requestActive_.load(); }
  uint8_t modelStatus() const { return modelStatus_.load(); }
  // Copies the current transcript / now-playing strings for the UI.
  void uiSnapshot(String &user, String &assist, String &nowPlaying);
  // Clears the transcript lines (the app calls this after conversation
  // idle so the overlay goes fully quiet - no chips, no marquee work).
  void clearConversationUi() { uiClearConversation(); }
  // Voice-intent hook: bumps on every FINAL transcript; fetch the text
  // with finalTranscript().
  uint32_t finalTranscriptCounter() const { return finalCount_.load(); }
  String finalTranscript();
  // Avatar events for the face controller: net-side happenings queued for
  // the render loop to drain each frame (codes are echoform::EchoEvent).
  bool drainAvatarEvent(uint8_t &out) {
    return eventQueue_ != nullptr &&
           xQueueReceive(eventQueue_, &out, 0) == pdTRUE;
  }

  LinkState linkState() const {
    return static_cast<LinkState>(linkState_.load());
  }
  uint32_t lastRttMs() const { return lastRttMs_.load(); }
  uint32_t pongCount() const { return pongCount_.load(); }
  uint32_t sessionDrops() const { return sessionDrops_.load(); }
  // Static string; safe to read cross-task.
  const char *lastDropReason() const { return lastDropReason_.load(); }
  bool relayConfigured() const { return relayConfigured_.load(); }
  // Rewrites /echoform.json and asks the task to reconnect with it. token
  // (optional) is the shared secret for publicly reachable relays.
  bool setRelayEndpoint(const String &host, uint16_t port,
                        const String &token = String());

 private:
  static void taskTrampoline(void *arg);
  void taskRun();
  bool loadConfigs();
  // One connected session; returns a static reason string for the log.
  const char *session(class WiFiClient &client);
  void handleFrame(class WiFiClient &client, const echo1::FrameHeader &header,
                   const uint8_t *body, size_t bodyLen, bool &helloAcked);
  bool sendFrame(class WiFiClient &client, echo1::MsgType type,
                 uint16_t requestId, uint16_t sequence, uint16_t flags,
                 const uint8_t *payload, size_t payloadLen);

  enum class CmdType : uint8_t { Begin, End, Cancel };
  struct Cmd {
    CmdType type;
    uint16_t flags;
  };

  // Flush the TX ring onto the wire; returns false on socket death.
  bool pumpMic(class WiFiClient &client, uint16_t requestId,
               uint16_t &outSeq);
  bool handleCmd(class WiFiClient &client, const Cmd &cmd,
                 echo1::RequestTracker &tracker, uint32_t ticks,
                 uint16_t &outSeq, const char *&why);

  std::atomic<uint8_t> linkState_{0};
  std::atomic<uint32_t> lastRttMs_{0};
  std::atomic<uint32_t> pongCount_{0};
  std::atomic<bool> relayConfigured_{false};
  std::atomic<bool> reloadRequested_{false};
  std::atomic<bool> requestActive_{false};
  std::atomic<uint8_t> modelStatus_{0};
  std::atomic<uint32_t> sessionDrops_{0};
  std::atomic<const char *> lastDropReason_{"none"};

  EchoformAudio *audio_ = nullptr;
  QueueHandle_t cmdQueue_ = nullptr;
  QueueHandle_t eventQueue_ = nullptr;
  void postAvatarEvent(uint8_t code) {
    if (eventQueue_ != nullptr) xQueueSend(eventQueue_, &code, 0);
  }
  // Valid only within session(); lets handleFrame reach the live tracker.
  echo1::RequestTracker *sessionTracker_ = nullptr;
  const uint32_t *sessionTicks_ = nullptr;

  // Transcript strings shared with the render side under a mutex; writes
  // happen on the net task only.
  SemaphoreHandle_t uiMutex_ = nullptr;
  std::atomic<uint32_t> finalCount_{0};
  String uiUserFinal_;
  String uiUser_;
  String uiAssist_;
  String uiNowPlaying_;
  void uiSetUser(const uint8_t *utf8, size_t len);
  void uiAppendAssist(const uint8_t *utf8, size_t len);
  void uiClearConversation();
  void uiSetNowPlaying(const uint8_t *utf8, size_t len);

  // Task-owned state (no access from other tasks after begin()).
  std::vector<OtaManager::Network> networks_;
  String relayHost_;
  uint16_t relayPort_ = 0;
  String relayToken_;
  static constexpr int kFailsBeforeWifiReset = 5;
  int relayConnectFails_ = 0;
  echo1::Parser parser_;
  uint32_t pingSentMs_ = 0;
  uint8_t frameOut_[echo1::kHeaderLen + 1024 + echo1::kCrcLen];
  uint8_t readBuf_[1024];
  int16_t pcm_[512];
  uint8_t pcmBytes_[1024];
};
