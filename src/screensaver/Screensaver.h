#pragma once

#include <Arduino.h>

// 3D dots-and-stars screensaver ported from
// https://github.com/gotnull/dots — adapted to our scaled framebuffer.
// State lives here; rendering happens in DisplayManager::renderScreensaverFrame.
class Screensaver {
 public:
  static constexpr int kPointsPerAxis = 6;
  static constexpr int kPointCount = kPointsPerAxis * kPointsPerAxis * kPointsPerAxis;
  static constexpr int kStarCount = 80;
  static constexpr int kPaletteSize = 15;

  struct Point {
    // Cube-space and sphere-space anchors (for morph), plus mutable runtime
    // position used during animation.
    float cubeX, cubeY, cubeZ;
    float sphereX, sphereY, sphereZ;
    float x, y, z;
    // After projection (filled by computeFrame), screen-space position +
    // depth used for back-to-front sort.
    float cx, cy, cz;
    int colorIndex;
  };
  struct Star {
    float x, y, z;
  };

  void begin(uint32_t seed = 0);
  // Advances one animation frame; call once per draw at ~30 fps.
  void tick();
  // Reorders point[] back-to-front along cz (largest first) so painters'
  // algorithm in the renderer produces correct depth.
  void sortPoints();

  const Point *points() const { return points_; }
  size_t pointCount() const { return kPointCount; }
  const Star *stars() const { return stars_; }
  size_t starCount() const { return kStarCount; }
  // RGB565 Pico-8 palette (no black). Index by colorIndex % 15.
  static const uint16_t *palette();

 private:
  void initPoints();
  void initStars();
  void updateStars();
  void updateMorph();

  Point points_[kPointCount];
  Star stars_[kStarCount];
  float t_ = 0.0f;
  float t_mod_ = 0.0f;
  float nextChange_ = 0.0f;
  float currentSpeed_ = 1.0f;
  float morphProgress_ = 0.0f;
  bool morphing_ = false;
};
