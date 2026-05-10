#include "demos/Starfield.h"

namespace {

constexpr float kLayerSpeed[Starfield::kLayerCount] = {40.0f, 90.0f, 180.0f};   // px/sec
constexpr uint8_t kLayerBaseBright[Starfield::kLayerCount] = {90, 170, 255};

}  // namespace

uint32_t Starfield::nextRandU32() {
  uint32_t x = prng_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  prng_ = x;
  return x;
}

void Starfield::begin(uint32_t seedMs) {
  prng_ = (seedMs != 0) ? seedMs : 0x12345678u;
  int idx = 0;
  for (int layer = 0; layer < kLayerCount; ++layer) {
    for (int i = 0; i < kStarsPerLayer; ++i) {
      Star &s = stars_[idx++];
      s.x = static_cast<float>(nextRandU32() % 640u);
      s.y = static_cast<int16_t>(nextRandU32() % 172u);
      s.layer = static_cast<uint8_t>(layer);
      // Tiny per-star brightness jitter so stars in the same layer don't
      // look identical.
      const int jitter = static_cast<int>(nextRandU32() & 0x1F) - 16;
      int b = static_cast<int>(kLayerBaseBright[layer]) + jitter;
      if (b < 32) b = 32;
      if (b > 255) b = 255;
      s.brightness = static_cast<uint8_t>(b);
    }
  }
  lastMs_ = 0;
}

void Starfield::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 60.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;
  for (int i = 0; i < kStarCount; ++i) {
    Star &s = stars_[i];
    s.x -= kLayerSpeed[s.layer] * dt;
    if (s.x < 0.0f) {
      s.x += 640.0f;
      // Re-randomise Y when wrapping so the field doesn't just repeat.
      s.y = static_cast<int16_t>(nextRandU32() % 172u);
    }
  }
}
