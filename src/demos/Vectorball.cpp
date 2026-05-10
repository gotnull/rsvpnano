#include "demos/Vectorball.h"
#include "demos/VectorballData.h"

#include <cstring>
#include <esp_heap_caps.h>
#include <math.h>

namespace {

// Aliases that map the data-table constants into the names the engine
// already uses internally. Centralised here so a future data refresh only
// needs to update VectorballData.h.
constexpr int kRotSegments = kVectorballRotSegments;
const uint8_t *kRotFrames = kVectorballRotFrames;
const float *kRotDegX = kVectorballRotDegX;
const float *kRotDegY = kVectorballRotDegY;
const float *kRotDegZ = kVectorballRotDegZ;
constexpr int kThresholdCount = kVectorballShapeThresholds;
const int *kShapeThresholds = kVectorballThresholds;
constexpr int kShapeCycleFrames = 3650;

constexpr float kDegToRad = 0.0174533f;

// Bucket-sort layout — bin = z + 800 - kCameraZ. With kCameraZ = -1100 that
// places typical Z in [200..1400], well inside the 1600-slot table.
constexpr int kBucketSize = 1600;
constexpr int kBucketBias = 800;

// Linear-congruential PRNG for shape 9 (the random cloud). Local so we
// don't perturb global rand() state between sessions.
struct Lcg {
  uint32_t s;
  uint32_t next() {
    s = s * 1664525u + 1013904223u;
    return s;
  }
  float frand() { return static_cast<float>(next() >> 8) / 16777216.0f; }
};

}  // namespace

void Vectorball::generateShapes() {
  // Per a.g() — generates 10 parallel shape sets. The original 20×20 grid
  // generators (shapes 0..2) use n = i + j*20; spirals (3..8) use n in
  // [0, kBallCount). Random cloud (9) is uniform in ±300.
  // All values clamped to int16 range — original is int but ±400-ish.
  constexpr int kAy = 20;  // sqrt(400)
  for (int j = 0; j < kAy; ++j) {
    for (int i = 0; i < kAy; ++i) {
      const int n = i + j * kAy;
      // Shape 0 — wave plane (cos·sin grid)
      shapes_[0][n][0] = static_cast<int16_t>(400 * i / 20 * 2.2f - 440);
      shapes_[0][n][1] = static_cast<int16_t>(400 * j / 20 * 2.2f - 440);
      shapes_[0][n][2] = static_cast<int16_t>(
          cosf(6.283f / 20.0f * i * 2.0f) *
          sinf(3.14159f / 20.0f * j * 2.0f) * 400.0f / 5.0f);
      // Shape 1 — quartic-saddle plane
      shapes_[1][n][0] = static_cast<int16_t>(400 * i / 20 * 2 - 400);
      shapes_[1][n][1] = static_cast<int16_t>(400 * j / 20 * 2 - 400);
      const long qsi = (i - 10) * (i - 10);
      const long qsj = (j - 10) * (j - 10);
      shapes_[1][n][2] = static_cast<int16_t>(
          static_cast<long>(qsi * qsj) * 400L / (20L * 20L * 20L * 20L / 16L) - 200L);
      // Shape 2 — saddle + cosine bump
      shapes_[2][n][0] = static_cast<int16_t>(400 * i / 20 * 2 - 400);
      shapes_[2][n][1] = static_cast<int16_t>(400 * j / 20 * 2 - 400);
      shapes_[2][n][2] = static_cast<int16_t>(
          -static_cast<long>(qsi * qsj) * 400L / (20L * 20L * 20L * 20L / 16L) +
          cosf(1.2566f * i * j) * (20 * 20 / 2.0f) + 160.0f);
    }
  }
  for (int n = 0; n < kBallCount; ++n) {
    // Shape 3 — Lissajous-ish blob (note original swizzles cc/ce/cd assignment;
    // PORT_NOTES has them as cc/ce/cd which is X/Y/Z = our [0]/[1]/[2]).
    shapes_[3][n][0] = static_cast<int16_t>(
        cosf(0.069811f * n) * sinf(0.10472f * n) * 400.0f * 0.9f);
    shapes_[3][n][1] = static_cast<int16_t>(
        sinf(0.089757f * n) * sinf(0.062832f * n) * 400.0f * 0.9f);
    shapes_[3][n][2] = static_cast<int16_t>(sinf(0.125664f * n) * 400.0f);
    // Shape 4 — torus/spiral ring (the iconic vectorball look)
    shapes_[4][n][0] = static_cast<int16_t>(
        cosf(0.6283f * n) * sinf(6.28318f / 400.0f * n) * 400.0f);
    shapes_[4][n][1] = static_cast<int16_t>(800 / 400 * n - 400);
    shapes_[4][n][2] = static_cast<int16_t>(
        sinf(0.6283f * n) * sinf(6.28318f / 400.0f * n) * 400.0f);
    // Shape 5 — half-frequency variant
    shapes_[5][n][0] = static_cast<int16_t>(
        sinf(0.6283f * n) * sinf(6.28318f / 400.0f * n / 2.0f) * 400.0f);
    shapes_[5][n][1] = static_cast<int16_t>(800 / 400 * n * 1.5f - 600);
    shapes_[5][n][2] = static_cast<int16_t>(
        cosf(0.6283f * n) * sinf(6.28318f / 400.0f * n / 2.0f) * 400.0f);
    // Shape 6 — same shape, slightly different spin
    shapes_[6][n][0] = static_cast<int16_t>(
        sinf(0.61598f * n) * sinf(6.28318f / 400.0f * n / 2.0f) * 400.0f);
    shapes_[6][n][1] = static_cast<int16_t>(800 / 400 * n * 1.5f - 600);
    shapes_[6][n][2] = static_cast<int16_t>(
        cosf(0.61598f * n) * sinf(6.28318f / 400.0f * n / 2.0f) * 400.0f);
    // Shape 7 — 1.5× freq spiral
    shapes_[7][n][0] = static_cast<int16_t>(
        cosf(0.6283f * n) * sinf(6.28318f / 400.0f * n * 1.5f) * 400.0f);
    shapes_[7][n][1] = static_cast<int16_t>(800 / 400 * n - 400);
    shapes_[7][n][2] = static_cast<int16_t>(
        sinf(0.6283f * n) * sinf(6.28318f / 400.0f * n * 1.5f) * 400.0f);
    // Shape 8 — tight 10-loop spiral (X linear)
    shapes_[8][n][0] = static_cast<int16_t>(400 * n / 400 * 2 - 400);
    shapes_[8][n][1] = static_cast<int16_t>(sinf(6.283f / 400.0f * n * 10.0f) * 400.0f);
    shapes_[8][n][2] = static_cast<int16_t>(cosf(6.283f / 400.0f * n * 10.0f) * 400.0f);
  }
  // Shape 9 — pure random cloud, ±300. Seed deterministically so each
  // begin() reproduces exactly.
  Lcg lcg{0xC0DECAFEu};
  for (int n = 0; n < kBallCount; ++n) {
    shapes_[9][n][0] = static_cast<int16_t>(600.0f * lcg.frand() - 300.0f);
    shapes_[9][n][1] = static_cast<int16_t>(600.0f * lcg.frand() - 300.0f);
    shapes_[9][n][2] = static_cast<int16_t>(600.0f * lcg.frand() - 300.0f);
  }
}

Vectorball::~Vectorball() {
  if (framebuffer_ != nullptr) {
    heap_caps_free(framebuffer_);
    framebuffer_ = nullptr;
  }
}

void Vectorball::begin(uint32_t seedMs) {
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
  generateShapes();
  for (int i = 0; i < kBallCount; ++i) drawOrder_[i] = static_cast<uint16_t>(i);
  a6_ = a4_ = a9_ = 0.0f;
  yBias_ = kInitialYBias;
  frameCounter_ = 0;
  segIdx_ = 0;
  segFrame_ = 0;
}

void Vectorball::paintFramebuffer() {
  if (framebuffer_ == nullptr) return;
  // Per a.t() — clear the framebuffer (the original used a precomputed
  // background plate for 4/5 of the screen; our spec writes solid 0).
  std::memset(framebuffer_, 0, kFramebufferBytes);

  // Stamp every ball in painter order. Sprites are 15×15 with embedded
  // zeros for the transparent corners — only non-zero pixels get written.
  for (int i = 0; i < kBallCount; ++i) {
    const Ball &b = balls_[drawOrder_[i]];
    if (b.brightness < 1 || b.brightness > 6) continue;
    if (b.sx < 0 || b.sy < 0) continue;
    const auto &sprite = kVectorballSprite[b.brightness - 1];
    for (int row = 0; row < kSpriteSize; ++row) {
      const int dstY = b.sy + row;
      if (dstY < 0 || dstY >= kFrameHeight) continue;
      uint8_t *dstRow = framebuffer_ + dstY * kFrameWidth + b.sx;
      const uint8_t *src = sprite[row];
      // Manual unroll-friendly loop; compiler will SIMD-style this.
      for (int col = 0; col < kSpriteSize; ++col) {
        const uint8_t v = src[col];
        if (v != 0) dstRow[col] = v;
      }
    }
  }
}

void Vectorball::updateAngles() {
  // Apply current segment's per-frame deltas.
  a6_ += kDegToRad * kRotDegX[segIdx_];
  a4_ += kDegToRad * kRotDegY[segIdx_];
  a9_ += kDegToRad * kRotDegZ[segIdx_];
  if (++segFrame_ >= kRotFrames[segIdx_]) {
    segFrame_ = 0;
    segIdx_ = (segIdx_ + 1) % kRotSegments;
  }
}

void Vectorball::buildRotationMatrix(float &ar, float &aq, float &ap,
                                     float &ao, float &an, float &am,
                                     float &al, float &ak, float &aj) {
  // Tait-Bryan ZYX from a.l() — note negations on the off-diagonal cells.
  const float f1 = cosf(a6_), f2 = sinf(a6_);
  const float f3 = cosf(a4_), f4 = sinf(a4_);
  const float f5 = cosf(a9_), f6 = sinf(a9_);
  ar = f1 * f3;
  aq = -(f1 * f4 * f6 + f2 * f5);
  ap = -(f1 * f4 * f5 - f2 * f6);
  ao = f2 * f3;
  an = -(f2 * f4 * f6 - f1 * f5);
  am = -(f2 * f4 * f5 + f1 * f6);
  al = f4;
  ak = f3 * f6;
  aj = f3 * f5;
}

void Vectorball::shapeAt(int n, float &x, float &y, float &z) const {
  // Where in the timeline are we? Map frame counter into [0, kShapeCycleFrames).
  const int cycleFrame = static_cast<int>(frameCounter_ % kShapeCycleFrames);
  // Walk the threshold table to find which segment (hold or morph) we're in.
  // Segments alternate hold/morph: holds are even-index, morphs are odd-index.
  // bf[2k]   = end of "hold shape k"          (hold shape k for [bf[2k-1], bf[2k]) )
  // bf[2k+1] = end of "morph from k to k+1"   (morph between bf[2k] and bf[2k+1])
  // bf[0] is special — "hold shape 0 from frame 0".
  if (cycleFrame < kShapeThresholds[0]) {
    // Initial hold of shape 0 (1000 frames).
    x = shapes_[0][n][0];
    y = shapes_[0][n][1];
    z = shapes_[0][n][2];
    return;
  }
  // After bf[0]: pairs (bf[2k+1], bf[2k+2]) bracket morph from shape k to k+1
  // and the subsequent hold of shape k+1.
  for (int k = 0; k < (kThresholdCount - 1) / 2; ++k) {
    const int morphStart = kShapeThresholds[2 * k];
    const int morphEnd = kShapeThresholds[2 * k + 1];
    const int holdEnd = kShapeThresholds[2 * k + 2];
    if (cycleFrame < morphEnd) {
      // Morphing from shape k to shape k+1.
      const float t = static_cast<float>(cycleFrame - morphStart) /
                      static_cast<float>(morphEnd - morphStart);
      const int kNext = (k + 1) % kShapeCount;
      x = shapes_[k][n][0] * (1.0f - t) + shapes_[kNext][n][0] * t;
      y = shapes_[k][n][1] * (1.0f - t) + shapes_[kNext][n][1] * t;
      z = shapes_[k][n][2] * (1.0f - t) + shapes_[kNext][n][2] * t;
      return;
    }
    if (cycleFrame < holdEnd) {
      // Holding shape k+1.
      const int kHold = (k + 1) % kShapeCount;
      x = shapes_[kHold][n][0];
      y = shapes_[kHold][n][1];
      z = shapes_[kHold][n][2];
      return;
    }
  }
  // Past the last threshold — hold the last shape until cycle wraps.
  x = shapes_[kShapeCount - 1][n][0];
  y = shapes_[kShapeCount - 1][n][1];
  z = shapes_[kShapeCount - 1][n][2];
}

void Vectorball::tick(uint32_t nowMs) {
  (void)nowMs;
  updateAngles();
  // Y bias: dive-in for first 160 frames, climb-out for last 160 frames of
  // the cycle. Per the original a.k().
  if (frameCounter_ < 160) yBias_ -= 6.0f;
  if (frameCounter_ > kShapeCycleFrames - 160) yBias_ += 6.0f;

  float ar, aq, ap, ao, an, am, al, ak, aj;
  buildRotationMatrix(ar, aq, ap, ao, an, am, al, ak, aj);

  // Rotate every ball, then project + bucket-classify.
  // Depth-cue: split [kCameraZ-600 .. kCameraZ+600] into 6 brightness buckets.
  constexpr float kNear = kCameraZ - 600.0f;
  constexpr float kStep = 200.0f;  // (kCameraZ+600 - (kCameraZ-600))/6

  for (int n = 0; n < kBallCount; ++n) {
    float mx, my, mz;
    shapeAt(n, mx, my, mz);
    const float xr = ar * mx + aq * my + ap * mz;
    const float yr = ao * mx + an * my + am * mz + yBias_;
    const float zr = al * mx + ak * my + aj * mz + kCameraZ;
    rotZ_[n] = static_cast<int16_t>(zr);
    if (zr >= 0.0f) {
      // Behind camera — sentinel sx=-1 means "skip" in the renderer.
      balls_[n].sx = -1;
      balls_[n].sy = -1;
      balls_[n].brightness = 0;
      continue;
    }
    const float f = kFocal / (kFocal - zr);
    int sx = static_cast<int>(xr * f + (kFrameWidth / 2.0f) - 7.0f);
    int sy = static_cast<int>(yr * f + (kFrameHeight / 2.0f) - 7.0f);
    if (sx < 0) sx = 0;
    if (sx > kFrameWidth - kSpriteSize - 1) sx = kFrameWidth - kSpriteSize - 1;
    if (sy < 0) sy = 0;
    if (sy > kFrameHeight - kSpriteSize - 1) sy = kFrameHeight - kSpriteSize - 1;
    balls_[n].sx = static_cast<int16_t>(sx);
    balls_[n].sy = static_cast<int16_t>(sy);
    int bucket = 6;  // nearest
    if (zr < kNear + kStep)        bucket = 1;
    else if (zr < kNear + 2 * kStep) bucket = 2;
    else if (zr < kNear + 3 * kStep) bucket = 3;
    else if (zr < kNear + 4 * kStep) bucket = 4;
    else if (zr < kNear + 5 * kStep) bucket = 5;
    balls_[n].brightness = static_cast<int8_t>(bucket);
  }
  ++frameCounter_;
}

void Vectorball::sortBalls() {
  // O(N) bucket sort with linear collision probing (a.r()). Bins are 1 Z
  // unit each; with kBucketBias=800 and Z roughly in [-1700..-500], bins
  // fall in [200..1400]. Bins fill back-to-front so iterating [0..N) of
  // the compacted list paints farthest first.
  std::memset(buckets_, 0, sizeof(buckets_));
  for (int n = 0; n < kBallCount; ++n) {
    int bin = static_cast<int>(rotZ_[n]) - static_cast<int>(kCameraZ) + kBucketBias;
    if (bin < 0) bin = 0;
    if (bin >= kBucketSize) bin = kBucketSize - 1;
    while (buckets_[bin] != 0) ++bin;  // linear-probe past collisions
    if (bin >= kBucketSize) bin = kBucketSize - 1;
    buckets_[bin] = static_cast<uint16_t>(n + 1);  // store ball index +1
  }
  int out = 0;
  for (int i = 0; i < kBucketSize && out < kBallCount; ++i) {
    if (buckets_[i] != 0) drawOrder_[out++] = static_cast<uint16_t>(buckets_[i] - 1);
  }
  // Pad any unwritten slots with the highest-index ball so the renderer
  // doesn't stutter on a partial list (shouldn't happen, but be defensive).
  while (out < kBallCount) drawOrder_[out++] = static_cast<uint16_t>(kBallCount - 1);
}
