#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

// Streaming MOD/XM/S3M/IT player powered by libxmp-lite. Runs a dedicated
// FreeRTOS task pinned to core 0 that renders the next chunk of audio with
// xmp_play_buffer() and writes it to the existing I2S channel set up by
// AudioManager.
//
// The player coexists with AudioManager's blocking RTTTL/WAV calls in a
// best-effort way — both end up writing to I2S_NUM_0, so simultaneous use
// will produce garbled audio. AudioManager::playRtttl()/playWavFromSd()
// callers are expected to call ModPlayer::pause() first if they want
// uninterrupted notification tones (the App-level coordination lives in
// Stage 3 of the rollout).
//
// Volume model: setVolumePercent() is the user's chosen master level
// (0..100, persistent). setDuckPercent() is a transient multiplier (0..100,
// resets to 100 on resume) used by the notification-tone duck path.
class ModPlayer {
 public:
  ModPlayer() = default;
  ~ModPlayer();

  // Spawns the audio task; safe to call repeatedly. Requires AudioManager
  // to have already initialised the I2S TX channel and ES8311 codec.
  bool begin();

  // Loads a module from SD and starts streaming playback. Returns false on
  // file-open / format errors. If a track is already playing, it's stopped
  // and replaced.
  bool playFile(const String &path);

  // Signals the audio task to stop and release the loaded module.
  // Returns immediately — cleanup (xmp_end_player / xmp_release_module /
  // xmp_free_context / heap_caps_free of the file buffer / i2s clock
  // restore) happens on the audio task. Callers that need the cleanup
  // synchronized (e.g. playFile() before installing a new context) call
  // waitForStop() afterwards.
  void stop();
  void waitForStop(uint32_t timeoutMs = 250);

  bool isPlaying() const { return running_; }

  // 0..100. Persistent across calls. Multiplied with duck % at render time.
  void setVolumePercent(uint8_t pct);
  uint8_t volumePercent() const { return volume_; }

  // Transient duck multiplier (0..100). Apply via setDuck(30) before a
  // notification tone, then setDuck(100) to restore.
  void setDuckPercent(uint8_t pct);

  // Basename of currently-playing file, or empty string if idle. Used by
  // the now-playing UI in Stage 2.
  const String &currentTrack() const { return currentTrack_; }

  // libxmp-derived now-playing telemetry. Safe to call from the UI tick at
  // ~30 fps; reads xmp_get_frame_info() under the player lock so the
  // audio task can't pull the context out from under it.
  struct NowPlaying {
    bool valid = false;
    char title[48] = {0};
    char format[16] = {0};
    int pos = 0;          // song position
    int row = 0;          // pattern row
    int numRows = 0;
    int bpm = 0;
    int speed = 0;
    int timeMs = 0;
    int totalMs = 0;
    int virtChannels = 0;
    int virtUsed = 0;
    // Per-channel volume snapshot, 0..64. channelCount tells the renderer
    // how many entries to read.
    uint8_t channelVolumes[32] = {0};
    uint8_t channelCount = 0;
  };
  void getNowPlaying(NowPlaying &out);

 private:
  static void audioTaskTrampoline(void *arg);
  void audioTaskLoop();

  bool initialised_ = false;
  TaskHandle_t taskHandle_ = nullptr;
  SemaphoreHandle_t lock_ = nullptr;

  // libxmp's xmp_context is `void *`. Storing as void* here so we don't
  // need to leak the include into App.h consumers.
  void *xmpCtx_ = nullptr;
  uint8_t *fileBuf_ = nullptr;
  size_t fileBufBytes_ = 0;

  volatile bool running_ = false;
  volatile bool stopRequested_ = false;
  uint8_t volume_ = 80;   // 0..100
  uint8_t duck_ = 100;    // 0..100 transient
  String currentTrack_;
};
