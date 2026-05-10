#pragma once

#include <Arduino.h>

// Multi-layer 2D parallax starfield. Different from the cube screensaver
// (which is a 3D rotating point cloud); these stars stream sideways at
// per-layer speeds for a Star-Wars/Hyperspace aesthetic.
class Starfield {
 public:
  static constexpr int kLayerCount = 3;
  static constexpr int kStarsPerLayer = 32;
  static constexpr int kStarCount = kLayerCount * kStarsPerLayer;

  struct Star {
    // Position in logical space (0..639 X, 0..171 Y). Stars wrap at the
    // left edge.
    float x;
    int16_t y;
    uint8_t layer;       // 0 = farthest/dimmest, kLayerCount-1 = closest/brightest
    uint8_t brightness;  // 0..255, derived from layer + a tiny per-star jitter
  };

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  const Star *stars() const { return stars_; }
  size_t starCount() const { return kStarCount; }

 private:
  uint32_t nextRandU32();
  Star stars_[kStarCount];
  uint32_t prng_ = 0x12345678u;
  uint32_t lastMs_ = 0;
};
