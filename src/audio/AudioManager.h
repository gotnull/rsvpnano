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

  // --- Echoform mic capture (docs/ECHOFORM.md) ---
  // Reinstalls I2S0 full duplex at 16 kHz mono and brings up the ES7210 mic
  // ADC. Mutually exclusive with ModPlayer (which reclocks the same port to
  // 44.1 kHz stereo); the caller gates on that. Playback via i2s_write keeps
  // working while capture is active — both directions share the 16 kHz clock.
  bool beginMicCapture();
  // Blocking read of up to maxSamples mono PCM16 samples. Returns the number
  // of samples actually read. Call from a worker task, never the main loop.
  size_t readMicSamples(int16_t *dst, size_t maxSamples, uint32_t timeoutMs);
  // Bench diagnostic: raw 32-bit I2S slot words, no folding. Same contract.
  size_t readMicRaw(int32_t *dst, size_t maxWords, uint32_t timeoutMs);
  // Stops the ES7210 and restores the TX-only I2S driver.
  void endMicCapture();
  bool micCaptureActive() const { return micCaptureActive_; }
  // Blocking write of mono PCM16 through the TX path at the configured
  // volume (Echoform TTS playback). Call from a worker task, never the main
  // loop; requires the TX-only driver (not capture mode).
  void writeSpeech(const int16_t *samples, size_t count);

 private:
  bool writeCodecRegister(uint8_t reg, uint8_t value);
  bool initCodec();
  bool initI2sChannel();
  bool installI2sDriver(bool duplex);
  void playToneSamples(uint16_t frequencyHz, uint16_t durationMs);
  void playSilence(uint16_t durationMs);

  bool initialized_ = false;
  bool micCaptureActive_ = false;
  uint8_t volumePercent_ = 60;
};
