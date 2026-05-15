#include "screensaver/ShapeRegistry.h"

#include <math.h>

// Shape generators. Each function fills its ShapeBuffer in-place with
// kPointCount = 216 triplets. The spacing/stride rationales live next to
// each generator — see the orchestrator's comment block on the "stacking
// rule" for the big picture (minimum 0.30 model units between visible
// neighbours so balls don't visually merge at the bumped 16/19 px radius).

namespace screensaver {
namespace {

constexpr int kPointCount = Screensaver::kPointCount;
constexpr float kModelScale = 1.5f;
constexpr float kSphereRadius = 1.5f;

// ----------------------------------------------------------------------------
// Shape 0 — cube grid (kPointsPerAxis³, fills exactly kPointCount).
// ----------------------------------------------------------------------------
void genCube(ShapeBuffer& out, ShapeRng&) {
  constexpr int kPerAxis = Screensaver::kPointsPerAxis;
  const float step = 2.0f / (kPerAxis - 1);
  int idx = 0;
  for (int i = 0; i < kPerAxis; ++i) {
    for (int j = 0; j < kPerAxis; ++j) {
      for (int k = 0; k < kPerAxis; ++k) {
        out[idx][0] = (-1.0f + i * step) * kModelScale;
        out[idx][1] = (-1.0f + j * step) * kModelScale;
        out[idx][2] = (-1.0f + k * step) * kModelScale;
        ++idx;
      }
    }
  }
}

// ----------------------------------------------------------------------------
// Shape 1 — sphere surface, Fibonacci-spiral distribution. Uniform within
// ~1 nearest-neighbour distance — no clumps and no holes, even at 216
// points. Random sampling needs ~10× the points before the gaps fill in.
// ----------------------------------------------------------------------------
void genSphere(ShapeBuffer& out, ShapeRng&) {
  constexpr float kGoldenAngle = 2.39996323f;  // π·(3 − √5)
  for (int n = 0; n < kPointCount; ++n) {
    const float y = 1.0f - (static_cast<float>(n) + 0.5f) * (2.0f / kPointCount);
    const float r = sqrtf(1.0f - y * y);
    const float theta = kGoldenAngle * static_cast<float>(n);
    out[n][0] = kSphereRadius * cosf(theta) * r;
    out[n][1] = kSphereRadius * y;
    out[n][2] = kSphereRadius * sinf(theta) * r;
  }
}

// ----------------------------------------------------------------------------
// Stacking rule (shapes 2..14): every shape below distributes only
// N_distinct positions and assigns 216/N_distinct consecutive indices to
// each — so adjacent indices share a position, the morph still has 216
// targets, and the *visible* nearest-neighbour distance is ≥ 0.30 model
// units (~18 px at kFocal=180, kCameraZ=3). Shapes 0/1 are already sparse
// enough at full 216-distinct so they're left alone.
// ----------------------------------------------------------------------------

// Shape 2 — torus, 9 × 8 = 72 distinct (stride 3), R=1.0, r=0.5. With
// 9 segments around the major loop and 8 around the minor, the smallest
// arc on the inner ring is 0.5·(2π/9) = 0.349 ≥ 0.30 ✓
void genTorus(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
  constexpr int kU = 9, kV = 8;
  static_assert(kU * kV * kStride == kPointCount, "torus stride must tile 216");
  constexpr float kMajorR = 1.0f;
  constexpr float kMinorR = 0.5f;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU)) * 2.0f * static_cast<float>(M_PI);
    const float v = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    out[n][0] = (kMajorR + kMinorR * cosf(v)) * cosf(u);
    out[n][1] = kMinorR * sinf(v);
    out[n][2] = (kMajorR + kMinorR * cosf(v)) * sinf(u);
  }
}

// Shape 3 — single-helix tube. 12 along × 9 around = 108 distinct (stride 2).
// R=1.0, tube_r=0.45, height=±1.5, 1 turn: along ≈ √((2π·1)² + 3²)/12 = 0.583,
// around = 2π·0.45/9 = 0.314 ✓
void genHelix(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 2;
  constexpr int kU = 12, kV = 9;
  static_assert(kU * kV * kStride == kPointCount, "helix stride must tile 216");
  constexpr float kR = 1.0f;
  constexpr float kTubeR = 0.45f;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float t = i / static_cast<float>(kU - 1);
    const float theta = t * 2.0f * static_cast<float>(M_PI);
    const float phi = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    const float radial = kR + kTubeR * cosf(phi);
    out[n][0] = radial * cosf(theta);
    out[n][1] = (t * 2.0f - 1.0f) * kModelScale + kTubeR * sinf(phi);
    out[n][2] = radial * sinf(theta);
  }
}

// Shape 4 — double helix, two tube-wrapped strands 180° apart. Per strand:
// 9 along × 6 around = 54 distinct (stride 2 globally on the strand). 2
// strands × 54 = 108 distinct overall. along=7.0/9=0.778, around=0.314 ✓
void genDoubleHelix(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 2;
  constexpr int kU = 9, kV = 6;
  constexpr int kStrands = 2;
  static_assert(kU * kV * kStrands * kStride == kPointCount,
                "double-helix stride must tile 216");
  constexpr float kR = 1.0f;
  constexpr float kTubeR = 0.3f;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int strand = vis / (kU * kV);
    const int local = vis % (kU * kV);
    const int i = local % kU;
    const int j = local / kU;
    const float t = i / static_cast<float>(kU - 1);
    const float theta = t * 2.0f * static_cast<float>(M_PI)
                        + (strand ? static_cast<float>(M_PI) : 0.0f);
    const float phi = (j / static_cast<float>(kV)) * 2.0f * static_cast<float>(M_PI);
    const float radial = kR + kTubeR * cosf(phi);
    out[n][0] = radial * cosf(theta);
    out[n][1] = (t * 2.0f - 1.0f) * kModelScale + kTubeR * sinf(phi);
    out[n][2] = radial * sinf(theta);
  }
}

// Shape 5 — random cloud, 108 distinct points (stride 2) in ±kModelScale.
// At 108 points in a 3³ cube the expected nearest-neighbour distance is
// ~0.63 model. RNG state is supplied by the caller for determinism.
void genRandomCloud(ShapeBuffer& out, ShapeRng& rng) {
  constexpr int kStride = 2;
  constexpr int kDistinct = kPointCount / kStride;  // 108
  float cloud[kDistinct][3];
  for (int v = 0; v < kDistinct; ++v) {
    cloud[v][0] = (rng.frand() * 2.0f - 1.0f) * kModelScale;
    cloud[v][1] = (rng.frand() * 2.0f - 1.0f) * kModelScale;
    cloud[v][2] = (rng.frand() * 2.0f - 1.0f) * kModelScale;
  }
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    out[n][0] = cloud[vis][0];
    out[n][1] = cloud[vis][1];
    out[n][2] = cloud[vis][2];
  }
}

// Shape 6 — wave-plane ripple, 8 × 9 = 72 distinct (stride 3).
void genWavePlane(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
  constexpr int kU = 8, kV = 9;
  static_assert(kU * kV * kStride == kPointCount, "wave stride must tile 216");
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU - 1)) * 2.0f - 1.0f;
    const float v = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
    const float rr = sqrtf(u * u + v * v);
    const float falloff = (rr > 1.4f) ? 0.0f : (1.0f - rr / 1.4f);
    out[n][0] = u * kModelScale;
    out[n][1] = cosf(rr * 6.0f) * 0.6f * falloff;
    out[n][2] = v * kModelScale;
  }
}

// Shape 7 — Lissajous blob, stride 3 → 72 distinct samples.
void genLissajous(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const float m = static_cast<float>(vis);
    out[n][0] = cosf(0.2094f * m) * sinf(0.3141f * m) * 0.9f * kModelScale;
    out[n][1] = sinf(0.2692f * m) * sinf(0.1884f * m) * 0.9f * kModelScale;
    out[n][2] = sinf(0.3769f * m) * kModelScale;
  }
}

// Shape 8 — octahedron edges. 12 edges × 9 distinct pts/edge = 108 (stride 2).
// Edge length = 2·1.3·√2 ≈ 3.68; step = 3.68/8 = 0.46 ✓
void genOctahedron(ShapeBuffer& out, ShapeRng&) {
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
  constexpr int kEdgePts = 9;
  static_assert(12 * kEdgePts * kStride == kPointCount, "oct stride must tile 216");
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int e = vis / kEdgePts;
    const int k = vis % kEdgePts;
    const auto &a = kOctVerts[kOctEdges[e][0]];
    const auto &b = kOctVerts[kOctEdges[e][1]];
    const float t = k / static_cast<float>(kEdgePts - 1);
    out[n][0] = (a[0] + (b[0] - a[0]) * t) * kOctR;
    out[n][1] = (a[1] + (b[1] - a[1]) * t) * kOctR;
    out[n][2] = (a[2] + (b[2] - a[2]) * t) * kOctR;
  }
}

// Shape 9 — trefoil knot, stride 6 → 36 distinct samples (~0.31 model step).
void genTrefoil(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 6;
  constexpr int kDistinct = kPointCount / kStride;
  constexpr float kKnotScale = 0.5f;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const float t = (vis / static_cast<float>(kDistinct)) * 2.0f * static_cast<float>(M_PI);
    out[n][0] = (sinf(t) + 2.0f * sinf(2.0f * t)) * kKnotScale;
    out[n][1] = (cosf(t) - 2.0f * cosf(2.0f * t)) * kKnotScale;
    out[n][2] = -sinf(3.0f * t) * kKnotScale * 1.5f;
  }
}

// Shape 10 — Möbius strip, 6 × 6 = 36 distinct (stride 6). Wide strip
// (half-width 0.7) so the v-axis spacing is 1.4/5 = 0.28 ✓
void genMobius(ShapeBuffer& out, ShapeRng&) {
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
    out[n][0] = radial * cosf(u);
    out[n][1] = w * s;
    out[n][2] = radial * sinf(u);
  }
}

// Shape 11 — hyperboloid of one sheet. 12 around × 9 along = 108 distinct
// (stride 2). At the waist (r=0.6) u-step = 2π·0.6/12 = 0.314 ✓
void genHyperboloid(ShapeBuffer& out, ShapeRng&) {
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
    out[n][0] = r * cosf(u);
    out[n][1] = z;
    out[n][2] = r * sinf(u);
  }
}

// Shape 12 — hyperbolic-paraboloid saddle. 8 × 9 = 72 distinct (stride 3).
void genSaddle(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
  constexpr int kU = 8, kV = 9;
  static_assert(kU * kV * kStride == kPointCount, "saddle stride must tile 216");
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const int i = vis % kU;
    const int j = vis / kU;
    const float u = (i / static_cast<float>(kU - 1)) * 2.0f - 1.0f;
    const float v = (j / static_cast<float>(kV - 1)) * 2.0f - 1.0f;
    out[n][0] = u * kModelScale;
    out[n][1] = (u * u - v * v) * 0.9f;
    out[n][2] = v * kModelScale;
  }
}

// Shape 13 — pyramid edges. 8 edges × 9 distinct pts/edge = 72 (stride 3).
void genPyramid(ShapeBuffer& out, ShapeRng&) {
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
    out[n][0] = (a[0] + (b[0] - a[0]) * t) * kPyrR;
    out[n][1] = (a[1] + (b[1] - a[1]) * t) * kPyrR;
    out[n][2] = (a[2] + (b[2] - a[2]) * t) * kPyrR;
  }
}

// Shape 14 — icosahedron edge wireframe. 30 edges × 5 distinct pts/edge =
// 150 visible points; the remaining 66 indices are stacked-on-vertex caps
// (5-6 per vertex) so the morph still has a full 216-wide target. Edge step
// after kIcoR scale = 1.366/4 = 0.342 ≥ 0.30 ✓
void genIcosahedron(ShapeBuffer& out, ShapeRng&) {
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
  constexpr int kEdgePts = 5;
  int n = 0;
  for (int e = 0; e < edgeCount; ++e) {
    const auto &a = kIcoVerts[edges[e][0]];
    const auto &b = kIcoVerts[edges[e][1]];
    for (int k = 0; k < kEdgePts; ++k) {
      const float t = k / static_cast<float>(kEdgePts - 1);
      out[n][0] = (a[0] + (b[0] - a[0]) * t) * kIcoR;
      out[n][1] = (a[1] + (b[1] - a[1]) * t) * kIcoR;
      out[n][2] = (a[2] + (b[2] - a[2]) * t) * kIcoR;
      ++n;
    }
  }
  // Remaining 66 indices stack on existing vertex positions (renderer paints
  // them on top of edge endpoints — visually no-op, keeps morph 216-wide).
  for (int c = 0; n < kPointCount; ++c, ++n) {
    const int v = c % 12;
    out[n][0] = kIcoVerts[v][0] * kIcoR;
    out[n][1] = kIcoVerts[v][1] * kIcoR;
    out[n][2] = kIcoVerts[v][2] * kIcoR;
  }
}

// Order must match the integer shape indices used by the timeline.
constexpr ShapeGenerator kRegistry[] = {
    {"cube",         genCube},
    {"sphere",       genSphere},
    {"torus",        genTorus},
    {"helix",        genHelix},
    {"double-helix", genDoubleHelix},
    {"cloud",        genRandomCloud},
    {"wave-plane",   genWavePlane},
    {"lissajous",    genLissajous},
    {"octahedron",   genOctahedron},
    {"trefoil",      genTrefoil},
    {"mobius",       genMobius},
    {"hyperboloid",  genHyperboloid},
    {"saddle",       genSaddle},
    {"pyramid",      genPyramid},
    {"icosahedron",  genIcosahedron},
};

static_assert(sizeof(kRegistry) / sizeof(kRegistry[0]) == Screensaver::kShapeCount,
              "shape registry size must match Screensaver::kShapeCount");

}  // namespace

const ShapeGenerator* shapeRegistry() { return kRegistry; }

}  // namespace screensaver
