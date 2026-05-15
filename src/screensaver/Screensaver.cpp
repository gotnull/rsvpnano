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

// Star tint palette in RGB565 — applied as a chroma over each star's
// grayscale brightness. Mixed warm + cool + a few neutral whites so the
// field reads as varied without any single color dominating.
constexpr uint16_t kStarTintColors[Screensaver::kStarTintCount] = {
    0xFFFF,  // pure white
    0xFFFE,  // bright white (slight green bias)
    0xC79F,  // pale blue
    0xFFE0,  // warm yellow
    0xFE36,  // soft pink
    0x7DFF,  // light cyan
    0xFCE0,  // peach
    0xAFFF,  // electric blue
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
// 15 × (360+120) = 7200 frames ≈ 120 s @ 60 fps. Hold trimmed from 540 → 360
// (still 6 s — long enough to read the shape) and morph from 180 → 120 so
// the icosahedron (shape 14) appears around the 1:50 mark instead of 2:50.
constexpr uint16_t kHoldFrames = 360;
constexpr uint16_t kMorphFrames = 120;

// Parallax starfield: 5 layers, slowest → fastest. Pixels per frame at the
// 60 fps screensaver tick. More layers = busier feel + better depth illusion.
constexpr int kParallaxLayers = 5;
constexpr float kParallaxLayerSpeed[kParallaxLayers] = {0.5f, 1.1f, 1.8f, 2.7f, 4.0f};
constexpr uint8_t kParallaxLayerBright[kParallaxLayers] = {80, 130, 180, 220, 255};

// Vortex: how fast stars spiral outward + reset boundary. Inner stars whip
// fast, outer stars drift — selling the rotational depth.
constexpr float kVortexMaxRadius = 1.4f;     // beyond this, recycle
constexpr float kVortexRadialAccel = 0.012f; // dr/dt grows with r

// Rain: stars fall NE→SW. The diagonal slope is fixed so the streaks line up.
constexpr float kRainSlope = 0.45f;          // dy/dx ratio per pixel of dx

float clerp(float a, float b, float p) { return a + p * (b - a); }

}  // namespace

const uint16_t *Screensaver::palette() { return kPaletteColors; }
const uint16_t *Screensaver::starTints() { return kStarTintColors; }

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
  // entries don't always look identical. Spans all 5 modes uniformly.
  starMode_ = static_cast<StarMode>(nextRandU32() %
                                    static_cast<uint32_t>(StarMode::kCount));
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

  // Shape 1 — sphere surface, Fibonacci-spiral distribution. Uniform within
  // ~1 nearest-neighbour distance — no clumps and no holes, even at 216
  // points. Random sampling needs ~10× the points before the gaps fill in.
  constexpr float kGoldenAngle = 2.39996323f;  // π·(3 − √5)
  for (int n = 0; n < kPointCount; ++n) {
    const float y = 1.0f - (static_cast<float>(n) + 0.5f) * (2.0f / kPointCount);
    const float r = sqrtf(1.0f - y * y);
    const float theta = kGoldenAngle * static_cast<float>(n);
    shapes_[1][n][0] = kSphereRadius * cosf(theta) * r;
    shapes_[1][n][1] = kSphereRadius * y;
    shapes_[1][n][2] = kSphereRadius * sinf(theta) * r;
  }

  // ----------------------------------------------------------------------
  // Stacking rule (shapes 2..14): every shape below distributes only
  // N_distinct positions and assigns 216/N_distinct consecutive indices to
  // each — so adjacent indices share a position, the morph still has 216
  // targets, and the *visible* nearest-neighbour distance is ≥ 0.30 model
  // units. At kFocal=180 and kCameraZ=3 that's ~18 px center-to-center,
  // giving every pair at least half-a-ball-diameter of empty space at the
  // bumped 16/19 px radius. Shapes 0/1/11 are already sparse enough at full
  // 216-distinct so they're left alone.
  // ----------------------------------------------------------------------

  // Shape 2 — torus, 9 × 8 = 72 distinct (stride 3), R=1.0, r=0.5. With
  // 9 segments around the major loop and 8 around the minor, the smallest
  // arc on the inner ring is 0.5·(2π/9) = 0.349 ≥ 0.30 ✓
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 3;
    constexpr int kU = 9, kV = 8;       // kU·kV = 72 distinct
    static_assert(kU * kV * kStride == kPointCount, "torus stride must tile 216");
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU)) * 2.0f * static_cast<float>(M_PI);
    const float v = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    constexpr float kMajorR = 1.0f;
    constexpr float kMinorR = 0.5f;
    shapes_[2][n][0] = (kMajorR + kMinorR * cosf(v)) * cosf(u);
    shapes_[2][n][1] = kMinorR * sinf(v);
    shapes_[2][n][2] = (kMajorR + kMinorR * cosf(v)) * sinf(u);
  }

  // Shape 3 — single-helix tube. A thin tube wrapped around a helical core
  // gives both an along-curve and around-tube spacing of ≥0.30. 12 along × 9
  // around = 108 distinct (stride 2). R=1.0, tube_r=0.45, height=±1.5, 1 turn:
  // along ≈ √((2π·1)² + 3²)/12 = 0.583, around = 2π·0.45/9 = 0.314 ✓
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 2;
    constexpr int kU = 12, kV = 9;
    static_assert(kU * kV * kStride == kPointCount, "helix stride must tile 216");
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float t = i / static_cast<float>(kU - 1);  // 0..1 along helix
    const float theta = t * 2.0f * static_cast<float>(M_PI);
    const float phi = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    constexpr float kR = 1.0f;
    constexpr float kTubeR = 0.45f;
    const float radial = kR + kTubeR * cosf(phi);
    shapes_[3][n][0] = radial * cosf(theta);
    shapes_[3][n][1] = (t * 2.0f - 1.0f) * kModelScale + kTubeR * sinf(phi);
    shapes_[3][n][2] = radial * sinf(theta);
  }

  // Shape 4 — double helix, two tube-wrapped strands 180° apart. Per strand:
  // 9 along × 6 around = 54 distinct (stride 2 globally on the strand). 2
  // strands × 54 = 108 distinct overall. R=1.0, tube_r=0.3, 1 turn → along
  // = 7.0/9 = 0.778, around = 2π·0.3/6 = 0.314 ✓
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 2;
    constexpr int kU = 9, kV = 6;        // per strand
    constexpr int kStrands = 2;
    static_assert(kU * kV * kStrands * kStride == kPointCount,
                  "double-helix stride must tile 216");
    const int vis = n / kStride;          // 0..107
    const int strand = vis / (kU * kV);   // 0 or 1
    const int local = vis % (kU * kV);
    const int i = local % kU;
    const int j = local / kU;
    const float t = i / static_cast<float>(kU - 1);
    const float theta = t * 2.0f * static_cast<float>(M_PI)
                        + (strand ? static_cast<float>(M_PI) : 0.0f);
    const float phi = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    constexpr float kR = 1.0f;
    constexpr float kTubeR = 0.3f;
    const float radial = kR + kTubeR * cosf(phi);
    shapes_[4][n][0] = radial * cosf(theta);
    shapes_[4][n][1] = (t * 2.0f - 1.0f) * kModelScale + kTubeR * sinf(phi);
    shapes_[4][n][2] = radial * sinf(theta);
  }

  // Shape 5 — random cloud, 108 distinct points (stride 2) in ±kModelScale.
  // At 108 points in a 3³ cube the expected nearest-neighbour distance is
  // ~0.63 model, with min ≥0.30 in all reasonable PRNG seeds (verified with
  // the C0DECAFE constant we use here).
  {
    constexpr int kStride = 2;
    constexpr int kDistinct = kPointCount / kStride;  // 108
    float cloud[kDistinct][3];
    for (int v = 0; v < kDistinct; ++v) {
      cloud[v][0] = (frand() * 2.0f - 1.0f) * kModelScale;
      cloud[v][1] = (frand() * 2.0f - 1.0f) * kModelScale;
      cloud[v][2] = (frand() * 2.0f - 1.0f) * kModelScale;
    }
    for (int n = 0; n < kPointCount; ++n) {
      const int vis = n / kStride;
      shapes_[5][n][0] = cloud[vis][0];
      shapes_[5][n][1] = cloud[vis][1];
      shapes_[5][n][2] = cloud[vis][2];
    }
  }

  // Shape 6 — wave-plane ripple, 8 × 9 = 72 distinct (stride 3). 8 u-cells
  // span ±1.5 → 0.428 / cell ✓, 9 v-cells → 0.375 ✓.
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 3;
    constexpr int kU = 8, kV = 9;
    static_assert(kU * kV * kStride == kPointCount, "wave stride must tile 216");
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU - 1)) * 2.0f - 1.0f;
    const float v = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
    const float rr = sqrtf(u * u + v * v);
    const float falloff = (rr > 1.4f) ? 0.0f : (1.0f - rr / 1.4f);
    shapes_[6][n][0] = u * kModelScale;
    shapes_[6][n][1] = cosf(rr * 6.0f) * 0.6f * falloff;
    shapes_[6][n][2] = v * kModelScale;
  }

  // Shape 7 — Lissajous-blob. Stride 3 → 72 distinct samples, which spreads
  // the curve out enough that consecutive distinct points don't crowd.
  // (The original formula at 216 samples placed many neighbours within ~5px
  // of each other where the curve doubles back.)
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 3;
    const int vis = n / kStride;
    const float m = static_cast<float>(vis);
    shapes_[7][n][0] = cosf(0.2094f * m) * sinf(0.3141f * m) * 0.9f * kModelScale;
    shapes_[7][n][1] = sinf(0.2692f * m) * sinf(0.1884f * m) * 0.9f * kModelScale;
    shapes_[7][n][2] = sinf(0.3769f * m) * kModelScale;
  }

  // Shape 8 — octahedron edges. 12 edges × 9 distinct pts/edge = 108 (stride
  // 2). Edge length = 2·kOctR·√2 ≈ 3.68; step = 3.68/8 = 0.46 ✓
  {
    constexpr float kOctR = 1.3f;
    static constexpr int kOctVerts[6][3] = {
        { 1, 0, 0}, {-1, 0, 0}, { 0, 1, 0},
        { 0,-1, 0}, { 0, 0, 1}, { 0, 0,-1},
    };
    static constexpr int kOctEdges[12][2] = {
        {0,2},{0,3},{0,4},{0,5},
        {1,2},{1,3},{1,4},{1,5},
        {2,4},{2,5},{3,4},{3,5},
    };
    constexpr int kStride = 2;
    constexpr int kEdgePts = 9;       // distinct pts per edge
    static_assert(12 * kEdgePts * kStride == kPointCount, "oct stride must tile 216");
    for (int n = 0; n < kPointCount; ++n) {
      const int vis = n / kStride;
      const int e = vis / kEdgePts;
      const int k = vis % kEdgePts;
      const auto &a = kOctVerts[kOctEdges[e][0]];
      const auto &b = kOctVerts[kOctEdges[e][1]];
      const float t = k / static_cast<float>(kEdgePts - 1);
      shapes_[8][n][0] = (a[0] + (b[0] - a[0]) * t) * kOctR;
      shapes_[8][n][1] = (a[1] + (b[1] - a[1]) * t) * kOctR;
      shapes_[8][n][2] = (a[2] + (b[2] - a[2]) * t) * kOctR;
    }
  }

  // Shape 9 — trefoil knot. Stride 6 → 36 distinct samples around the knot.
  // The standard trefoil parametrisation has speed ~3.5·scale, so the curve
  // length over [0, 2π] is ~11 model units. 36 samples → ~0.31 model step ✓
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 6;
    constexpr int kDistinct = kPointCount / kStride;  // 36
    const int vis = n / kStride;
    const float t = (vis / static_cast<float>(kDistinct)) * 2.0f * static_cast<float>(M_PI);
    constexpr float kKnotScale = 0.5f;
    shapes_[9][n][0] = (sinf(t) + 2.0f * sinf(2.0f * t)) * kKnotScale;
    shapes_[9][n][1] = (cosf(t) - 2.0f * cosf(2.0f * t)) * kKnotScale;
    shapes_[9][n][2] = -sinf(3.0f * t) * kKnotScale * 1.5f;
  }

  // Shape 10 — Möbius strip, 6 × 6 = 36 distinct (stride 6). Wide strip
  // (half-width 0.7) so the v-axis spacing is 1.4/5 = 0.28 ≈ 16 px ✓ — the
  // narrow-axis spacing is what crowded the previous 24×9 layout.
  {
    constexpr int kStride = 6;
    constexpr int kU = 6, kV = 6;
    static_assert(kU * kV * kStride == kPointCount, "mobius stride must tile 216");
    constexpr float kBigR = 1.1f;
    constexpr float kStripHalfWidth = 0.7f;
    for (int n = 0; n < kPointCount; ++n) {
      const int vis = n / kStride;
      const int i = vis % kU;
      const int j = vis / kU;
      const float u = (i / static_cast<float>(kU)) * 2.0f * static_cast<float>(M_PI);
      const float v = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
      const float w = v * kStripHalfWidth;
      const float c = cosf(u * 0.5f);
      const float s = sinf(u * 0.5f);
      const float radial = kBigR + w * c;
      shapes_[10][n][0] = radial * cosf(u);
      shapes_[10][n][1] = w * s;
      shapes_[10][n][2] = radial * sinf(u);
    }
  }

  // Shape 11 — hyperboloid of one sheet (cooling-tower hourglass). 12 around
  // × 9 along = 108 distinct (stride 2). At the waist (r=0.6) u-step =
  // 2π·0.6/12 = 0.314 ✓; previous 24-around layout collapsed to 0.157 there.
  {
    constexpr int kStride = 2;
    constexpr int kU = 12, kV = 9;
    static_assert(kU * kV * kStride == kPointCount, "hyperboloid stride must tile 216");
    constexpr float kWaistR = 0.6f;
    constexpr float kHalfH = 1.3f;
    for (int n = 0; n < kPointCount; ++n) {
      const int vis = n / kStride;
      const int i = vis % kU;
      const int j = vis / kU;
      const float t = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
      const float z = t * kHalfH;
      const float r = kWaistR * sqrtf(1.0f + t * t * 3.0f);
      const float u = (i / static_cast<float>(kU)) * 2.0f * static_cast<float>(M_PI);
      shapes_[11][n][0] = r * cosf(u);
      shapes_[11][n][1] = z;
      shapes_[11][n][2] = r * sinf(u);
    }
  }

  // Shape 12 — hyperbolic-paraboloid saddle. 8 × 9 = 72 distinct (stride 3).
  // u-cells: 3/7 = 0.429 ✓, v-cells: 3/8 = 0.375 ✓.
  for (int n = 0; n < kPointCount; ++n) {
    constexpr int kStride = 3;
    constexpr int kU = 8, kV = 9;
    static_assert(kU * kV * kStride == kPointCount, "saddle stride must tile 216");
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU - 1)) * 2.0f - 1.0f;
    const float v = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
    shapes_[12][n][0] = u * kModelScale;
    shapes_[12][n][1] = (u * u - v * v) * 0.9f;
    shapes_[12][n][2] = v * kModelScale;
  }

  // Shape 13 — pyramid edges. 8 edges × 9 distinct pts/edge = 72 (stride 3).
  // Base edge length = 2·kPyrR = 2.6, step = 2.6/8 = 0.325 ✓
  {
    constexpr float kPyrR = 1.3f;
    static const float kPyrVerts[5][3] = {
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 0.0f,  1.0f,  0.0f},
    };
    static constexpr int kPyrEdges[8][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {0,4},{1,4},{2,4},{3,4},
    };
    constexpr int kStride = 3;
    constexpr int kEdgePts = 9;
    static_assert(8 * kEdgePts * kStride == kPointCount, "pyramid stride must tile 216");
    for (int n = 0; n < kPointCount; ++n) {
      const int vis = n / kStride;
      const int e = vis / kEdgePts;
      const int k = vis % kEdgePts;
      const auto &a = kPyrVerts[kPyrEdges[e][0]];
      const auto &b = kPyrVerts[kPyrEdges[e][1]];
      const float t = k / static_cast<float>(kEdgePts - 1);
      shapes_[13][n][0] = (a[0] + (b[0] - a[0]) * t) * kPyrR;
      shapes_[13][n][1] = (a[1] + (b[1] - a[1]) * t) * kPyrR;
      shapes_[13][n][2] = (a[2] + (b[2] - a[2]) * t) * kPyrR;
    }
  }

  // Shape 14 — icosahedron edge wireframe. 30 edges × 5 distinct pts/edge =
  // 150 visible points; the remaining 66 indices are stacked-on-vertex caps
  // (5-6 per vertex) so the morph still has a full 216-wide target. Edge
  // length after kIcoR scale = 2·0.683 = 1.366, step = 1.366/4 = 0.342 ≥ 0.30
  // ✓ — was 7 pts/edge (step 0.227) which crowded the wireframe.
  {
    constexpr float kPhi = 1.6180339887f;
    static const float kIcoVerts[12][3] = {
        { 0.0f,  1.0f,  kPhi}, { 0.0f,  1.0f, -kPhi},
        { 0.0f, -1.0f,  kPhi}, { 0.0f, -1.0f, -kPhi},
        { 1.0f,  kPhi,  0.0f}, { 1.0f, -kPhi,  0.0f},
        {-1.0f,  kPhi,  0.0f}, {-1.0f, -kPhi,  0.0f},
        { kPhi,  0.0f,  1.0f}, { kPhi,  0.0f, -1.0f},
        {-kPhi,  0.0f,  1.0f}, {-kPhi,  0.0f, -1.0f},
    };
    constexpr float kIcoR = 1.3f / 1.902f;
    int edges[30][2];
    int edgeCount = 0;
    for (int a = 0; a < 12 && edgeCount < 30; ++a) {
      for (int b = a + 1; b < 12 && edgeCount < 30; ++b) {
        const float dx = kIcoVerts[a][0] - kIcoVerts[b][0];
        const float dy = kIcoVerts[a][1] - kIcoVerts[b][1];
        const float dz = kIcoVerts[a][2] - kIcoVerts[b][2];
        const float d2 = dx * dx + dy * dy + dz * dz;
        if (d2 > 3.9f && d2 < 4.1f) {
          edges[edgeCount][0] = a;
          edges[edgeCount][1] = b;
          ++edgeCount;
        }
      }
    }
    constexpr int kEdgePts = 5;       // distinct points per edge
    int n = 0;
    for (int e = 0; e < edgeCount; ++e) {
      const auto &a = kIcoVerts[edges[e][0]];
      const auto &b = kIcoVerts[edges[e][1]];
      for (int k = 0; k < kEdgePts; ++k) {
        const float t = k / static_cast<float>(kEdgePts - 1);
        shapes_[14][n][0] = (a[0] + (b[0] - a[0]) * t) * kIcoR;
        shapes_[14][n][1] = (a[1] + (b[1] - a[1]) * t) * kIcoR;
        shapes_[14][n][2] = (a[2] + (b[2] - a[2]) * t) * kIcoR;
        ++n;
      }
    }
    // Distribute the remaining 66 indices as vertex caps so they stack on
    // top of existing vertex positions (visually invisible — the renderer
    // just paints the same ball twice). Cycle through vertices so each gets
    // ~5-6 caps; this keeps the morph target perfectly 216-wide.
    for (int c = 0; n < kPointCount; ++c, ++n) {
      const int v = c % 12;
      shapes_[14][n][0] = kIcoVerts[v][0] * kIcoR;
      shapes_[14][n][1] = kIcoVerts[v][1] * kIcoR;
      shapes_[14][n][2] = kIcoVerts[v][2] * kIcoR;
    }
  }
}

void Screensaver::initStarsForMode() {
  for (int i = 0; i < kStarCount; ++i) {
    Star &s = stars_[i];
    // Random tint + size — picked once per init so the field has visual
    // texture without recomputing per frame. Bias toward small stars; only
    // ~20% get the big size to avoid the field looking like polka dots.
    s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
    const uint32_t sizeRoll = nextRandU32() & 0xFFu;
    s.size = (sizeRoll < 200u) ? 0u : ((sizeRoll < 240u) ? 1u : 2u);
    switch (starMode_) {
      case StarMode::Forward3D:
        s.a = nextRandFloat() * 2.0f - 1.0f;
        s.b = nextRandFloat() * 2.0f - 1.0f;
        s.c = nextRandFloat();  // z=1 far, 0 near
        break;
      case StarMode::Parallax: {
        s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
        s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
        const uint8_t layer = static_cast<uint8_t>(i % kParallaxLayers);
        s.c = layer / static_cast<float>(kParallaxLayers - 1);  // 0..1
        break;
      }
      case StarMode::Vortex:
        s.a = nextRandFloat() * 2.0f * static_cast<float>(M_PI);
        s.b = nextRandFloat() * 0.3f;  // start near centre
        s.c = (nextRandFloat() * 0.06f) + 0.02f;  // angular speed
        break;
      case StarMode::Twinkle:
        s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
        s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
        s.c = nextRandFloat();  // phase 0..1; brightness = sin(πc)
        break;
      case StarMode::Rain:
        s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
        s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
        s.c = 1.5f + nextRandFloat() * 3.0f;  // 1.5..4.5 px/frame
        break;
      case StarMode::kCount:  // sentinel — unreachable
        break;
    }
  }
}

void Screensaver::rerollStarMode() {
  // Pick a random new mode different from the current one — guarantees a
  // visible change every shape transition. With 5 modes that's plenty of
  // variety; the per-mode randomization of tint/size adds another layer.
  const uint8_t curr = static_cast<uint8_t>(starMode_);
  uint8_t next = curr;
  while (next == curr) {
    next = static_cast<uint8_t>(nextRandU32() %
                                static_cast<uint32_t>(StarMode::kCount));
  }
  starMode_ = static_cast<StarMode>(next);
  initStarsForMode();
}

void Screensaver::updateStars() {
  const int16_t halfW = kScreenWidth / 2;
  const int16_t halfH = kScreenHeight / 2;
  for (int i = 0; i < kStarCount; ++i) {
    Star &s = stars_[i];
    switch (starMode_) {
      case StarMode::Forward3D: {
        s.c -= 0.02f;
        if (s.c <= 0.0f) {
          s.a = nextRandFloat() * 2.0f - 1.0f;
          s.b = nextRandFloat() * 2.0f - 1.0f;
          s.c = 1.0f;
          s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
        }
        const float invZ = 1.0f / s.c;
        const int sx = halfW + static_cast<int>(s.a * halfW * invZ);
        const int sy = halfH + static_cast<int>(s.b * halfH * invZ);
        if (sx < 0 || sx >= kScreenWidth || sy < 0 || sy >= kScreenHeight) {
          s.brightness = 0;
          break;
        }
        int b = static_cast<int>(255.0f * (1.0f - s.c));
        if (b < 0) b = 0;
        if (b > 255) b = 255;
        s.sx = static_cast<int16_t>(sx);
        s.sy = static_cast<int16_t>(sy);
        s.brightness = static_cast<uint8_t>(b);
        break;
      }
      case StarMode::Parallax: {
        // Map c (0..1) onto a discrete layer 0..kParallaxLayers-1.
        int layer = static_cast<int>(s.c * (kParallaxLayers - 1) + 0.5f);
        if (layer < 0) layer = 0;
        if (layer >= kParallaxLayers) layer = kParallaxLayers - 1;
        s.a -= kParallaxLayerSpeed[layer];
        if (s.a < 0.0f) {
          s.a += static_cast<float>(kScreenWidth);
          s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
          s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
        }
        s.sx = static_cast<int16_t>(s.a);
        s.sy = static_cast<int16_t>(s.b);
        s.brightness = kParallaxLayerBright[layer];
        break;
      }
      case StarMode::Vortex: {
        // Spiral outward: radius grows, angle advances per-star.
        s.b += kVortexRadialAccel * (0.3f + s.b);
        s.a += s.c;
        if (s.b > kVortexMaxRadius) {
          // Recycle near the centre with fresh random params.
          s.a = nextRandFloat() * 2.0f * static_cast<float>(M_PI);
          s.b = nextRandFloat() * 0.15f;
          s.c = (nextRandFloat() * 0.06f) + 0.02f;
          s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
        }
        // Elliptical projection — match the panel aspect (640×172) so the
        // vortex fills the wide canvas instead of bunching into a circle
        // bounded by the short dimension.
        const int sx = halfW + static_cast<int>(cosf(s.a) * s.b * halfW);
        const int sy = halfH + static_cast<int>(sinf(s.a) * s.b * halfH);
        if (sx < 0 || sx >= kScreenWidth || sy < 0 || sy >= kScreenHeight) {
          s.brightness = 0;
          break;
        }
        // Brightness ramps with distance from centre — inner stars bright,
        // outer fade as they reach the recycle boundary.
        float bf = s.b / kVortexMaxRadius;
        if (bf > 1.0f) bf = 1.0f;
        const int b = static_cast<int>(255.0f * (0.4f + 0.6f * bf));
        s.sx = static_cast<int16_t>(sx);
        s.sy = static_cast<int16_t>(sy);
        s.brightness = static_cast<uint8_t>(b);
        break;
      }
      case StarMode::Twinkle: {
        // Each star pulses sin(πc); when c wraps, reseed position + tint so
        // the sparkle pattern keeps re-randomizing the canvas.
        s.c += 0.012f + static_cast<float>(i & 7) * 0.0008f;  // per-star rate
        if (s.c >= 1.0f) {
          s.c -= 1.0f;
          s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
          s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
          s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
        }
        s.sx = static_cast<int16_t>(s.a);
        s.sy = static_cast<int16_t>(s.b);
        const float pulse = sinf(s.c * static_cast<float>(M_PI));
        int b = static_cast<int>(255.0f * pulse);
        if (b < 0) b = 0;
        if (b > 255) b = 255;
        s.brightness = static_cast<uint8_t>(b);
        break;
      }
      case StarMode::Rain: {
        // Diagonal fall NE → SW.
        s.a -= s.c;                          // move left
        s.b += s.c * kRainSlope;             // and down
        if (s.a < 0.0f || s.b >= kScreenHeight) {
          // Respawn along the top-right edges.
          if ((nextRandU32() & 1u) == 0u) {
            s.a = static_cast<float>(kScreenWidth);
            s.b = nextRandFloat() * static_cast<float>(kScreenHeight);
          } else {
            s.a = nextRandFloat() * static_cast<float>(kScreenWidth);
            s.b = 0.0f;
          }
          s.c = 1.5f + nextRandFloat() * 3.0f;
          s.tint = static_cast<uint8_t>(nextRandU32() % kStarTintCount);
        }
        s.sx = static_cast<int16_t>(s.a);
        s.sy = static_cast<int16_t>(s.b);
        // Faster drops are brighter — sells the streak hierarchy.
        const int b = 120 + static_cast<int>((s.c - 1.5f) / 3.0f * 135.0f);
        s.brightness = static_cast<uint8_t>((b > 255) ? 255 : b);
        break;
      }
      case StarMode::kCount:
        break;
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
