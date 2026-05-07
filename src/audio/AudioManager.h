#pragma once

#include <Arduino.h>

class AudioManager {
 public:
  bool begin();
  // maxDurationMs > 0 truncates playback for quick previews. 0 = play in full.
  bool playRtttl(const String &melody, uint32_t maxDurationMs = 0);
  // Plays a PCM WAV file straight from the SD card. Supports 8-bit unsigned
  // and 16-bit signed mono/stereo at any sample rate the I2S peripheral can
  // produce. Returns false on header parse error or if the codec isn't ready.
  bool playWavFromSd(const String &path, uint32_t maxDurationMs = 0);
  // Volume 0..100 (linear). Persists across calls; only affects subsequent playback.
  void setVolumePercent(uint8_t percent);
  uint8_t volumePercent() const { return volumePercent_; }
  // Short fixed sine — used for volume cycling so the main loop is only blocked
  // for ~80 ms per tap instead of the full RTTTL/WAV duration.
  void playUiClick();

 private:
  bool writeCodecRegister(uint8_t reg, uint8_t value);
  bool initCodec();
  bool initI2sChannel();
  void playToneSamples(uint16_t frequencyHz, uint16_t durationMs);
  void playSilence(uint16_t durationMs);

  bool initialized_ = false;
  uint8_t volumePercent_ = 60;
};
