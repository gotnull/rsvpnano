#include "echoform/EchoformApp.h"

#include <SD_MMC.h>
#include <WiFi.h>
#include <Wire.h>

#include "audio/Es7210.h"
#include "board/BoardConfig.h"
#include "network/WifiConnector.h"

namespace {

// 60 fps frame cap; the native-stripe renderer + SPI flush land in the
// ~12-16 ms slot naturally, this just keeps the loop predictable.
constexpr uint32_t kFrameIntervalMs = 16;
// Same guardrail as the reader OS: any tick over 33 ms is blocking work
// that has no business on this loop.
constexpr uint32_t kSlowTickWarnUs = 33000;
constexpr uint32_t kMicRecordSeconds = 5;
constexpr char kRecWavPath[] = "/rec.wav";

}  // namespace

void EchoformApp::begin() {
  Serial.println("[echoform] standalone firmware boot");

  // Text orientation for how the device is held (the bobs are agnostic,
  // the transcript strip is not). Fulvio holds Echoform speaker-on-top,
  // which is the rotated orientation.
  display_.setUiRotated(true);
  const bool displayReady = display_.begin();
  if (!displayReady) {
    Serial.println("[echoform] display init FAILED");
  }

  // SD first (config + /rec.wav), then audio. Neither failing is fatal —
  // the starfield must come up regardless so the device never looks dead.
  sdReady_ = storage_.begin();
  if (!sdReady_) {
    Serial.println("[echoform] SD mount failed; REC and config unavailable");
  }
  if (!audio_.begin()) {
    Serial.println("[echoform] audio init failed");
  }
  // Volume: default 100%, voice-adjustable ("set volume to 60"), persisted.
  prefs_.begin("echoform", false);
  audio_.setVolumePercent(prefs_.getUChar("vol", 100));

  // Boot-time self-update (SD + network must be checked before workers
  // spawn; blocking is fine here, the device is still booting).
  maybeBootOta();

  // The wave field buffers (RN-native 128x128 index + phosphor).
  waveFieldFb_ = static_cast<uint8_t *>(
      malloc(echoform::kFieldW * echoform::kFieldH));
  waveFieldGlow_ = static_cast<uint8_t *>(
      malloc(echoform::kFieldW * echoform::kFieldH));
  faceFieldFb_ = static_cast<uint8_t *>(
      malloc(echoform::kFieldW * echoform::kFieldH));
  if (faceFieldFb_ != nullptr) {
    memset(faceFieldFb_, 0, echoform::kFieldW * echoform::kFieldH);
  }
  if (waveFieldFb_ != nullptr && waveFieldGlow_ != nullptr) {
    memset(waveFieldFb_, 0, echoform::kFieldW * echoform::kFieldH);
    memset(waveFieldGlow_, 0, echoform::kFieldW * echoform::kFieldH);
  }

  // Audio pipe (M2) + ECHO1 relay client (M1): worker tasks own I2S and
  // WiFi/TCP; the render loop reads atomics only.
  echoAudio_.begin(&audio_);
  if (sdReady_) {
    net_.begin(&echoAudio_);
  }
  bootButton_.begin();

  screensaver_.begin(millis());
  overlay_.hideBobs = true;  // the face replaces the dots; stars stay
  Serial.println("[echoform] entering the starfield");
}

namespace {

// Normalise a timestamp tag to a comparable "YYYYMMDDHHMMSS" digit string.
// Handles both release tags ("ota-20260807050200") and build tags
// ("260807-171530", %y%m%d-%H%M%S). Empty result = not comparable.
String normalizeStamp(const String &tag) {
  String digits;
  digits.reserve(tag.length());
  for (size_t i = 0; i < tag.length(); ++i) {
    if (isdigit(tag[i])) digits += tag[i];
  }
  if (digits.length() == 12) {
    digits = String("20") + digits;  // yy -> 20yy
  }
  return digits.length() == 14 ? digits : String();
}

}  // namespace

void EchoformApp::otaStatusTrampoline(void *context, const char *title,
                                      const char *line1, const char *line2,
                                      int progressPercent) {
  auto *self = static_cast<EchoformApp *>(context);
  self->display_.renderProgress(title, line1, line2,
                                progressPercent < 0
                                    ? 0
                                    : static_cast<uint8_t>(progressPercent));
}

void EchoformApp::maybeBootOta() {
#ifndef RSVP_BUILD_TAG
  return;  // no build stamp, no comparison basis
#else
  if (!sdReady_) return;
  OtaManager ota;
  if (!ota.loadConfigFromSd()) {
    Serial.println("[ota] no wifi.json; skipping boot update check");
    return;
  }
  // Echoform's own asset on the shared release channel.
  OtaManager::Config cfg = ota.config();
  const int slash = cfg.firmwareUrl.lastIndexOf('/');
  if (slash < 0) return;
  cfg.firmwareUrl = cfg.firmwareUrl.substring(0, slash + 1) + "echoform.bin";
  ota.setConfig(cfg);
  ota.setStatusCallback(&EchoformApp::otaStatusTrampoline, this);

  display_.renderProgress("ECHOFORM", "Checking for update", "", 5);
  if (!WifiConnector::connect(cfg.networks, 15000, "echoform-ota")) {
    Serial.println("[ota] wifi unavailable; booting current build");
    return;
  }
  WiFi.setSleep(false);
  const String releaseTag = ota.fetchLatestReleaseTag();
  const String releaseStamp = normalizeStamp(releaseTag);
  const String buildStamp = normalizeStamp(RSVP_BUILD_TAG);
  Serial.printf("[ota] build %s vs release %s\n", RSVP_BUILD_TAG,
                releaseTag.isEmpty() ? "<none>" : releaseTag.c_str());
  if (releaseStamp.isEmpty() || buildStamp.isEmpty() ||
      releaseStamp <= buildStamp) {
    Serial.println("[ota] up to date");
    return;
  }
  Serial.println("[ota] newer release available; updating");
  display_.renderProgress("ECHOFORM UPDATE", releaseTag, "Downloading", 10);
  if (ota.runUpdate()) {
    display_.renderProgress("ECHOFORM UPDATE", "Complete", "Restarting", 100);
    delay(800);
    ESP.restart();
  }
  Serial.printf("[ota] update failed: %s\n", ota.lastError().c_str());
#endif
}

void EchoformApp::handleVoiceIntents() {
  const uint32_t counter = net_.finalTranscriptCounter();
  if (counter == lastFinalHandled_) return;
  lastFinalHandled_ = counter;
  String text = net_.finalTranscript();
  text.toLowerCase();
  if (text.indexOf("volume") < 0) return;
  int level = -1;
  if (text.indexOf("mute") >= 0) {
    level = 0;
  } else if (text.indexOf("full") >= 0 || text.indexOf("max") >= 0 ||
             text.indexOf("hundred") >= 0) {
    level = 100;
  } else if (text.indexOf("half") >= 0) {
    level = 50;
  } else {
    // First 1-3 digit number in the utterance.
    for (int i = 0; i < static_cast<int>(text.length()); ++i) {
      if (isdigit(text[i])) {
        int v = 0;
        int j = i;
        while (j < static_cast<int>(text.length()) && isdigit(text[j])) {
          v = v * 10 + (text[j] - '0');
          ++j;
        }
        if (v >= 0 && v <= 100) level = v;
        break;
      }
    }
  }
  if (level < 0) return;
  audio_.setVolumePercent(static_cast<uint8_t>(level));
  prefs_.putUChar("vol", static_cast<uint8_t>(level));
  Serial.printf("[echoform] volume set to %d%% by voice\n", level);
}

bool EchoformApp::startUtterance() {
  if (pttActive_) return true;
  Serial.printf(
      "[echoform] PTT gates: link=%d reqActive=%d speechBusy=%d rec=%d\n",
      static_cast<int>(net_.linkState()), net_.requestActive() ? 1 : 0,
      echoAudio_.speechBusy() ? 1 : 0, micRecorder_.recording() ? 1 : 0);
  if (net_.linkState() != EchoformNet::LinkState::SessionLive) {
    Serial.println("[echoform] can't talk: no relay session");
    return false;
  }
  if (net_.requestActive() || echoAudio_.speechBusy()) {
    Serial.println("[echoform] can't talk: pipeline busy");
    return false;
  }
  if (micRecorder_.recording()) {
    Serial.println("[echoform] can't talk: REC in progress");
    return false;
  }
  // Capture bring-up (I2S reinstall + codec init, ~20 ms) happens on the
  // net task when it dequeues the Begin command — never on this loop.
  net_.beginUtterance(0);
  pttActive_ = true;
  controller_.handle(echoform::EchoEvent::PushToTalkPressed);
  Serial.println("[echoform] listening...");
  return true;
}

void EchoformApp::finishUtterance() {
  if (!pttActive_) return;
  pttActive_ = false;
  talkDeadlineMs_ = 0;
  // Stop NEW samples; the net task flushes the ring tail, sends
  // EndUtterance, and hands the I2S port back for playback.
  echoAudio_.stopCapture();
  net_.endUtterance();
  controller_.handle(echoform::EchoEvent::PushToTalkReleased);
  Serial.println("[echoform] sent; waiting for reply");
}

void EchoformApp::updateOverlay(uint32_t nowMs) {
  // 10 Hz is plenty for text; String copies + truncation stay off the
  // per-frame path.
  if (nowMs - lastOverlayMs_ < 100) return;
  lastOverlayMs_ = nowMs;

  // Status chip.
  const char *chip = "";
  switch (net_.linkState()) {
    case EchoformNet::LinkState::NoIp:
      chip = net_.relayConfigured() ? "NO IP" : "NO RELAY CFG";
      break;
    case EchoformNet::LinkState::NoRelay:
      chip = "NO RELAY";
      break;
    case EchoformNet::LinkState::SessionLive:
      if (pttActive_) {
        chip = "LISTENING";
      } else if (echoAudio_.speechBusy()) {
        chip = "SPEAKING";
      } else {
        switch (net_.modelStatus()) {
          case 1: chip = "TRANSCRIBING"; break;
          case 2: chip = "THINKING"; break;
          case 3:
          case 4: chip = "SPEAKING"; break;
          default: chip = ""; break;  // idle: the bobs are the show
        }
      }
      break;
  }
  snprintf(overlay_.chip, sizeof(overlay_.chip), "%s", chip);

  // Transcript lines + now playing: pass FULL strings (up to the buffer
  // caps) — the chip widget's marquee handles overflow. For strings past
  // the cap keep the tail (the freshest words while streaming).
  String user, assist, nowPlaying;
  net_.uiSnapshot(user, assist, nowPlaying);
  auto tailInto = [](char *dst, size_t cap, const String &s) {
    // The tiny font is ASCII-only: translate common UTF-8 punctuation
    // (em-dash, curly quotes, ellipsis) instead of letting each byte fall
    // back to a space (the "massive gap" bug), drop other multibyte
    // sequences, and collapse whitespace runs.
    String clean;
    clean.reserve(s.length());
    bool lastSpace = false;
    for (size_t i = 0; i < s.length(); ++i) {
      const uint8_t c = static_cast<uint8_t>(s[i]);
      if (c >= 0x80) {
        // UTF-8 sequence: match the common punctuation, skip the rest.
        if (c == 0xE2 && i + 2 < s.length() &&
            static_cast<uint8_t>(s[i + 1]) == 0x80) {
          const uint8_t b3 = static_cast<uint8_t>(s[i + 2]);
          const char *sub = nullptr;
          if (b3 == 0x93 || b3 == 0x94) sub = "-";        // en/em dash
          else if (b3 == 0x98 || b3 == 0x99) sub = "'";   // curly single
          else if (b3 == 0x9C || b3 == 0x9D) sub = "\""; // curly double
          else if (b3 == 0xA6) sub = "...";               // ellipsis
          if (sub != nullptr) {
            clean += sub;
            lastSpace = false;
          }
          i += 2;
        } else {
          // Skip continuation bytes of any other sequence.
          while (i + 1 < s.length() &&
                 (static_cast<uint8_t>(s[i + 1]) & 0xC0) == 0x80) {
            ++i;
          }
        }
        continue;
      }
      if (isspace(c)) {
        if (!lastSpace && clean.length() > 0) clean += ' ';
        lastSpace = true;
      } else {
        clean += static_cast<char>(c);
        lastSpace = false;
      }
    }
    const size_t maxChars = cap - 1;
    if (clean.length() <= maxChars) {
      snprintf(dst, cap, "%s", clean.c_str());
    } else {
      snprintf(dst, cap, "%s", clean.c_str() + (clean.length() - maxChars));
    }
  };
  tailInto(overlay_.line1, sizeof(overlay_.line1), user);
  tailInto(overlay_.line2, sizeof(overlay_.line2), assist);
  tailInto(overlay_.nowPlaying, sizeof(overlay_.nowPlaying), nowPlaying);

  // Conversation-idle sweep: once nothing has been active for a while,
  // clear the transcript so the overlay stops drawing entirely (empty
  // strings draw no chips, no marquee — the screensaver runs free).
  constexpr uint32_t kTranscriptLingerMs = 20000;
  const bool conversationBusy =
      pttActive_ || net_.requestActive() || echoAudio_.speechBusy();
  if (conversationBusy) {
    lastConversationMs_ = nowMs;
  } else if ((user.length() > 0 || assist.length() > 0) &&
             lastConversationMs_ != 0 &&
             nowMs - lastConversationMs_ > kTranscriptLingerMs) {
    net_.clearConversationUi();
  }
}

void EchoformApp::updatePushToTalk(uint32_t nowMs) {
  bootButton_.update(nowMs);
  if (bootButton_.wasPressedEvent()) {
    startUtterance();
  }
  if (bootButton_.wasReleasedEvent() && pttActive_ && talkDeadlineMs_ == 0) {
    finishUtterance();
  }
  if (talkDeadlineMs_ != 0 && nowMs >= talkDeadlineMs_) {
    finishUtterance();
  }
}

void EchoformApp::update(uint32_t nowMs) {
  const uint32_t tickStartUs = micros();

  pollMicRecorderDebug(nowMs);
  updatePushToTalk(nowMs);
  updateOverlay(nowMs);

  if (nowMs - lastRenderMs_ >= kFrameIntervalMs) {
    lastRenderMs_ = nowMs;
    // Frame-stage profiling: worst per second, printed with the heartbeat.
    static uint32_t sStageCtrlUs = 0, sStageSimUs = 0, sStageComposeUs = 0,
                    sStageRenderUs = 0;
    static uint32_t sStageLogMs = 0;
    uint32_t stageT = micros();
    auto stage = [&stageT](uint32_t &acc) {
      const uint32_t now = micros();
      if (now - stageT > acc) acc = now - stageT;
      stageT = now;
    };
    // The rusty-nail visual system, drive mapping transcribed from fcecho
    // engine.rs. The wave is ALWAYS alive - idle it breathes; voice
    // erupts through it. Which stream drives it depends on state:
    // speaking uses the outgoing TTS levels (already normalised),
    // listening uses the mic through the visual AGC.
    EchoformAudio::LiveFeatures f{};
    const bool fresh = echoAudio_.featuresSnapshot(f);
    const bool speaking = echoAudio_.speechBusy();

    // Avatar controller: drain net events, add app-side ones, then step
    // exactly once per frame (state.rs contract). The mouth articulates
    // from the real playback PCM features.
    uint8_t eventCode;
    while (net_.drainAvatarEvent(eventCode)) {
      controller_.handle(static_cast<echoform::EchoEvent>(eventCode));
    }
    if (speaking != speechWasBusy_) {
      controller_.handle(speaking ? echoform::EchoEvent::SpeechAudioStarted
                                  : echoform::EchoEvent::SpeechAudioEnded);
      speechWasBusy_ = speaking;
    }
    if (pttActive_ && fresh) {
      controller_.handle(f.voice ? echoform::EchoEvent::VoiceStarted
                                 : echoform::EchoEvent::VoiceEnded);
      controller_.handle(echoform::EchoEvent::AudioBlockReady);
    }
    controller_.step(speaking ? f.rms : 0.0f, speaking ? f.low : 0.0f,
                     speaking ? f.mid : 0.0f);
    handleVoiceIntents();
    stage(sStageCtrlUs);

    float rms = 0.0f, transient = 0.0f, high = 0.0f;
    if (fresh && (echoAudio_.capturing() || speaking)) {
      if (speaking) {
        rms = f.rms;
        transient = f.transient;
        high = f.high;
      } else {
        // engine.rs agc_mic: peak tracker rises instantly, ~11 s
        // half-life decay, floor stops silence being amplified; scale
        // targets recent peaks at ~0.7, clamped 1..8x.
        agcPeak_ = agcPeak_ * 0.999f;
        if (f.rms > agcPeak_) agcPeak_ = f.rms;
        if (agcPeak_ < 0.06f) agcPeak_ = 0.06f;
        float scale = 0.7f / agcPeak_;
        if (scale < 1.0f) scale = 1.0f;
        if (scale > 8.0f) scale = 8.0f;
        auto b = [scale](float v) {
          v *= scale;
          return v > 1.0f ? 1.0f : v;
        };
        rms = b(f.rms);
        transient = b(f.transient);
        high = b(f.high);
      }
    }
    const bool voiceLive = rms > 0.02f;
    if (voiceLive) {
      wave_.excite(rms);
      waveThickness_ += (rms - waveThickness_) * (1.0f / 6.0f);
    } else {
      waveThickness_ *= 0.9f;
      // Idle attract only: the dream reel's rolling ocean (dream.rs Copper
      // drive). Stands down the moment the avatar leaves Idle so the calm
      // chin band can form under the face.
      if (controller_.state() == echoform::AvatarState::Idle) {
        wave_.excite(0.14f);
        if (waveFrame_ % 40 == 0) {
          waveRng_ = waveRng_ * 1664525u + 1013904223u;
          const size_t x = 16 + ((waveRng_ >> 16) % 96);
          wave_.impulse(x, 2.0f);
        }
      }
    }
    if (transient > 0.12f) {
      // Transients splash at a column in the middle half; droplet count
      // scales with the hit (engine.rs).
      waveRng_ = waveRng_ * 1664525u + 1013904223u;
      const size_t x = 32 + ((waveRng_ >> 16) % 64);
      float strength = transient * 3.0f;
      if (strength > 4.0f) strength = 4.0f;
      wave_.impulse(x, strength);
      const float surface =
          echoform::faceBaseY(controller_.coherence()) + wave_.points()[x].y;
      int drops = 1 + static_cast<int>(transient * 4.0f);
      if (drops > 5) drops = 5;
      for (int d = 0; d < drops; ++d) {
        particles_.splash(x, strength * 0.5f, surface);
      }
    }
    if (high > 0.0f) {
      wave_.turbulence(high);
    }
    float coherence = controller_.coherence();
    if (faceForceUntilMs_ != 0) {
      if (nowMs < faceForceUntilMs_) {
        coherence = 1.0f;
      } else {
        faceForceUntilMs_ = 0;
      }
    }
    const float baseY = echoform::faceBaseY(coherence);
    // Breath amplitude fades as the face forms (wave.rs step contract).
    wave_.step(1.0f - echoform::fxSmoothstep(coherence));
    particles_.step(wave_, baseY, waveFrame_++, rms);
    stage(sStageSimUs);
    if (waveFieldFb_ != nullptr) {
      echoform::composeField(waveFieldFb_, waveFieldGlow_, wave_, particles_,
                             waveThickness_, baseY, coherence);
      overlay_.waveField = waveFieldFb_;
      overlay_.wavePresence = 1.0f;
    }
    if (faceFieldFb_ != nullptr) {
      // Alternate-frame face composition: the float port of RN's integer
      // sampler costs ~11 ms when the head is formed; every second frame
      // halves that and the per-cell temporal smoothing hides it.
      // TODO: integer-ise EchoFace's hot loop like the original and go
      // back to every frame.
      if ((waveFrame_ & 1) == 0) {
        echoform::FaceFrame face{&controller_.pose(), &controller_.headPose(),
                                 coherence, facePrevE_};
        echoform::composeFaceField(faceFieldFb_, face);
      }
      overlay_.faceField = faceFieldFb_;
    }
    stage(sStageComposeUs);
    display_.renderScreensaverFrame(screensaver_, &overlay_);
    stage(sStageRenderUs);
    if (nowMs - sStageLogMs >= 1000) {
      sStageLogMs = nowMs;
      Serial.printf("[echoperf] ctrl=%lu sim=%lu compose=%lu render=%lu us\n",
                    static_cast<unsigned long>(sStageCtrlUs),
                    static_cast<unsigned long>(sStageSimUs),
                    static_cast<unsigned long>(sStageComposeUs),
                    static_cast<unsigned long>(sStageRenderUs));
      sStageCtrlUs = sStageSimUs = sStageComposeUs = sStageRenderUs = 0;
    }
  }

  // Once-per-second heartbeat, same shape as the reader OS's so the bench
  // tooling reads both.
  static uint32_t sHbLastMs = 0;
  static uint32_t sHbTicks = 0;
  static uint32_t sHbMaxTickUs = 0;
  const uint32_t tickUs = micros() - tickStartUs;
  ++sHbTicks;
  if (tickUs > sHbMaxTickUs) sHbMaxTickUs = tickUs;
  if (tickUs > kSlowTickWarnUs) {
    Serial.printf("[tick] SLOW %lu us\n", static_cast<unsigned long>(tickUs));
  }
  if (nowMs - sHbLastMs >= 1000) {
    Serial.printf(
        "[heart] t=%lu ticks=%lu heap=%u min=%u maxTick=%lu us rec=%d "
        "net=%u rtt=%lu pongs=%lu coh=%d st=%d rssi=%d drops=%lu(%s)\n",
        static_cast<unsigned long>(nowMs),
        static_cast<unsigned long>(sHbTicks),
        static_cast<unsigned>(ESP.getFreeHeap()),
        static_cast<unsigned>(ESP.getMinFreeHeap()),
        static_cast<unsigned long>(sHbMaxTickUs),
        micRecorder_.recording() ? 1 : 0,
        static_cast<unsigned>(net_.linkState()),
        static_cast<unsigned long>(net_.lastRttMs()),
        static_cast<unsigned long>(net_.pongCount()),
        static_cast<int>(controller_.coherence() * 100.0f),
        static_cast<int>(controller_.state()),
        static_cast<int>(WiFi.RSSI()),
        static_cast<unsigned long>(net_.sessionDrops()),
        net_.lastDropReason());
    sHbLastMs = nowMs;
    sHbTicks = 0;
    sHbMaxTickUs = 0;
  }
}

void EchoformApp::pollMicRecorderDebug(uint32_t nowMs) {
  (void)nowMs;
  if (micRecorder_.takeFinished()) {
    Serial.printf("[echoform] mic recording %s\n",
                  micRecorder_.lastSucceeded() ? "saved to /rec.wav" : "FAILED");
  }

  static String lineBuffer;
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());
    if (c == '\r') {
      continue;
    }
    if (c != '\n') {
      lineBuffer += c;
      if (lineBuffer.length() > 64) {
        lineBuffer = "";
      }
      continue;
    }
    lineBuffer.trim();
    const bool isRecCommand = lineBuffer.equalsIgnoreCase("REC");
    // Bench diagnostics for the M0 level bug: "I2C" scans Wire1, "ESRW"
    // write/readback-tests an ES7210 scratch register both with a repeated
    // start and with a full stop between write and read.
    if (lineBuffer.equalsIgnoreCase("I2C")) {
      lineBuffer = "";
      String found = "[echoform] i2c bus 1:";
      for (uint8_t addr = 0x08; addr <= 0x77; ++addr) {
        Wire1.beginTransmission(addr);
        if (Wire1.endTransmission() == 0) {
          char buf[8];
          snprintf(buf, sizeof(buf), " 0x%02X", addr);
          found += buf;
        }
      }
      Serial.println(found);
      continue;
    }
    if (lineBuffer.equalsIgnoreCase("ESRW")) {
      lineBuffer = "";
      Es7210::probeRegisterReadback();
      continue;
    }
    // "FACE": force full coherence for 15 s (bench: materialise the head).
    if (lineBuffer.equalsIgnoreCase("FACE")) {
      lineBuffer = "";
      faceForceUntilMs_ = millis() + 15000;
      Serial.println("[echoform] forcing coherence 1.0 for 15 s");
      continue;
    }
    // "FIELD": dump the composed 128x128 index field as base64 (bench).
    if (lineBuffer.equalsIgnoreCase("FIELD")) {
      lineBuffer = "";
      if (waveFieldFb_ != nullptr) {
        static const char b64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        Serial.setTxTimeoutMs(500);
        Serial.println("---FIELD-BEGIN---");
        char line[89];
        int o = 0;
        for (int i = 0; i < 128 * 128; i += 3) {
          const uint32_t b0 = waveFieldFb_[i];
          const uint32_t b1 = i + 1 < 128 * 128 ? waveFieldFb_[i + 1] : 0;
          const uint32_t b2 = i + 2 < 128 * 128 ? waveFieldFb_[i + 2] : 0;
          const uint32_t n = (b0 << 16) | (b1 << 8) | b2;
          line[o++] = b64[(n >> 18) & 0x3F];
          line[o++] = b64[(n >> 12) & 0x3F];
          line[o++] = b64[(n >> 6) & 0x3F];
          line[o++] = b64[n & 0x3F];
          if (o >= 88) {
            line[o] = '\0';
            Serial.println(line);
            o = 0;
          }
        }
        if (o > 0) {
          line[o] = '\0';
          Serial.println(line);
        }
        Serial.println("---FIELD-END---");
        Serial.setTxTimeoutMs(0);
      }
      continue;
    }
    // "FFIELD": dump the FACE field as base64 (bench).
    if (lineBuffer.equalsIgnoreCase("FFIELD")) {
      lineBuffer = "";
      if (faceFieldFb_ != nullptr) {
        static const char b64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        Serial.setTxTimeoutMs(500);
        Serial.println("---FFIELD-BEGIN---");
        char line[89];
        int o = 0;
        for (int i = 0; i < 128 * 128; i += 3) {
          const uint32_t b0 = faceFieldFb_[i];
          const uint32_t b1 = i + 1 < 128 * 128 ? faceFieldFb_[i + 1] : 0;
          const uint32_t b2 = i + 2 < 128 * 128 ? faceFieldFb_[i + 2] : 0;
          const uint32_t n = (b0 << 16) | (b1 << 8) | b2;
          line[o++] = b64[(n >> 18) & 0x3F];
          line[o++] = b64[(n >> 12) & 0x3F];
          line[o++] = b64[(n >> 6) & 0x3F];
          line[o++] = b64[n & 0x3F];
          if (o >= 88) {
            line[o] = '\0';
            Serial.println(line);
            o = 0;
          }
        }
        if (o > 0) {
          line[o] = '\0';
          Serial.println(line);
        }
        Serial.println("---FFIELD-END---");
        Serial.setTxTimeoutMs(0);
      }
      continue;
    }
    // "OFF": release the battery power hold so unplugging USB genuinely
    // cuts 3V3. Needed to cold-power-cycle the ES7210 (its register file
    // appears wedged since an early full-reset opcode, and the battery
    // means warm reboots never drop its rail). Replugging USB boots again.
    if (lineBuffer.equalsIgnoreCase("OFF")) {
      lineBuffer = "";
      Serial.println("[echoform] releasing power hold; unplug USB to cut power");
      BoardConfig::releaseBatteryPowerHold();
      continue;
    }
    // "PLAY": play /rec.wav back through the speaker, on a worker task so
    // the render loop never blocks. Closes the M0 loop audibly.
    if (lineBuffer.equalsIgnoreCase("PLAY")) {
      lineBuffer = "";
      if (micRecorder_.recording()) {
        Serial.println("[echoform] PLAY ignored: recording in progress");
      } else {
        xTaskCreatePinnedToCore(
            [](void *arg) {
              auto *self = static_cast<EchoformApp *>(arg);
              Serial.println("[echoform] playing /rec.wav");
              const uint32_t startMs = millis();
              self->audio_.setVolumePercent(100);
              const bool ok = self->audio_.playWavFromSd("/rec.wav");
              Serial.printf("[echoform] playback %s in %lu ms\n",
                            ok ? "done" : "FAILED",
                            static_cast<unsigned long>(millis() - startMs));
              vTaskDelete(nullptr);
            },
            "wav_play", 8192, this, tskIDLE_PRIORITY + 2, nullptr, 0);
      }
      continue;
    }
    // "DUMP": stream /rec.wav to the host as base64 between marker lines so
    // the bench can decode and listen/analyse on real speakers. Runs on a
    // worker; raises the CDC tx timeout for the duration so nothing drops
    // (the host must be draining the port).
    if (lineBuffer.equalsIgnoreCase("DUMP")) {
      lineBuffer = "";
      xTaskCreatePinnedToCore(
          [](void *arg) {
            (void)arg;
            File f = SD_MMC.open("/rec.wav");
            if (!f || f.isDirectory()) {
              Serial.println("[echoform] DUMP: /rec.wav open failed");
              if (f) f.close();
              vTaskDelete(nullptr);
              return;
            }
            Serial.setTxTimeoutMs(500);
            Serial.printf("---WAV-BEGIN %u---\n",
                          static_cast<unsigned>(f.size()));
            static const char b64[] =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            uint8_t in[720];
            char out[961];
            while (f.available()) {
              const int got = f.read(in, sizeof(in));
              if (got <= 0) break;
              int o = 0;
              for (int i = 0; i < got; i += 3) {
                const uint32_t b0 = in[i];
                const uint32_t b1 = (i + 1 < got) ? in[i + 1] : 0;
                const uint32_t b2 = (i + 2 < got) ? in[i + 2] : 0;
                const uint32_t n = (b0 << 16) | (b1 << 8) | b2;
                out[o++] = b64[(n >> 18) & 0x3F];
                out[o++] = b64[(n >> 12) & 0x3F];
                out[o++] = (i + 1 < got) ? b64[(n >> 6) & 0x3F] : '=';
                out[o++] = (i + 2 < got) ? b64[n & 0x3F] : '=';
              }
              out[o] = '\0';
              Serial.println(out);
            }
            f.close();
            Serial.println("---WAV-END---");
            Serial.setTxTimeoutMs(0);
            vTaskDelete(nullptr);
          },
          "wav_dump", 8192, nullptr, tskIDLE_PRIORITY + 2, nullptr, 0);
      continue;
    }
    // "TALK n": benchless push-to-talk — capture n seconds (default 4)
    // then send, as if BOOT had been held. The reply plays on the speaker.
    if (lineBuffer.startsWith("TALK") || lineBuffer.startsWith("talk")) {
      const long secs = lineBuffer.length() > 4
                            ? lineBuffer.substring(5).toInt()
                            : 0;
      lineBuffer = "";
      const uint32_t durationMs =
          (secs > 0 && secs <= 30) ? static_cast<uint32_t>(secs) * 1000 : 4000;
      if (startUtterance()) {
        talkDeadlineMs_ = millis() + durationMs;
        Serial.printf("[echoform] TALK: capturing %lu ms\n",
                      static_cast<unsigned long>(durationMs));
      }
      continue;
    }
    // "SETRELAY host:port": writes /echoform.json and reconnects. The
    // echoform build has no USB MSC UI, so the bench sets the endpoint
    // over serial.
    if (lineBuffer.startsWith("SETRELAY ") || lineBuffer.startsWith("setrelay ")) {
      const String arg = lineBuffer.substring(9);
      lineBuffer = "";
      const int colon = arg.lastIndexOf(':');
      if (colon <= 0) {
        Serial.println("[echoform] SETRELAY usage: SETRELAY 192.168.1.10:8125");
      } else {
        const String host = arg.substring(0, colon);
        const long port = arg.substring(colon + 1).toInt();
        if (port <= 0 || port > 65535) {
          Serial.println("[echoform] SETRELAY: bad port");
        } else {
          net_.setRelayEndpoint(host, static_cast<uint16_t>(port));
        }
      }
      continue;
    }
    // "BEEP": known-good RTTTL tone path — isolates speaker/amp problems
    // from WAV-specific ones.
    if (lineBuffer.equalsIgnoreCase("BEEP")) {
      lineBuffer = "";
      xTaskCreatePinnedToCore(
          [](void *arg) {
            auto *self = static_cast<EchoformApp *>(arg);
            Serial.println("[echoform] beep");
            const bool ok = self->audio_.playRtttl(
                "beep:d=8,o=6,b=120:c,e,g,4c7");
            Serial.printf("[echoform] beep %s\n", ok ? "done" : "FAILED");
            vTaskDelete(nullptr);
          },
          "beep_play", 8192, this, tskIDLE_PRIORITY + 2, nullptr, 0);
      continue;
    }
    lineBuffer = "";
    if (!isRecCommand) {
      continue;
    }
    if (micRecorder_.recording()) {
      Serial.println("[echoform] REC ignored: already recording");
    } else if (!sdReady_) {
      Serial.println("[echoform] REC ignored: SD not mounted");
    } else {
      micRecorder_.start(audio_, kRecWavPath, kMicRecordSeconds);
    }
  }
}
