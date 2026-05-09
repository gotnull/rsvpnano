#include "screensaver/Screensaver.h"

#include <math.h>

namespace {

constexpr float kSphereRadius = 1.5f;

float randFloat() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

// Pico-8 palette in RGB565 (black removed — that's the background).
constexpr uint16_t kPaletteColors[Screensaver::kPaletteSize] = {
    0x18C3, 0x7C09, 0x016A, 0xA9A4, 0x52EA, 0xC618, 0xFFFB, 0xF810,
    0xFD60, 0xFFE0, 0x07E6, 0x2B7F, 0x8418, 0xFE36, 0xFF55,
};

float clerp(float a, float b, float p) { return a + p * (b - a); }

}  // namespace

const uint16_t *Screensaver::palette() { return kPaletteColors; }

void Screensaver::begin(uint32_t seed) {
  if (seed != 0) srand(seed);
  initPoints();
  initStars();
  t_ = 0.0f;
  t_mod_ = 0.0f;
  nextChange_ = 0.0f;
  currentSpeed_ = 1.0f;
  morphProgress_ = 0.0f;
  morphing_ = false;
}

void Screensaver::initPoints() {
  int idx = 0;
  const float step = 2.0f / (kPointsPerAxis - 1);
  for (int i = 0; i < kPointsPerAxis; ++i) {
    for (int j = 0; j < kPointsPerAxis; ++j) {
      for (int k = 0; k < kPointsPerAxis; ++k) {
        if (idx >= kPointCount) break;
        Point &p = points_[idx];
        p.cubeX = -1.0f + i * step;
        p.cubeY = -1.0f + j * step;
        p.cubeZ = -1.0f + k * step;

        const float u = randFloat();
        const float v = randFloat();
        const float theta = 2.0f * M_PI * u;
        const float phi = acosf(2.0f * v - 1.0f);
        p.sphereX = kSphereRadius * sinf(phi) * cosf(theta);
        p.sphereY = kSphereRadius * sinf(phi) * sinf(theta);
        p.sphereZ = kSphereRadius * cosf(phi);

        p.x = p.cubeX;
        p.y = p.cubeY;
        p.z = p.cubeZ;
        p.cx = p.cy = p.cz = 0.0f;
        p.colorIndex = 8 + (static_cast<int>(p.cubeX * 2 + p.cubeY * 3) % 8);
        if (p.colorIndex < 0) p.colorIndex += kPaletteSize;
        ++idx;
      }
    }
  }
}

void Screensaver::initStars() {
  for (int i = 0; i < kStarCount; ++i) {
    stars_[i].x = randFloat() * 2.0f - 1.0f;
    stars_[i].y = randFloat() * 2.0f - 1.0f;
    stars_[i].z = randFloat();
  }
}

void Screensaver::updateStars() {
  for (int i = 0; i < kStarCount; ++i) {
    stars_[i].z -= 0.02f;
    if (stars_[i].z <= 0.0f) {
      stars_[i].x = randFloat() * 2.0f - 1.0f;
      stars_[i].y = randFloat() * 2.0f - 1.0f;
      stars_[i].z = 1.0f;
    }
  }
}

void Screensaver::updateMorph() {
  if (morphing_) {
    morphProgress_ += 0.02f;
    if (morphProgress_ >= 1.0f) {
      morphProgress_ = 1.0f;
      morphing_ = false;
    }
    for (int i = 0; i < kPointCount; ++i) {
      Point &p = points_[i];
      p.x = clerp(p.cubeX, p.sphereX, morphProgress_);
      p.y = clerp(p.cubeY, p.sphereY, morphProgress_);
      p.z = clerp(p.cubeZ, p.sphereZ, morphProgress_);
    }
  } else if (rand() % 1000 < 10) {
    morphing_ = true;
    morphProgress_ = 0.0f;
    // Swap cube and sphere targets so the next morph reverses direction.
    for (int i = 0; i < kPointCount; ++i) {
      Point &p = points_[i];
      const float tx = p.cubeX, ty = p.cubeY, tz = p.cubeZ;
      p.cubeX = p.sphereX;
      p.cubeY = p.sphereY;
      p.cubeZ = p.sphereZ;
      p.sphereX = tx;
      p.sphereY = ty;
      p.sphereZ = tz;
    }
  }
}

void Screensaver::tick() {
  // Variable-speed pacing: every few frames, randomly switch between slow and
  // fast playback. Same idea as the original.
  if (t_mod_ >= nextChange_) {
    nextChange_ = t_mod_ + (200 + (rand() % 800)) / 100.0f;
    currentSpeed_ = (rand() % 2 == 0) ? 0.5f : 5.0f;
  }
  constexpr float kBaseSpeed = 0.1f;
  t_mod_ += kBaseSpeed;
  t_ += kBaseSpeed * currentSpeed_;

  updateStars();
  updateMorph();

  // Project each point through camera rotation to (cx, cy, cz).
  const float cos_a = cosf(t_ / 4.0f);
  const float sin_a = sinf(t_ / 4.0f);
  const float cos_b = cosf(t_ / 6.0f);
  const float sin_b = sinf(t_ / 6.0f);
  const float cos_c = cosf(t_ / 5.0f);

  for (int i = 0; i < kPointCount; ++i) {
    Point &p = points_[i];
    const float rx = cos_a * p.x - sin_a * p.z;
    const float rz = sin_a * p.x + cos_a * p.z;
    p.cx = rx;
    p.cz = rz;
    const float ry2 = cos_b * p.y - sin_b * p.cz;
    const float rz2 = sin_b * p.y + cos_b * p.cz;
    p.cy = ry2;
    p.cz = rz2 + 2.0f + cos_c;
    if (p.cz < 0.1f) p.cz = 0.1f;
  }
}

void Screensaver::sortPoints() {
  // Insertion sort (n=216 — small, mostly already-sorted between frames).
  for (int i = 1; i < kPointCount; ++i) {
    Point key = points_[i];
    int j = i - 1;
    while (j >= 0 && points_[j].cz < key.cz) {
      points_[j + 1] = points_[j];
      --j;
    }
    points_[j + 1] = key;
  }
}
