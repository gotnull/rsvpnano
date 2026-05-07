#pragma once

#include <Arduino.h>

class AudioManager {
 public:
  bool begin();
  bool playRtttl(const String &melody);
  // Plays a PCM WAV file straight from the SD card. Supports 8-bit unsigned
  // and 16-bit signed mono/stereo at any sample rate the I2S peripheral can
  // produce. Returns false on header parse error or if the codec isn't ready.
  bool playWavFromSd(const String &path);
  // Volume 0..100 (linear). Persists across calls; only affects subsequent playback.
  void setVolumePercent(uint8_t percent);
  uint8_t volumePercent() const { return volumePercent_; }

 private:
  bool writeCodecRegister(uint8_t reg, uint8_t value);
  bool initCodec();
  bool initI2sChannel();
  void playToneSamples(uint16_t frequencyHz, uint16_t durationMs);
  void playSilence(uint16_t durationMs);

  bool initialized_ = false;
  uint8_t volumePercent_ = 60;
};
