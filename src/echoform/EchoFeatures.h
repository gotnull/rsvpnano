#pragma once

#include <stddef.h>
#include <stdint.h>

// Block audio feature extractor - C++ port of rusty-nail's
// crates/fcecho/src/features.rs. Runs once per 16 ms block (256 samples at
// 16 kHz). One-pole IIR band split (no FFT), asymmetric noise floor,
// voice-activity gate with level + time hysteresis (3 blocks to open, 8 to
// close). Float where the original used Q16.16; the constants are the
// original's values.
namespace echoform {

constexpr size_t kBlockSamples = 256;

struct AudioFeatures {
  float rms = 0.0f;
  float peak = 0.0f;
  float transient = 0.0f;
  float low = 0.0f;
  float mid = 0.0f;
  float high = 0.0f;
  float noiseFloor = 0.0f;
  float voiceProbability = 0.0f;
};

class FeatureExtractor {
 public:
  // Process one block of mono PCM16; updates voiceActive().
  AudioFeatures process(const int16_t *samples, size_t count);
  bool voiceActive() const { return voiceActive_; }
  const AudioFeatures &last() const { return last_; }

 private:
  // One-pole coefficients for 16 kHz: 1 - e^(-2*pi*fc/fs).
  static constexpr float kALp300 = 3644.0f / 32768.0f;
  static constexpr float kALp2k = 17829.0f / 32768.0f;
  static constexpr float kFastAttack = 0.55f;
  static constexpr float kFastRelease = 0.20f;
  static constexpr float kSlowAttack = 0.08f;
  static constexpr float kSlowRelease = 0.04f;
  static constexpr float kFloorFall = 0.08f;
  static constexpr float kFloorRise = 0.004f;
  static constexpr uint8_t kVoiceOpenBlocks = 3;
  static constexpr uint8_t kVoiceCloseBlocks = 8;

  float lp300_ = 0.0f;
  float lp2k_ = 0.0f;
  float fastEnv_ = 0.0f;
  float slowEnv_ = 0.0f;
  // Start mid-range so the first blocks cannot instantly count as voice.
  float noiseFloor_ = 0.05f;
  float voiceProb_ = 0.0f;
  bool voiceActive_ = false;
  uint8_t openRun_ = 0;
  uint8_t closeRun_ = 0;
  AudioFeatures last_;
};

}  // namespace echoform
