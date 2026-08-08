#include "echoform/EchoFeatures.h"

#include <math.h>

namespace echoform {

namespace {

float clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }

void stepEnv(float &env, float input, float attack, float release) {
  const float rate = input > env ? attack : release;
  env += (input - env) * rate;
}

}  // namespace

AudioFeatures FeatureExtractor::process(const int16_t *samples, size_t count) {
  if (samples == nullptr || count == 0) {
    return last_;
  }
  uint64_t sumSq = 0;
  int32_t peak = 0;
  float sumLow = 0.0f;
  float sumMid = 0.0f;
  float sumHigh = 0.0f;
  for (size_t i = 0; i < count; ++i) {
    const int32_t x = samples[i];
    const int32_t ax = x < 0 ? -x : x;
    sumSq += static_cast<uint64_t>(static_cast<int64_t>(x) * x);
    if (ax > peak) peak = ax;
    const float xf = static_cast<float>(x);
    lp300_ += kALp300 * (xf - lp300_);
    lp2k_ += kALp2k * (xf - lp2k_);
    sumLow += fabsf(lp300_);
    sumMid += fabsf(lp2k_ - lp300_);
    sumHigh += fabsf(xf - lp2k_);
  }
  const float n = static_cast<float>(count);
  const float rms = sqrtf(static_cast<float>(sumSq) / n) / 32768.0f;
  const float peakF = static_cast<float>(peak) / 32768.0f;
  const float low = (sumLow / n) / 32768.0f;
  const float mid = (sumMid / n) / 32768.0f;
  const float high = (sumHigh / n) / 32768.0f;

  stepEnv(fastEnv_, rms, kFastAttack, kFastRelease);
  stepEnv(slowEnv_, rms, kSlowAttack, kSlowRelease);
  const float transient =
      clamp01(clamp01(fastEnv_ - slowEnv_ - slowEnv_ * 0.25f) * 3.0f);

  // Adaptive noise floor: falls fast on quiet, rises very slowly.
  noiseFloor_ += (rms - noiseFloor_) * (rms < noiseFloor_ ? kFloorFall
                                                          : kFloorRise);
  if (noiseFloor_ < 0.0005f) noiseFloor_ = 0.0005f;
  if (noiseFloor_ > 1.0f) noiseFloor_ = 1.0f;

  // Voice probability: level margin over the floor, weighted toward the
  // speech band, then time hysteresis.
  const float margin = rms - noiseFloor_ * 3.0f;
  const float denom = noiseFloor_ * 6.0f + 0.01f;
  const float levelP = clamp01(margin / denom);
  const float bandP = rms > 0.0f ? clamp01(mid / rms) : 0.0f;
  const float p = levelP * 0.7f + bandP * levelP * 0.3f;
  voiceProb_ += (p - voiceProb_) * 0.5f;

  if (voiceProb_ > 0.55f) {
    openRun_ = static_cast<uint8_t>(openRun_ + 1);
    closeRun_ = 0;
    if (openRun_ >= kVoiceOpenBlocks) voiceActive_ = true;
  } else if (voiceProb_ < 0.30f) {
    closeRun_ = static_cast<uint8_t>(closeRun_ + 1);
    openRun_ = 0;
    if (closeRun_ >= kVoiceCloseBlocks) voiceActive_ = false;
  } else {
    openRun_ = 0;
    closeRun_ = 0;
  }

  last_.rms = rms;
  last_.peak = peakF;
  last_.transient = transient;
  last_.low = low;
  last_.mid = mid;
  last_.high = high;
  last_.noiseFloor = noiseFloor_;
  last_.voiceProbability = voiceProb_;
  return last_;
}

}  // namespace echoform
