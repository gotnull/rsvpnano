#include "demos/SineScroller.h"

void SineScroller::begin(uint32_t seedMs, const char *message) {
  (void)seedMs;
  if (message != nullptr && message[0] != '\0') message_ = message;
  scrollPx_ = 0.0f;
  wavePhase_ = 0.0f;
  lastMs_ = 0;
}

void SineScroller::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 60.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;

  // ~120 px / sec — roughly one glyph-width per ~80 ms at our font size,
  // readable but not sluggish.
  constexpr float kScrollSpeedPx = 120.0f;
  scrollPx_ += kScrollSpeedPx * dt;

  // Wave phase: ~1 Hz sine; renderer applies it per glyph offset.
  constexpr float kWaveHz = 1.2f;
  wavePhase_ += dt * 2.0f * 3.14159265f * kWaveHz;
}
