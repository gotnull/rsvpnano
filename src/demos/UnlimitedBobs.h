#pragma once

#include <Arduino.h>

// Equinox "Unlimited Bobs" effect (Keops) — 1:1 port of the eqx=1 mode
// from `UnlimitedBobs.java`. The applet stamps a single 15×15 sprite
// (sprites2.gif) at the current position of a Lissajous-style curve.
// The framebuffer is NEVER cleared — accumulated stamps build the
// characteristic "wavy mesh" purple/pink trail across the screen.
//
// Six curves are selectable (Java's `a_`); cycleCurve() advances the
// index and clears the framebuffer to give a clean trail.
class UnlimitedBobs {
 public:
  static constexpr int kFrameWidth = 320;       // Java's a3
  static constexpr int kFrameHeight = 200;      // Java's a2
  static constexpr int kSpriteSize = 15;        // sprites2.gif is 15×15
  static constexpr int kCurveCount = 6;         // Java's `a_` cycles 0..5
  // 8 inner steps per visual tick — mirrors Java's 8-buffer
  // accumulate / cycle behaviour with a single buffer.
  static constexpr int kStepsPerFrame = 8;
  static constexpr size_t kFramebufferPixels =
      static_cast<size_t>(kFrameWidth) * kFrameHeight;
  static constexpr size_t kFramebufferBytes =
      kFramebufferPixels * sizeof(uint16_t);

  ~UnlimitedBobs();

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  // Cycle curve 0→1→…→5→0 + clear framebuffer (matches Java mouseDown).
  void cycleCurve();

  // RGB565 framebuffer, 320×200, accumulating sprite stamps.
  const uint16_t *framebuffer() const { return framebuffer_; }

 private:
  void runCurveStep();
  void stampSprite(int cx, int cy);

  uint16_t *framebuffer_ = nullptr;

  // Curve phase accumulators (Java's a.{au, at, as, ar}).
  float au_ = 0.0f, at_ = 0.0f, as_ = 0.0f, ar_ = 0.0f;
  // Oscillating amplitudes (Java's a.{s, r, q, p}).
  float s_ = 5.0f, r_ = 200.0f, q_ = 0.0125f, p_ = 0.15f;
  // Current sprite stamp position (Java's a.{aw, av}).
  int16_t stampX_ = kFrameWidth / 2 - kSpriteSize / 2;
  int16_t stampY_ = kFrameHeight / 2 - kSpriteSize / 2;

  uint8_t curveIndex_ = 0;
};
