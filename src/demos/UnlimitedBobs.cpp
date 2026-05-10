#include "demos/UnlimitedBobs.h"

#include <algorithm>
#include <cstring>
#include <esp_heap_caps.h>
#include <math.h>

namespace {

// 8 cube corners at ±100 in X/Y/Z, indexed 0..7. Order matches a.d() in the
// original — the face table below references vertices by these indices.
constexpr int16_t kCubeVerts[8][3] = {
    { 100, -100,  100},  // 0
    { 100, -100, -100},  // 1
    {-100, -100, -100},  // 2
    {-100, -100,  100},  // 3
    { 100,  100,  100},  // 4
    { 100,  100, -100},  // 5
    {-100,  100, -100},  // 6
    {-100,  100,  100},  // 7
};

// 12 triangular faces (2 per cube side). Lifted verbatim from the static
// `a` table in a.java's class init. Vertex indices reference kCubeVerts.
constexpr uint8_t kCubeFaces[12][3] = {
    {0, 1, 5},   //  0  — top-back / right
    {0, 5, 4},   //  1
    {1, 2, 6},   //  2
    {1, 6, 5},   //  3
    {2, 3, 7},   //  4
    {2, 7, 6},   //  5
    {3, 0, 4},   //  6
    {3, 4, 7},   //  7
    {7, 4, 5},   //  8
    {7, 5, 6},   //  9
    {2, 1, 0},   // 10
    {2, 0, 3},   // 11
};

// Per-call rotation deltas (a.{aq, ap, ao} += these each g() call).
constexpr float kDeltaRotX = 0.04f;
constexpr float kDeltaRotY = 0.06f;
constexpr float kDeltaRotZ = 0.03f;

// Tightly clamp the lit shade so the cubes never go full-black against the
// black framebuffer (would disappear) or beyond palette range.
inline uint8_t clampShade(int v) {
  if (v < 16) return 16;
  if (v > 255) return 255;
  return static_cast<uint8_t>(v);
}

}  // namespace

UnlimitedBobs::~UnlimitedBobs() {
  if (framebuffer_ != nullptr) {
    heap_caps_free(framebuffer_);
    framebuffer_ = nullptr;
  }
}

void UnlimitedBobs::begin(uint32_t seedMs) {
  (void)seedMs;
  if (framebuffer_ == nullptr) {
    framebuffer_ = static_cast<uint8_t *>(
        heap_caps_malloc(kFramebufferBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (framebuffer_ == nullptr) {
      framebuffer_ = static_cast<uint8_t *>(
          heap_caps_malloc(kFramebufferBytes, MALLOC_CAP_8BIT));
    }
  }
  if (framebuffer_ != nullptr) {
    std::memset(framebuffer_, 0, kFramebufferBytes);
  }
  // Reset rotation + curve state. Original lets the user mouse-click to
  // cycle through the 6 curves; our touch handler is reserved for "exit
  // demo", so we randomise the starting curve from the seed instead. Each
  // launch picks one of the 6 — relaunch to see a different one.
  angleX_ = angleY_ = angleZ_ = 0.0f;
  au_ = at_ = as_ = ar_ = 0.0f;
  s_ = 5.0f;
  r_ = 200.0f;
  q_ = 0.0125f;
  p_ = 0.15f;
  curveIndex_ = static_cast<uint8_t>((seedMs ? seedMs : 1u) % kCurveCount);
}

void UnlimitedBobs::cycleCurve() {
  curveIndex_ = (curveIndex_ + 1) % kCurveCount;
  if (framebuffer_ != nullptr) std::memset(framebuffer_, 0, kFramebufferBytes);
  // Match a.h() reset: zero phase accumulators (s/r/q/p stay so the
  // amplitude oscillation continues smoothly across curve changes).
  au_ = at_ = as_ = ar_ = 0.0f;
  angleX_ = angleY_ = angleZ_ = 0.0f;
}

void UnlimitedBobs::buildRotationMatrix() {
  // Tait-Bryan ZYX, mirrors a.e().
  const float f1 = cosf(angleX_), f2 = sinf(angleX_);
  const float f3 = cosf(angleY_), f4 = sinf(angleY_);
  const float f5 = cosf(angleZ_), f6 = sinf(angleZ_);
  r_o_ = f1 * f3;
  r_n_ = -(f1 * f4 * f6 + f2 * f5);
  r_m_ = -(f1 * f4 * f5 - f2 * f6);
  r_l_ = f2 * f3;
  r_k_ = -(f2 * f4 * f6 - f1 * f5);
  r_j_ = -(f2 * f4 * f5 + f1 * f6);
  r_i_ = f4;
  r_g_ = f3 * f6;
  r_f_ = f3 * f5;
}

void UnlimitedBobs::projectVertices() {
  // Mirrors a.f(). For each cube corner: rotate, then perspective project
  // through focal=300 with camera at z=u=-3000. Output X/Y are biased by
  // (+8, +4) per the original (sprite half-offset).
  for (int n = 0; n < 8; ++n) {
    const float vx = kCubeVerts[n][0];
    const float vy = kCubeVerts[n][1];
    const float vz = kCubeVerts[n][2];
    const float rx = r_o_ * vx + r_n_ * vy + r_m_ * vz;
    const float ry = r_l_ * vx + r_k_ * vy + r_j_ * vz;
    const float rz = r_i_ * vx + r_g_ * vy + r_f_ * vz;
    const float denom = kFocal - (rz + kCameraZ);
    const float scale = (denom != 0.0f) ? (kFocal / denom) : 1.0f;
    projX_[n] = static_cast<int16_t>(rx * scale + 8.0f);
    projY_[n] = static_cast<int16_t>(ry * scale + 4.0f);
    projZ_[n] = static_cast<int16_t>(rz);  // for face-normal Z lighting
  }
}

void UnlimitedBobs::runCurve() {
  // Per a.g()'s switch(n) — six different (cos/sin)-driven 2D paths for
  // the cube center. Only the canonical eqx=1 mode is implemented (n2=1).
  const int halfW = kFrameWidth / 2 - kSpriteSize / 2;
  const int halfH = kFrameHeight / 2 - kSpriteSize / 2;
  const float ampX = static_cast<float>(halfW - 8 - 0);  // t = 0 in eqx=1
  const float ampY = static_cast<float>(halfH - 8 - 0);
  switch (curveIndex_) {
    case 0:  // basic figure-8
      centerX_ = static_cast<int16_t>(ampX * cosf(au_) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.02f;
      centerY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.02f;
      break;
    case 1:  // 3× X frequency, opposite-direction X phase
      centerX_ = static_cast<int16_t>(ampX * cosf(au_ * 3.0f + 1.57f) *
                                      cosf(au_ / 50.0f) + halfW);
      au_ -= 0.015f;
      centerY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.01f) * cosf(au_ / 80.0f) + halfH);
      at_ -= 0.02f;
      break;
    case 2:  // additive cos+sin combo
      centerX_ = static_cast<int16_t>(
          ampX * (cosf(au_) * cosf(au_ / 80.0f) + sinf(as_ / 5.0f)) / 2.0f + halfW);
      au_ += 0.04f;
      as_ += 0.003f;
      centerY_ = static_cast<int16_t>(
          ampY * (sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + cosf(ar_ / 5.0f)) / 2.0f + halfH);
      at_ += 0.038f;
      ar_ += 0.00305f;
      break;
    case 3:  // nested cos
      centerX_ = static_cast<int16_t>(ampX * cosf(cosf(au_)) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.02f;
      centerY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.02f;
      break;
    case 4:  // sin/cos with at/ar phase modulation
      centerX_ = static_cast<int16_t>(
          ampX * (cosf(au_ * 0.99f) * sinf(au_ / 80.0f + at_) + sinf(as_ / 5.0f)) / 2.0f
          + halfW);
      au_ += 0.04f;
      as_ += 0.003f;
      centerY_ = static_cast<int16_t>(
          ampY * (sinf(au_ * 0.98f) * cosf(au_ / 60.0f) + cosf(ar_ / 5.0f)) / 2.0f
          + halfH);
      at_ += 0.038f;
      ar_ += 0.00305f;
      break;
    case 5:  // slow drift, double-cosine Y
      centerX_ = static_cast<int16_t>(ampX * cosf(au_) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.0095f;
      centerY_ = static_cast<int16_t>(
          ampY * cosf(sinf(au_ * 1.02f) * 2.0f + at_) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.01f;
      break;
  }
  // Oscillating amplitudes — q_ and p_ ping-pong between bounds. Original
  // applet uses these to subtly modulate sprite scale (we simplify to no
  // scale change here; the bounds-flip is preserved for fidelity).
  s_ += q_;
  r_ += p_;
  if (s_ < 3.0f || s_ > 175.0f) q_ = -q_;
  if (r_ < 3.0f || r_ > 250.0f) p_ = -p_;
}

void UnlimitedBobs::fillTriangle(int x0, int y0, int x1, int y1,
                                 int x2, int y2, uint8_t shade) {
  if (framebuffer_ == nullptr) return;
  // Sort vertices by Y ascending so we can scan-line top→bottom.
  if (y1 < y0) { std::swap(y0, y1); std::swap(x0, x1); }
  if (y2 < y0) { std::swap(y0, y2); std::swap(x0, x2); }
  if (y2 < y1) { std::swap(y1, y2); std::swap(x1, x2); }
  if (y0 == y2) return;  // degenerate

  // Edge 0→2 is the long side; edges 0→1 and 1→2 are the short sides.
  const float invSlope_long  = static_cast<float>(x2 - x0) / (y2 - y0);
  const float invSlope_short1 = (y1 != y0) ? static_cast<float>(x1 - x0) / (y1 - y0) : 0.0f;
  const float invSlope_short2 = (y2 != y1) ? static_cast<float>(x2 - x1) / (y2 - y1) : 0.0f;

  for (int y = std::max(0, y0); y < std::min(kFrameHeight, y2); ++y) {
    float xa = x0 + invSlope_long * (y - y0);
    float xb = (y < y1)
        ? x0 + invSlope_short1 * (y - y0)
        : x1 + invSlope_short2 * (y - y1);
    int xLo = static_cast<int>(xa < xb ? xa : xb);
    int xHi = static_cast<int>(xa < xb ? xb : xa);
    if (xLo < 0) xLo = 0;
    if (xHi >= kFrameWidth) xHi = kFrameWidth - 1;
    if (xLo > xHi) continue;
    uint8_t *row = framebuffer_ + y * kFrameWidth + xLo;
    const int n = xHi - xLo + 1;
    for (int i = 0; i < n; ++i) row[i] = shade;
  }
}

void UnlimitedBobs::stepOnce() {
  buildRotationMatrix();
  projectVertices();
  // Advance rotation accumulators per the original deltas.
  angleX_ += kDeltaRotX;
  angleY_ += kDeltaRotY;
  angleZ_ += kDeltaRotZ;
  // Move cube center along the current curve.
  runCurve();

  // Render 12 faces. Each face is back-face-culled (winding via 2D cross
  // product on the projected vertices), then flat-shaded by the Z-component
  // of the 3D face normal.
  for (int f = 0; f < kFaceCount; ++f) {
    const uint8_t a = kCubeFaces[f][0];
    const uint8_t b = kCubeFaces[f][1];
    const uint8_t c = kCubeFaces[f][2];
    const int x0 = projX_[a] + centerX_;
    const int y0 = projY_[a] + centerY_;
    const int x1 = projX_[b] + centerX_;
    const int y1 = projY_[b] + centerY_;
    const int x2 = projX_[c] + centerX_;
    const int y2 = projY_[c] + centerY_;
    // 2D cross product (b()-equivalent) for back-face cull.
    const int cross = (x1 - x0) * (y2 - y1) - (x2 - x1) * (y1 - y0);
    if (cross <= 0) continue;
    // 3D face normal Z-component for shading (a()-equivalent). Use rotated
    // Z values stored in projZ_.
    const float ex = static_cast<float>(x1 - x0);
    const float ey = static_cast<float>(y1 - y0);
    const float fx = static_cast<float>(x2 - x1);
    const float fy = static_cast<float>(y2 - y1);
    const float dz1 = static_cast<float>(projZ_[b] - projZ_[a]);
    const float dz2 = static_cast<float>(projZ_[c] - projZ_[b]);
    const float nx = ey * dz2 - dz1 * fy;
    const float ny = dz1 * fx - ex * dz2;
    const float nz = ex * fy - ey * fx;
    const float magnitude = sqrtf(nx * nx + ny * ny + nz * nz);
    if (magnitude <= 0.0f) continue;
    const float normalized = nz / magnitude;
    if (normalized <= 0.0f) continue;  // back-facing in the lighting sense
    const int z = static_cast<int>(sqrtf(sqrtf(normalized)) * 255.0f);
    fillTriangle(x0, y0, x1, y1, x2, y2, clampShade(z));
  }
}

void UnlimitedBobs::tick(uint32_t nowMs) {
  (void)nowMs;
  if (framebuffer_ == nullptr) return;
  // Original called g() once per buffer × 8 buffers per visual frame; we
  // collapse to a single buffer and call the inner step 8× — visually
  // equivalent (the curve advances, so each step lays a fresh cube).
  for (int i = 0; i < kStepsPerFrame; ++i) stepOnce();
}
