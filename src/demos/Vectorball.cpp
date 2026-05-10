#include "demos/Vectorball.h"

#include <math.h>

namespace {

// 16-color palette tuned for shiny vectorballs against black:
// punchy primaries + a smooth grey ramp for highlights.
constexpr uint16_t kPalette[Vectorball::kPaletteSize] = {
    0xF800, 0xFD00, 0xFFE0, 0x07E0,  // red, orange, yellow, green
    0x07FF, 0x001F, 0x781F, 0xF81F,  // cyan, blue, purple, magenta
    0xFC18, 0xFEC0, 0x9FE0, 0x07F8,  // pinks, lime, teal
    0xC618, 0xE71C, 0xFFFF, 0xAD55,  // greys + white
};

}  // namespace

const uint16_t *Vectorball::palette() { return kPalette; }

void Vectorball::initFibonacciSphere() {
  // Spherical Fibonacci spiral — yields near-uniform point spacing on the
  // unit sphere, no clustering at poles. Gold ratio drives the angular step.
  const float phi = 3.1415926535f * (3.0f - sqrtf(5.0f));
  for (int i = 0; i < kBallCount; ++i) {
    const float y = 1.0f - (static_cast<float>(i) / (kBallCount - 1)) * 2.0f;  // 1 → -1
    const float r = sqrtf(1.0f - y * y);
    const float theta = phi * static_cast<float>(i);
    balls_[i].ux = r * cosf(theta);
    balls_[i].uy = y;
    balls_[i].uz = r * sinf(theta);
    // Color index seeded from spatial position so neighbouring balls vary
    // slightly — gives the cluster a "sparkle" look rather than monotone.
    int c = static_cast<int>((balls_[i].ux + 1.0f) * 4.0f +
                             (balls_[i].uz + 1.0f) * 2.0f) % kPaletteSize;
    if (c < 0) c += kPaletteSize;
    balls_[i].colorIndex = c;
    drawOrder_[i] = static_cast<uint16_t>(i);
  }
}

void Vectorball::begin(uint32_t seedMs) {
  initFibonacciSphere();
  // Phase the rotation timer with seed so each session opens at a different
  // angle.
  t_ = (seedMs ? static_cast<float>(seedMs & 0xFFFF) / 1000.0f : 0.0f);
  lastMs_ = 0;
}

void Vectorball::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 60.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;
  t_ += dt;

  // Multi-axis rotation: pitch around X, yaw around Y, slow roll around Z.
  // Independent angular speeds keep the motion non-periodic.
  const float ca = cosf(t_ * 0.6f), sa = sinf(t_ * 0.6f);  // around Y
  const float cb = cosf(t_ * 0.4f), sb = sinf(t_ * 0.4f);  // around X
  const float cc = cosf(t_ * 0.27f), sc = sinf(t_ * 0.27f);  // around Z

  for (int i = 0; i < kBallCount; ++i) {
    Ball &b = balls_[i];
    // Rotate Y (yaw): x' = x*ca - z*sa, z' = x*sa + z*ca
    const float x1 = b.ux * ca - b.uz * sa;
    const float z1 = b.ux * sa + b.uz * ca;
    // Rotate X (pitch): y' = y*cb - z*sb, z'' = y*sb + z*cb
    const float y2 = b.uy * cb - z1 * sb;
    const float z2 = b.uy * sb + z1 * cb;
    // Rotate Z (roll): x'' = x*cc - y*sc, y'' = x*sc + y*cc
    const float x3 = x1 * cc - y2 * sc;
    const float y3 = x1 * sc + y2 * cc;
    b.cx = x3;
    b.cy = y3;
    // Push the sphere in front of the camera and clamp so balls behind us
    // (cz <= 0) clip cleanly in the renderer.
    b.cz = z2 + 3.5f;
    if (b.cz < 0.1f) b.cz = 0.1f;
  }
}

void Vectorball::sortBalls() {
  // Insertion sort over indices — n=400, mostly already-sorted between
  // frames so worst case per frame is small. Sorting indices (2 bytes)
  // not full Ball structs (24 bytes) keeps the swap cost down.
  for (int i = 1; i < kBallCount; ++i) {
    const uint16_t key = drawOrder_[i];
    const float keyCz = balls_[key].cz;
    int j = i - 1;
    while (j >= 0 && balls_[drawOrder_[j]].cz < keyCz) {
      drawOrder_[j + 1] = drawOrder_[j];
      --j;
    }
    drawOrder_[j + 1] = key;
  }
}
