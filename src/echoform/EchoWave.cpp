#include "echoform/EchoWave.h"

#include <math.h>
#include <string.h>

namespace echoform {

namespace {

// IDLE material from wave.rs WaveParams (no face on this device, so the
// COHERENT blend never engages).
constexpr float kCoupling = 0.30f;
constexpr float kStiffness = 0.02f;
constexpr float kDamping = 0.94f;
constexpr float kYBound = 28.0f;

float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

// sin over a u16 phase (the original's table-free equivalent).
float phaseSin(uint16_t phase) {
  return sinf(static_cast<float>(phase) * (6.2831853f / 65536.0f));
}

}  // namespace

void EchoWave::reset() {
  memset(points_, 0, sizeof(points_));
  phase_[0] = 0;
  phase_[1] = 17000;
  phase_[2] = 41000;
}

uint32_t EchoWave::nextRand() {
  // xorshift32, matching the original's discipline (own PRNG, no rand()).
  rng_ ^= rng_ << 13;
  rng_ ^= rng_ >> 17;
  rng_ ^= rng_ << 5;
  return rng_;
}

void EchoWave::impulse(size_t x, float strength) {
  constexpr int kSpread = 3;
  for (int d = -kSpread; d <= kSpread; ++d) {
    const int i = static_cast<int>(x) + d;
    if (i < 0 || i >= static_cast<int>(kWavePoints)) continue;
    const float w =
        static_cast<float>(kSpread + 1 - (d < 0 ? -d : d)) / (kSpread + 1);
    Point &p = points_[i];
    p.velocity += strength * w;
    p.energy = clampf(p.energy + fabsf(strength) * w, 0.0f, 4.0f);
  }
}

void EchoWave::excite(float level) {
  const float gain = level * 0.12f;
  for (Point &p : points_) {
    p.energy = clampf(p.energy + gain, 0.0f, 4.0f);
  }
}

void EchoWave::turbulence(float level) {
  if (level <= 0.0f) return;
  for (int k = 0; k < 8; ++k) {
    const size_t i = nextRand() % kWavePoints;
    // signed unit in [-1, 1)
    const float unit =
        static_cast<float>(static_cast<int32_t>(nextRand())) / 2147483648.0f;
    points_[i].velocity += unit * level * 0.35f;
  }
}

void EchoWave::step(float breathAmp) {
  phase_[0] = static_cast<uint16_t>(phase_[0] + 210);
  phase_[1] = static_cast<uint16_t>(phase_[1] + 347);
  phase_[2] = static_cast<uint16_t>(phase_[2] + 89);

  float prevY[kWavePoints];
  for (size_t i = 0; i < kWavePoints; ++i) {
    prevY[i] = points_[i].y;
  }
  for (size_t i = 0; i < kWavePoints; ++i) {
    const float left = prevY[i == 0 ? 0 : i - 1];
    const float right = prevY[i == kWavePoints - 1 ? i : i + 1];
    Point &p = points_[i];
    const float laplacian = left + right - 2.0f * p.y;
    const float accel = laplacian * kCoupling - p.y * kStiffness;
    p.velocity = (p.velocity + accel) * kDamping;
    p.y = clampf(p.y + p.velocity, -kYBound, kYBound);
    p.energy *= 0.92f;
  }

  // Idle harmonics as velocity nudges - the watery breathing.
  if (breathAmp > 0.0f) {
    for (size_t i = 0; i < kWavePoints; i += 2) {
      const uint16_t x = static_cast<uint16_t>(i * 512);
      const float h =
          phaseSin(static_cast<uint16_t>(phase_[0] + x)) * 0.30f +
          phaseSin(static_cast<uint16_t>(phase_[1] + x * 3)) * 0.16f +
          phaseSin(static_cast<uint16_t>(phase_[2] + x * 7)) * 0.08f;
      const float pull = h * breathAmp * 0.06f;
      points_[i].velocity += pull;
      if (i + 1 < kWavePoints) {
        points_[i + 1].velocity += pull;
      }
    }
  }
}

}  // namespace echoform
