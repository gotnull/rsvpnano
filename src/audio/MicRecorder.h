#pragma once

#include <Arduino.h>

#include <atomic>

class AudioManager;

// Echoform M0 bring-up harness (docs/ECHOFORM.md): records N seconds from the
// ES7210 mic array to a mono 16 kHz PCM16 WAV on the SD card, on its own
// FreeRTOS task so the main loop never blocks. Temporary instrumentation —
// the real Echoform mic task replaces this once the hardware path is proven.
class MicRecorder {
 public:
  // Spawns the recorder task. Returns false if a recording is already in
  // flight or the task could not be created. The caller gates on ModPlayer
  // being idle (both would own I2S0 otherwise).
  bool start(AudioManager &audio, const String &path, uint32_t seconds);

  bool recording() const { return recording_.load(); }
  // One-shot: true exactly once after a recording finishes.
  bool takeFinished() { return finishedOneShot_.exchange(false); }
  bool lastSucceeded() const { return success_.load(); }

 private:
  static void taskTrampoline(void *arg);
  void taskRun();

  AudioManager *audio_ = nullptr;
  String path_;
  uint32_t seconds_ = 0;

  std::atomic<bool> recording_{false};
  std::atomic<bool> finishedOneShot_{false};
  std::atomic<bool> success_{false};
};
