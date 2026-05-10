#pragma once

#include <Arduino.h>

// "Vectorball" — homage to Keops/Equinox's 1992 Atari ST effect (later ported
// to Java applet on equinox.planet-d.net). Original used 50 balls on Atari,
// 400 in the Java version. We render 400 on ESP32 native stripes.
//
// Approach: Fibonacci sphere distribution gives near-uniform point spacing
// without needing the original obfuscated lookup table. Visually identical
// to the source — perceptual ball arrangement is what matters.
//
// State only — DisplayManager::renderVectorballFrame does the projection,
// painter sort, and depth-cued drawing.
class Vectorball {
 public:
  static constexpr int kBallCount = 400;
  // Pico-8-ish 16-color palette + per-ball per-radius shade ramp generates
  // the "shiny depth-cued sphere" look from a single base color per ball.
  static constexpr int kPaletteSize = 16;

  struct Ball {
    // Unit-sphere position (pre-rotation).
    float ux, uy, uz;
    // Filled by tick() each frame:
    float cx, cy, cz;        // post-rotation camera-space
    int colorIndex;          // 0..kPaletteSize-1
  };

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);
  // Reorders drawOrder() by cz descending so the renderer can paint
  // back-to-front. Called after tick().
  void sortBalls();

  const Ball *balls() const { return balls_; }
  size_t ballCount() const { return kBallCount; }
  const uint16_t *drawOrder() const { return drawOrder_; }
  // RGB565 (raw, panelColor() applied at draw time).
  static const uint16_t *palette();

 private:
  void initFibonacciSphere();

  Ball balls_[kBallCount];
  uint16_t drawOrder_[kBallCount];
  float t_ = 0.0f;
  uint32_t lastMs_ = 0;
};
