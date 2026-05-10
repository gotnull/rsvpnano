#include "demos/Rasterbars.h"

#include <math.h>

namespace {

// A small palette per bar — saturated demoscene-flavor primaries.
// RGB565 (raw, byte-swap is applied later via panelColor()).
constexpr uint16_t kBarPalette[Rasterbars::kBarCount] = {
    0xF800,  // red
    0xFD60,  // orange
    0xFFE0,  // yellow
    0x07E0,  // green
    0x07FF,  // cyan
    0xF81F,  // magenta
};

}  // namespace

void Rasterbars::begin(uint32_t seedMs) {
  for (int i = 0; i < kBarCount; ++i) {
    // Stagger phases so the bars don't all peak together at frame 0.
    phase_[i] = (static_cast<float>(i) / kBarCount) * 2.0f * static_cast<float>(M_PI);
    // Speeds in radians per second — small spread so the bars drift in/out
    // of phase over time. Seed perturbs slightly per session.
    const float jitter = static_cast<float>((seedMs ^ (i * 0x9E3779B1u)) & 0xFF) / 255.0f;
    speed_[i] = 0.6f + 0.25f * i + 0.15f * jitter;
    halfThickness_[i] = 6 + (i & 1) * 2;  // alternating 6 / 8 px half thickness
    colors_[i] = kBarPalette[i];
    centerY_[i] = 86;  // initialised properly on first tick()
  }
  copperPhase_ = 0.0f;
  lastMs_ = 0;
}

void Rasterbars::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 60.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;

  // Bars sweep up/down through the 172 px strip with a margin so they don't
  // clip at the edges.
  constexpr int kHeight = 172;
  constexpr int kMargin = 12;
  const int amplitude = (kHeight / 2) - kMargin;
  for (int i = 0; i < kBarCount; ++i) {
    phase_[i] += speed_[i] * dt;
    centerY_[i] = static_cast<int16_t>((kHeight / 2) + amplitude * sinf(phase_[i]));
  }

  copperPhase_ += dt * 0.7f;  // slow background drift
}
