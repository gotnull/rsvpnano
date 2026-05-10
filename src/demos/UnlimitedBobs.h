#pragma once

#include <Arduino.h>

// 1:1 port of the Equinox "Unlimited Bobs" effect by Keops (Java applet on
// equinox.planet-d.net). Renders a back-face-culled, flat-shaded 3D cube
// that translates along one of 6 Lissajous-style curves while rotating;
// the framebuffer is never cleared, so each frame stamps a fresh cube on
// top of the previous ones — the cloud of accumulated cubes is the effect.
//
// Design notes (ported from `a.java` / `UnlimitedBobs.java`):
//   - 8 cube corners at ±100 model-space units
//   - 12 triangular faces (two per cube face) from the static a[12][3] table
//   - Tait-Bryan ZYX rotation (matches Vectorball)
//   - Perspective projection: x' = 300 * x / (300 - (z + u))  with u = -3000
//   - Each face shaded by z-component of its surface normal:
//       z_intensity = sqrt(sqrt(normalZ)) * 255   (in 0..255)
//       color       = RGB(50, 50, z_intensity)
//   - Original spawned 8 framebuffers and called g() 8× per visual frame
//     to smear cubes along the curve. We collapse to a single framebuffer
//     and call the inner step 8× per tick — visually equivalent.
//
// Curve cycling: pressing nothing, the demo holds curve 0. App.cpp wires
// a touch hook so a tap cycles 0→1→…→5→0, matching the Java mouseDown
// behaviour.
class UnlimitedBobs {
 public:
  static constexpr int kFrameWidth = 320;       // a3
  static constexpr int kFrameHeight = 200;      // a2
  static constexpr int kSpriteSize = 15;        // a5/a4 — the original sprite was a 15×15 GIF
  static constexpr int kCubeRadius = 100;       // v
  static constexpr int kFaceCount = 12;
  static constexpr int kCurveCount = 6;
  static constexpr int kStepsPerFrame = 8;      // 8 g() calls per visual frame
  static constexpr float kCameraZ = -3000.0f;   // u
  static constexpr float kFocal = 300.0f;
  static constexpr size_t kFramebufferBytes =
      static_cast<size_t>(kFrameWidth) * kFrameHeight;  // 64 000 bytes

  ~UnlimitedBobs();

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  // Advance to the next curve (0→1→…→5→0) and clear the framebuffer.
  // Wired to the touch hook in App so a tap matches the original mouseDown.
  void cycleCurve();

  const uint8_t *framebuffer() const { return framebuffer_; }

 private:
  // Per-tick step (called kStepsPerFrame times by tick()). Updates the
  // rotation accumulators, advances curve phase, projects vertices,
  // then back-face-culls + flat-shades + fills each of the 12 faces.
  void stepOnce();
  void buildRotationMatrix();
  void projectVertices();
  void runCurve();
  // Triangle scan-line fill into the framebuffer. Clipped to the screen.
  void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t shade);

  uint8_t *framebuffer_ = nullptr;

  // Rotation accumulators (matches a.{aq, ap, ao}).
  float angleX_ = 0.0f, angleY_ = 0.0f, angleZ_ = 0.0f;
  // 9 rotation matrix coefficients (matches a.{o, n, m, l, k, j, i, g, f}).
  float r_o_ = 0, r_n_ = 0, r_m_ = 0;
  float r_l_ = 0, r_k_ = 0, r_j_ = 0;
  float r_i_ = 0, r_g_ = 0, r_f_ = 0;

  // Curve phase accumulators (matches a.{au, at, as, ar}).
  float au_ = 0.0f, at_ = 0.0f, as_ = 0.0f, ar_ = 0.0f;
  // Oscillating amplitudes (matches a.{s, r, q, p}). Bound between
  // [3, 175] / [3, 250] with deltas q / p that flip sign at the bounds.
  float s_ = 5.0f, r_ = 200.0f, q_ = 0.0125f, p_ = 0.15f;

  // Cube center on screen for the current step.
  int16_t centerX_ = kFrameWidth / 2 - kSpriteSize / 2;
  int16_t centerY_ = kFrameHeight / 2 - kSpriteSize / 2;

  // Projected vertices for the current step.
  int16_t projX_[8] = {0};
  int16_t projY_[8] = {0};
  int16_t projZ_[8] = {0};

  uint8_t curveIndex_ = 0;  // 0..5 (matches `a_` from UnlimitedBobs.java)
};
