#include "screensaver/ShapeRegistry.h"

#include <math.h>

// Shape generators. Each function fills its ShapeBuffer in-place with
// kPointCount = 216 triplets.
//
// Density philosophy: every shape has its own natural feel — a 36-point
// trefoil reads as a wireframe knot, a 216-point trefoil reads as a thick
// tube. Rather than enforcing one global spacing, each generator picks a
// stride / grid that preserves the classic Equinox-vectorball density for
// that topology. Strides + grid dimensions are constexpr at the top of
// each function so the tuning knob is obvious. The earlier "half-diameter
// gap" rule (stride doubled across the board) made the cluster too sparse
// and lost the vectorball look — halved back here.

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

// Shape 2 — torus, 12 × 9 = 108 distinct (stride 2), R=1.0, r=0.5. Inner-
// ring arc = 0.5·(2π/12) = 0.262, v-step = 0.5·(2π/9) = 0.349. Doubles the
// distinct count vs the previous (9×8=72) so the ring reads as a solid tube.
void genTorus(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 2;
  constexpr int kU = 12, kV = 9;
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

// Shape 3 — single-helix tube. 24 along × 9 around = 216 distinct (stride 1).
// R=1.0, tube_r=0.45, height=±1.5, 1 turn: along = √((2π·1)² + 3²)/24 = 0.292,
// around = 2π·0.45/9 = 0.314. Doubles along-curve density so the tube
// looks solid instead of stretchy.
void genHelix(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  constexpr int kU = 24, kV = 9;
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
// 18 along × 6 around = 108 distinct (stride 1 globally). 2 strands × 108
// = 216 indices. along = 7.0/18 = 0.389, around = 2π·0.3/6 = 0.314. Each
// strand now reads as a continuous coil rather than a sparse dotted line.
void genDoubleHelix(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  constexpr int kU = 18, kV = 6;
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

// Shape 5 — random cloud, 216 distinct points (stride 1) in ±kModelScale.
// At 216 points in a 3³ cube the expected nearest-neighbour distance is
// ~0.5 model; minimum will be smaller in pockets — that's the natural
// cloud look (clusters + gaps are part of the vectorball aesthetic).
void genRandomCloud(ShapeBuffer& out, ShapeRng& rng) {
  constexpr int kStride = 1;
  constexpr int kDistinct = kPointCount / kStride;  // 216
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

// Shape 6 — wave-plane ripple, 18 × 12 = 216 distinct (stride 1). Cells:
// u = 3/17 = 0.176, v = 3/11 = 0.273. Higher sample count gives a smooth
// ripple surface that doesn't read as a sparse grid of bumps.
void genWavePlane(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  constexpr int kU = 18, kV = 12;
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

// Shape 7 — Lissajous blob, 216 samples (stride 1). The curve crosses
// itself so adjacent samples cluster naturally — that's the signature
// vectorball "blob" look; doubling density vs the previous 72 fills out
// the shape without erasing the lobed structure.
void genLissajous(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  for (int n = 0; n < kPointCount; ++n) {
    const int vis = n / kStride;
    const float m = static_cast<float>(vis);
    out[n][0] = cosf(0.2094f * m) * sinf(0.3141f * m) * 0.9f * kModelScale;
    out[n][1] = sinf(0.2692f * m) * sinf(0.1884f * m) * 0.9f * kModelScale;
    out[n][2] = sinf(0.3769f * m) * kModelScale;
  }
}

// Shape 8 — octahedron edges. 12 edges × 18 distinct pts/edge = 216
// (stride 1). Edge length = 2·1.3·√2 ≈ 3.68; step = 3.68/17 = 0.216.
// Doubles density vs the previous 12×9 — the wireframe reads as solid
// lit edges instead of dotted segments.
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
  constexpr int kStride = 1;
  constexpr int kEdgePts = 18;
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

// Shape 9 — trefoil knot, 72 distinct samples (stride 3). Curve length
// ≈ 11 model → step 11/72 ≈ 0.153. Doubles density from the previous
// 36 samples so the knot reads as a thick line rather than a dotted
// outline. Lower stride than that and adjacent samples would visibly
// overlap because the curve doubles back through itself.
void genTrefoil(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
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

// Shape 10 — Möbius strip, 12 × 6 = 72 distinct (stride 3). Wide strip
// (half-width 0.7) so v-axis spacing is 1.4/5 = 0.28. Doubled u resolution
// (6 → 12) lets the half-twist read clearly as the strip rotates.
void genMobius(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 3;
  constexpr int kU = 12, kV = 6;
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

// Shape 11 — hyperboloid of one sheet. 24 around × 9 along = 216 distinct
// (stride 1). At the waist (r=0.6) u-step = 2π·0.6/24 = 0.157 — tight, but
// that's the hourglass cinch and reads correctly that way. Flare u-step =
// 2π·1.2/24 = 0.314, v-step = 2.6/8 = 0.325.
void genHyperboloid(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  constexpr int kU = 24, kV = 9;
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

// Shape 12 — hyperbolic-paraboloid saddle. 18 × 12 = 216 distinct
// (stride 1). u-cells = 3/17 = 0.176, v-cells = 3/11 = 0.273. Tripled
// distinct count vs the previous 8×9 — Pringles-chip curve is now smooth
// rather than chunky.
void genSaddle(ShapeBuffer& out, ShapeRng&) {
  constexpr int kStride = 1;
  constexpr int kU = 18, kV = 12;
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

// Shape 13 — pyramid edges. 8 edges × 27 distinct pts/edge = 216
// (stride 1). Base edge length = 2·1.3 = 2.6, slant edges = 2.6·√1.5 ≈
// 3.18. Step on base = 2.6/26 = 0.10, slant = 3.18/26 = 0.12. Tight by
// design — vectorball wireframes look "right" when adjacent balls almost
// touch along the edges.
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
  constexpr int kStride = 1;
  constexpr int kEdgePts = 27;
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

// Shape 14 — icosahedron edge wireframe. 30 edges × 7 distinct pts/edge =
// 210 visible points + 6 vertex caps. Edge step after kIcoR scale = 1.366/6
// = 0.228 — dense enough to read as continuous edges, and the per-vertex
// caps render as the iconic "highlighted ball at every corner" that makes
// the shape look like the SocialMesh reference icosahedron.
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
  constexpr int kEdgePts = 7;
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
  // Remaining 6 indices land on the first 6 vertex positions — the renderer
  // paints them on top of the edge endpoints already there. Visually that's
  // an extra-bright "ball at the vertex", reinforcing the icosahedron's
  // corner glow without changing index count.
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
