#include "screensaver/Screensaver.h"

#include <algorithm>
#include <math.h>

namespace {

constexpr float kSphereRadius = 1.5f;
constexpr float kModelScale = 1.5f;
// Camera Z offset added after rotation. Pulled from 4.0 → 3.0 so the
// cluster sits closer to the viewer — invCz is bigger, screen radius is
// bigger, and the cluster fills more of the panel.
constexpr float kCameraZ = 3.0f;
// Z-brightness depth cue range. cz=kBrightNear  → full bright (255),
// cz=kBrightFar → minimum bright (kMinBright). Tuned to roughly bracket
// the cluster's typical Z range under kCameraZ=3.
constexpr float kBrightNear = 1.5f;
constexpr float kBrightFar = 4.5f;
constexpr int kMinBright = 90;  // never let dim points go fully black

// Pico-8 palette in RGB565 (black removed — that's the background).
constexpr uint16_t kPaletteColors[Screensaver::kPaletteSize] = {
    0x18C3, 0x7C09, 0x016A, 0xA9A4, 0x52EA, 0xC618, 0xFFFB, 0xF810,
    0xFD60, 0xFFE0, 0x07E6, 0x2B7F, 0x8418, 0xFE36, 0xFF55,
};

// Multi-segment rotation DSL — frame counts per segment + per-axis radian
// deltas. Inspired by Vectorball but tuned milder for screensaver vibe.
// Total cycle: 320+240+280+200+320+260+220 = 1840 frames ≈ 31 s @ 60 fps.
constexpr uint8_t kRotSegments = 7;
constexpr uint16_t kRotFrames[kRotSegments] = {320, 240, 280, 200, 320, 260, 220};
constexpr float kRotDx[kRotSegments] = {0.012f, 0.008f, 0.018f, 0.005f, 0.010f, 0.022f, 0.006f};
constexpr float kRotDy[kRotSegments] = {0.018f, 0.022f, 0.010f, 0.030f, 0.014f, 0.006f, 0.024f};
constexpr float kRotDz[kRotSegments] = {0.004f, 0.012f, 0.020f, 0.008f, 0.006f, 0.018f, 0.014f};

// Shape-morph timeline: hold each shape for kHoldFrames, then morph to next
// over kMorphFrames. Total cycle = kShapeCount × (kHoldFrames+kMorphFrames).
// 10 × (540+180) = 7200 frames ≈ 120 s @ 60 fps before the loop repeats.
constexpr uint16_t kHoldFrames = 540;
constexpr uint16_t kMorphFrames = 180;

// Parallax starfield: 3 layers, slowest → fastest. Pixels per frame at the
// 60 fps screensaver tick.
constexpr float kParallaxLayerSpeed[3] = {0.7f, 1.6f, 3.2f};
constexpr uint8_t kParallaxLayerBright[3] = {110, 180, 255};

float clerp(float a, float b, float p) { return a + p * (b - a); }

}  // namespace

const uint16_t *Screensaver::palette() { return kPaletteColors; }

uint32_t Screensaver::nextRandU32() {
  uint32_t x = prng_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  prng_ = x;
  return x;
}

float Screensaver::nextRandFloat() {
  return static_cast<float>(nextRandU32()) / static_cast<float>(0xFFFFFFFFu);
}

uint8_t Screensaver::currentShapeIndex() const {
  constexpr uint32_t perShape = kHoldFrames + kMorphFrames;
  constexpr uint32_t cycleLength = static_cast<uint32_t>(kShapeCount) * perShape;
  const uint32_t cycleFrame = frameCounter_ % cycleLength;
  return static_cast<uint8_t>(cycleFrame / perShape);
}

void Screensaver::begin(uint32_t seed) {
  prng_ = (seed != 0) ? seed : 0x12345678u;
  initShapes();
  // Start in a randomly-chosen star mode so back-to-back screensaver
  // entries don't always look identical.
  starMode_ = (nextRandU32() & 1u) ? StarMode::Parallax : StarMode::Forward3D;
  initStarsForMode();
  for (int i = 0; i < kPointCount; ++i) {
    drawOrder_[i] = static_cast<uint16_t>(i);
    Point &p = points_[i];
    p.x = shapes_[0][i][0];
    p.y = shapes_[0][i][1];
    p.z = shapes_[0][i][2];
    p.cx = p.cy = 0.0f;
    p.cz = 1.0f;
    p.brightness = 255;
    // Spread the Pico-8 palette across the cluster — bias toward the brighter
    // half (indices 6..14) so the cube shape doesn't read as muddy.
    p.colorIndex = 6 + (i * 7) % 9;
  }
  frameCounter_ = 0;
  rotSegIdx_ = 0;
  rotSegFrame_ = 0;
  angleX_ = angleY_ = angleZ_ = 0.0f;
  yBias_ = 0.0f;
  lastShapeIdx_ = 0;
}

void Screensaver::initShapes() {
  // Local deterministic PRNG so the random shapes don't depend on the
  // begin() seed — keeps morph targets stable across re-entries.
  uint32_t s = 0xC0DECAFEu;
  auto frand = [&]() {
    s = s * 1664525u + 1013904223u;
    return static_cast<float>(s >> 8) / 16777216.0f;
  };

  // Shape 0 — cube grid (kPointsPerAxis³, fills exactly kPointCount).
  {
    int idx = 0;
    const float step = 2.0f / (kPointsPerAxis - 1);
    for (int i = 0; i < kPointsPerAxis; ++i) {
      for (int j = 0; j < kPointsPerAxis; ++j) {
        for (int k = 0; k < kPointsPerAxis; ++k) {
          shapes_[0][idx][0] = (-1.0f + i * step) * kModelScale;
          shapes_[0][idx][1] = (-1.0f + j * step) * kModelScale;
          shapes_[0][idx][2] = (-1.0f + k * step) * kModelScale;
          ++idx;
        }
      }
    }
  }

  // Shape 1 — sphere surface (uniform via inverse-CDF on cosφ).
  for (int n = 0; n < kPointCount; ++n) {
    const float u = frand();
    const float v = frand();
    const float theta = 2.0f * static_cast<float>(M_PI) * u;
    const float phi = acosf(2.0f * v - 1.0f);
    shapes_[1][n][0] = kSphereRadius * sinf(phi) * cosf(theta);
    shapes_[1][n][1] = kSphereRadius * sinf(phi) * sinf(theta);
    shapes_[1][n][2] = kSphereRadius * cosf(phi);
  }

  // Shape 2 — torus, R=1.0, r=0.45. u sweeps the major loop, v decorrelates
  // via the *11 multiplier so adjacent point indices land on different
  // points of the tube cross-section (richer morph trails).
  for (int n = 0; n < kPointCount; ++n) {
    const float u = (n / static_cast<float>(kPointCount)) * 2.0f * static_cast<float>(M_PI);
    const float v = ((n * 11) % kPointCount) / static_cast<float>(kPointCount)
                    * 2.0f * static_cast<float>(M_PI);
    constexpr float kMajorR = 1.0f;
    constexpr float kMinorR = 0.45f;
    shapes_[2][n][0] = (kMajorR + kMinorR * cosf(v)) * cosf(u) * kModelScale;
    shapes_[2][n][1] = kMinorR * sinf(v) * kModelScale;
    shapes_[2][n][2] = (kMajorR + kMinorR * cosf(v)) * sinf(u) * kModelScale;
  }

  // Shape 3 — single helix, 4 turns, height ±kModelScale.
  for (int n = 0; n < kPointCount; ++n) {
    const float t = n / static_cast<float>(kPointCount - 1);
    const float angle = t * 2.0f * static_cast<float>(M_PI) * 4.0f;
    shapes_[3][n][0] = cosf(angle) * 1.0f;
    shapes_[3][n][1] = (t * 2.0f - 1.0f) * kModelScale;
    shapes_[3][n][2] = sinf(angle) * 1.0f;
  }

  // Shape 4 — double helix (DNA), interleaved strands 180° apart.
  for (int n = 0; n < kPointCount; ++n) {
    const int strand = n & 1;
    const float t = (n / 2) / static_cast<float>(kPointCount / 2 - 1);
    const float angle = t * 2.0f * static_cast<float>(M_PI) * 3.0f
                        + (strand ? static_cast<float>(M_PI) : 0.0f);
    shapes_[4][n][0] = cosf(angle) * 1.1f;
    shapes_[4][n][1] = (t * 2.0f - 1.0f) * kModelScale;
    shapes_[4][n][2] = sinf(angle) * 1.1f;
  }

  // Shape 5 — random cloud, ±kModelScale.
  for (int n = 0; n < kPointCount; ++n) {
    shapes_[5][n][0] = (frand() * 2.0f - 1.0f) * kModelScale;
    shapes_[5][n][1] = (frand() * 2.0f - 1.0f) * kModelScale;
    shapes_[5][n][2] = (frand() * 2.0f - 1.0f) * kModelScale;
  }

  // Shape 6 — wave plane, cos·sin grid (Vectorball shape 0).
  // We arrange points on a sqrt(216)≈14.7 grid; round down to 14×14=196 and
  // tile the remaining 20 points across the surface so there are no gaps.
  {
    constexpr int kGrid = 14;  // 14² = 196 < 216
    int n = 0;
    for (int j = 0; j < kGrid && n < kPointCount; ++j) {
      for (int i = 0; i < kGrid && n < kPointCount; ++i) {
        const float u = (i / static_cast<float>(kGrid - 1)) * 2.0f - 1.0f;
        const float v = (j / static_cast<float>(kGrid - 1)) * 2.0f - 1.0f;
        shapes_[6][n][0] = u * kModelScale;
        shapes_[6][n][1] = cosf(2.0f * static_cast<float>(M_PI) * u)
                           * sinf(static_cast<float>(M_PI) * v) * 0.5f;
        shapes_[6][n][2] = v * kModelScale;
        ++n;
      }
    }
    // Fill remaining points by re-sampling random grid cells so morphing
    // still has 216 distinct targets.
    while (n < kPointCount) {
      const float u = frand() * 2.0f - 1.0f;
      const float v = frand() * 2.0f - 1.0f;
      shapes_[6][n][0] = u * kModelScale;
      shapes_[6][n][1] = cosf(2.0f * static_cast<float>(M_PI) * u)
                         * sinf(static_cast<float>(M_PI) * v) * 0.5f;
      shapes_[6][n][2] = v * kModelScale;
      ++n;
    }
  }

  // Shape 7 — Lissajous-blob (Vectorball shape 3 family). Three irrational
  // frequency ratios produce a non-repeating 3D figure-8 cluster.
  for (int n = 0; n < kPointCount; ++n) {
    shapes_[7][n][0] = cosf(0.069811f * n) * sinf(0.10472f * n) * 0.9f * kModelScale;
    shapes_[7][n][1] = sinf(0.089757f * n) * sinf(0.062832f * n) * 0.9f * kModelScale;
    shapes_[7][n][2] = sinf(0.125664f * n) * kModelScale;
  }

  // Shape 8 — tight 10-loop spiral (Vectorball shape 8). X linear, Y/Z
  // wound tightly — looks like a spring stretched out along the X axis.
  for (int n = 0; n < kPointCount; ++n) {
    const float t = n / static_cast<float>(kPointCount - 1);
    const float angle = t * 2.0f * static_cast<float>(M_PI) * 10.0f;
    shapes_[8][n][0] = (t * 2.0f - 1.0f) * kModelScale;
    shapes_[8][n][1] = sinf(angle) * 0.6f;
    shapes_[8][n][2] = cosf(angle) * 0.6f;
  }

  // Shape 9 — trefoil knot. Standard parametrisation, scaled to fit.
  for (int n = 0; n < kPointCount; ++n) {
    const float t = (n / static_cast<float>(kPointCount)) * 2.0f * static_cast<float>(M_PI);
    constexpr float kKnotScale = 0.5f;
    shapes_[9][n][0] = (sinf(t) + 2.0f * sinf(2.0f * t)) * kKnotScale;
    shapes_[9][n][1] = (cosf(t) - 2.0f * cosf(2.0f * t)) * kKnotScale;
    shapes_[9][n][2] = -sinf(3.0f * t) * kKnotScale * 1.5f;
  }
}

void Screensaver::initStarsForMode() {
  for (int i = 0; i < kStarCount; ++i) {
    Star &s = stars_[i];
    if (starMode_ == StarMode::Forward3D) {
      // Normalised 3D coords; project + brightness-derive in updateStars().
      s.a = nextRandFloat() * 2.0f - 1.0f;
      s.b = nextRandFloat() * 2.0f - 1.0f;
      s.c = nextRandFloat();  // z = 1 = far, 0 = nearest
    } else {
      // Parallax: spread across the panel, assign a layer.
      s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
      s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
      const uint8_t layer = i % 3;
      s.c = layer / 2.0f;  // 0, 0.5, 1.0 → maps to layer index in tick
    }
  }
}

void Screensaver::rerollStarMode() {
  // 50/50 toss to flip mode; either way, re-init star positions so the
  // transition is a clean cut rather than a jumbled mix.
  if ((nextRandU32() & 1u) == 0u) {
    starMode_ = (starMode_ == StarMode::Forward3D) ? StarMode::Parallax
                                                   : StarMode::Forward3D;
    initStarsForMode();
  }
}

void Screensaver::updateStars() {
  const int16_t halfW = kScreenWidth / 2;
  const int16_t halfH = kScreenHeight / 2;
  for (int i = 0; i < kStarCount; ++i) {
    Star &s = stars_[i];
    if (starMode_ == StarMode::Forward3D) {
      s.c -= 0.02f;
      if (s.c <= 0.0f) {
        s.a = nextRandFloat() * 2.0f - 1.0f;
        s.b = nextRandFloat() * 2.0f - 1.0f;
        s.c = 1.0f;
      }
      // Project — same formula the renderer used to do, now lives here.
      const float invZ = 1.0f / s.c;
      const int sx = halfW + static_cast<int>(s.a * halfW * invZ);
      const int sy = halfH + static_cast<int>(s.b * halfH * invZ);
      if (sx < 0 || sx >= kScreenWidth || sy < 0 || sy >= kScreenHeight) {
        s.brightness = 0;  // off-screen → renderer skips
        continue;
      }
      int b = static_cast<int>(255.0f * (1.0f - s.c));
      if (b < 0) b = 0;
      if (b > 255) b = 255;
      s.sx = static_cast<int16_t>(sx);
      s.sy = static_cast<int16_t>(sy);
      s.brightness = static_cast<uint8_t>(b);
    } else {
      // Parallax: scroll left at layer-dependent speed; wrap at the left
      // edge with a fresh Y so the field doesn't feel like a loop.
      const int layer = static_cast<int>(s.c * 2.0f + 0.5f);  // 0,1,2
      s.a -= kParallaxLayerSpeed[layer];
      if (s.a < 0.0f) {
        s.a += static_cast<float>(kScreenWidth);
        s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
      }
      s.sx = static_cast<int16_t>(s.a);
      s.sy = static_cast<int16_t>(s.b);
      s.brightness = kParallaxLayerBright[layer];
    }
  }
}

void Screensaver::updateAngles() {
  angleX_ += kRotDx[rotSegIdx_];
  angleY_ += kRotDy[rotSegIdx_];
  angleZ_ += kRotDz[rotSegIdx_];
  if (++rotSegFrame_ >= kRotFrames[rotSegIdx_]) {
    rotSegFrame_ = 0;
    rotSegIdx_ = (rotSegIdx_ + 1) % kRotSegments;
  }
}

void Screensaver::buildRotationMatrix(float &m00, float &m01, float &m02,
                                      float &m10, float &m11, float &m12,
                                      float &m20, float &m21, float &m22) const {
  // Tait-Bryan ZYX, mirrors Vectorball::buildRotationMatrix(). Off-diagonal
  // negations come from the original Equinox derivation.
  const float f1 = cosf(angleX_), f2 = sinf(angleX_);
  const float f3 = cosf(angleY_), f4 = sinf(angleY_);
  const float f5 = cosf(angleZ_), f6 = sinf(angleZ_);
  m00 = f1 * f3;
  m01 = -(f1 * f4 * f6 + f2 * f5);
  m02 = -(f1 * f4 * f5 - f2 * f6);
  m10 = f2 * f3;
  m11 = -(f2 * f4 * f6 - f1 * f5);
  m12 = -(f2 * f4 * f5 + f1 * f6);
  m20 = f4;
  m21 = f3 * f6;
  m22 = f3 * f5;
}

void Screensaver::shapeAt(int i, float &x, float &y, float &z) const {
  // Cycle: each shape holds for kHoldFrames, then morphs to next over
  // kMorphFrames. Total per shape = kHoldFrames + kMorphFrames.
  constexpr uint32_t perShape = kHoldFrames + kMorphFrames;
  constexpr uint32_t cycleLength = static_cast<uint32_t>(kShapeCount) * perShape;
  const uint32_t cycleFrame = frameCounter_ % cycleLength;
  const uint32_t shapeIdx = cycleFrame / perShape;
  const uint32_t inShape = cycleFrame - shapeIdx * perShape;
  if (inShape < kHoldFrames) {
    x = shapes_[shapeIdx][i][0];
    y = shapes_[shapeIdx][i][1];
    z = shapes_[shapeIdx][i][2];
  } else {
    const float t = static_cast<float>(inShape - kHoldFrames) / kMorphFrames;
    const uint32_t next = (shapeIdx + 1) % kShapeCount;
    x = clerp(shapes_[shapeIdx][i][0], shapes_[next][i][0], t);
    y = clerp(shapes_[shapeIdx][i][1], shapes_[next][i][1], t);
    z = clerp(shapes_[shapeIdx][i][2], shapes_[next][i][2], t);
  }
}

void Screensaver::tick() {
  ++frameCounter_;
  // Detect shape transition; reroll star mode each time the held shape
  // index advances. The ~9 s hold means the mode change is occasional, not
  // every frame — and a 50/50 toss inside reroll keeps it interesting.
  const uint8_t shapeIdx = currentShapeIndex();
  if (shapeIdx != lastShapeIdx_) {
    lastShapeIdx_ = shapeIdx;
    rerollStarMode();
  }
  updateAngles();
  updateStars();
  // Subtle Y-bias bob — pulls the cluster gently up/down across the screen
  // so it doesn't feel locked to the centre. ~10 s period at 60 fps.
  yBias_ = 0.25f * sinf(frameCounter_ * 0.01f);

  float m00, m01, m02, m10, m11, m12, m20, m21, m22;
  buildRotationMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);

  for (int i = 0; i < kPointCount; ++i) {
    Point &p = points_[i];
    shapeAt(i, p.x, p.y, p.z);
    const float rx = m00 * p.x + m01 * p.y + m02 * p.z;
    const float ry = m10 * p.x + m11 * p.y + m12 * p.z + yBias_;
    const float rz = m20 * p.x + m21 * p.y + m22 * p.z + kCameraZ;
    p.cx = rx;
    p.cy = ry;
    p.cz = (rz < 0.1f) ? 0.1f : rz;
    // Map cz onto kBrightNear..kBrightFar → 255..kMinBright.
    float t = (p.cz - kBrightNear) / (kBrightFar - kBrightNear);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const int b = 255 - static_cast<int>(t * (255 - kMinBright));
    p.brightness = static_cast<uint8_t>(b);
  }
}

void Screensaver::sortPoints() {
  // Insertion sort over the index array — n=216, mostly already-sorted
  // between frames (rotation is small per tick). Each swap moves 2 bytes
  // instead of ~52 — eliminates the Point struct copy that dominated the
  // previous sort. points_ stays put.
  for (int i = 1; i < kPointCount; ++i) {
    const uint16_t key = drawOrder_[i];
    const float keyCz = points_[key].cz;
    int j = i - 1;
    while (j >= 0 && points_[drawOrder_[j]].cz < keyCz) {
      drawOrder_[j + 1] = drawOrder_[j];
      --j;
    }
    drawOrder_[j + 1] = key;
  }
}
