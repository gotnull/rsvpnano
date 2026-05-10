#pragma once

#include <Arduino.h>

// Classic Amiga-style rasterbars: animated horizontal color bars over a copper
// gradient background. State only — rendering happens in
// DisplayManager::renderRasterbarsFrame using the native-stripe path.
class Rasterbars {
 public:
  // Number of independent bars; each has its own phase + color + thickness.
  // Tuned for the 172-tall logical strip — more than ~6 bars and they stack
  // on top of each other and the effect looks muddy.
  static constexpr int kBarCount = 6;

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  // Per-bar Y center (logical pixel space, 0..171) for the current frame.
  int barCenterY(int i) const { return centerY_[i]; }
  int barHalfThickness(int i) const { return halfThickness_[i]; }
  // RGB565 (panel byte order applied at draw time).
  uint16_t barColor(int i) const { return colors_[i]; }
  // Copper gradient is sampled per-row in the renderer; this returns the
  // animated phase that scrolls it.
  float copperPhase() const { return copperPhase_; }

 private:
  int16_t centerY_[kBarCount] = {0};
  int8_t halfThickness_[kBarCount] = {0};
  uint16_t colors_[kBarCount] = {0};
  float phase_[kBarCount] = {0};
  float speed_[kBarCount] = {0};
  float copperPhase_ = 0.0f;
  uint32_t lastMs_ = 0;
};
