#pragma once

#include <Arduino.h>

#include <atomic>

#include "echoform/EchoFeatures.h"

class AudioManager;

// Echoform's audio pipe (docs/ECHOFORM.md M2): the two rings between the
// hardware and the net task, each fed/drained by its own worker so neither
// the render loop nor the net task ever blocks on I2S.
//
//   mic task (core 0):  duplex I2S -> mono PCM16 (TDM slot 0) -> TX ring
//   speech task (core 0): speech ring -> i2s_write (TX-only driver)
//
// Half duplex by construction: capture owns the I2S port in duplex mode;
// speech playback requires the TX-only driver, restored at captureTeardown.
class EchoformAudio {
 public:
  // TX ring >= 1 s so the M4 preroll can come straight from here later.
  static constexpr size_t kTxRingSamples = 16384;
  // Speech ring >= 600 ms (plan: relay bursts 250 ms then paces 40 ms
  // frames; RN shipped overflow crackle until ring > burst + headroom).
  static constexpr size_t kSpeechRingSamples = 16384;
  // Prime playback at ~180 ms so the first TTS burst never underruns.
  static constexpr size_t kSpeechPrimeSamples = 2880;

  void begin(AudioManager *audio);

  // -- capture side (called from the app / net task) --
  // Reconfigures I2S duplex and starts the mic worker filling the TX ring.
  bool startCapture();
  // Stops NEW samples entering the ring; the ring keeps its tail for the
  // net task to flush.
  void stopCapture() { capturing_.store(false); }
  // Restores the TX-only I2S driver once the utterance is fully flushed.
  // Safe to call from the net task.
  void captureTeardown();
  bool capturing() const { return capturing_.load(); }
  // Drain up to maxSamples from the TX ring; returns samples written.
  size_t txDrain(int16_t *out, size_t maxSamples);

  // -- speech side (called from the net task) --
  // Push inbound SpeechPcm16; drops if the ring is full (counted).
  void pushSpeech(const int16_t *samples, size_t count);
  // The relay finished this reply's stream (Completed): let the player
  // drain to empty and go idle.
  void speechStreamEnded() { speechEnded_.store(true); }
  // True while the speaker is playing or has queued audio - the wake/PTT
  // gate per the plan ("gate on session state, not speaker-silent").
  bool speechBusy() const {
    return speechLen_.load() > 0 || speechPlaying_.load();
  }
  uint32_t speechDropped() const { return speechDropped_.load(); }

  // -- live scope (render loop) --
  // The most recent audio window from whichever stream is live: the mic
  // while capturing, the speaker feed while speaking. Tearing is tolerable
  // (visual only, single reader). Returns true if fed within the last
  // 150 ms.
  static constexpr size_t kScopePoints = 216;
  bool scopeSnapshot(int16_t *out) const;

  // Latest 16 ms block features from the live stream (same freshness rule).
  struct LiveFeatures {
    float rms;
    float transient;
    float low;
    float mid;
    float high;
    bool voice;
  };
  bool featuresSnapshot(LiveFeatures &out) const;

 private:
  static void micTaskTrampoline(void *arg);
  void micTaskRun();
  static void speechTaskTrampoline(void *arg);
  void speechTaskRun();

  AudioManager *audio_ = nullptr;

  // TX ring: single producer (mic task), single consumer (net task).
  int16_t txRing_[kTxRingSamples];
  std::atomic<size_t> txHead_{0};  // producer writes
  std::atomic<size_t> txTail_{0};  // consumer reads
  std::atomic<bool> capturing_{false};
  std::atomic<bool> micTaskAlive_{false};

  // Speech ring: single producer (net task), single consumer (speech task).
  int16_t speechRing_[kSpeechRingSamples];
  std::atomic<size_t> speechHead_{0};
  std::atomic<size_t> speechTail_{0};
  std::atomic<size_t> speechLen_{0};
  std::atomic<bool> speechEnded_{false};
  std::atomic<uint32_t> speechPushMs_{0};
  std::atomic<bool> speechPlaying_{false};
  std::atomic<uint32_t> speechDropped_{0};

  // Scope: scrolled by the live worker, read (lock-free, tear-tolerant) by
  // the render side.
  void scopeFeed(const int16_t *samples, size_t count, size_t stride);
  int16_t scope_[kScopePoints] = {0};
  std::atomic<uint32_t> scopeStampMs_{0};

  // Feature extractors: one per stream, each owned by its worker task; the
  // latest block's numbers published through atomics for the render side.
  void publishFeatures(const echoform::AudioFeatures &f, bool voice);
  echoform::FeatureExtractor micExtractor_;
  echoform::FeatureExtractor speechExtractor_;
  std::atomic<float> featRms_{0.0f};
  std::atomic<float> featTransient_{0.0f};
  std::atomic<float> featLow_{0.0f};
  std::atomic<float> featMid_{0.0f};
  std::atomic<float> featHigh_{0.0f};
  std::atomic<bool> featVoice_{false};
  std::atomic<uint32_t> featStampMs_{0};
};
