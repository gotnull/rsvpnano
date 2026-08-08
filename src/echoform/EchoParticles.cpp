#include "echoform/EchoParticles.h"

#include <math.h>

namespace echoform {

namespace {

constexpr size_t kAmbientTarget = 44;
constexpr size_t kRespawnPerFrame = 2;

float phaseSin(uint16_t phase) {
  return sinf(static_cast<float>(phase) * (6.2831853f / 65536.0f));
}

float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

}  // namespace

uint32_t EchoParticles::nextRand() {
  rng_ ^= rng_ << 13;
  rng_ ^= rng_ >> 17;
  rng_ ^= rng_ << 5;
  return rng_;
}

float EchoParticles::unit() {
  return static_cast<float>(nextRand() >> 8) / 16777216.0f;
}

float EchoParticles::signedUnit() { return unit() * 2.0f - 1.0f; }

int EchoParticles::allocate() {
  for (size_t probe = 0; probe < kParticlePool; ++probe) {
    const size_t i = (cursor_ + probe) % kParticlePool;
    if (!pool_[i].alive()) {
      cursor_ = (i + 1) % kParticlePool;
      return static_cast<int>(i);
    }
  }
  // Steal the lowest-lifetime particle (all roles stealable here).
  int victim = -1;
  uint16_t best = 0xFFFF;
  for (size_t i = 0; i < kParticlePool; ++i) {
    if (pool_[i].lifetime < best) {
      best = pool_[i].lifetime;
      victim = static_cast<int>(i);
    }
  }
  return victim;
}

void EchoParticles::splash(size_t x, float strength, float waveY) {
  const int i = allocate();
  if (i < 0) return;
  const float jitterX = signedUnit() * 0.75f;
  const float jitterV = signedUnit() * 0.5f;
  Particle &p = pool_[i];
  p.x = static_cast<float>(x);
  p.y = waveY;
  p.vx = jitterX;
  p.vy = -fabsf(strength) - fabsf(jitterV);
  p.brightness = 255;
  p.lifetime = static_cast<uint16_t>(40 + (nextRand() % 50));
  p.role = Role::Splash;
}

void EchoParticles::spawnAmbient(float baseY) {
  const int i = allocate();
  if (i < 0) return;
  Particle &p = pool_[i];
  p.x = static_cast<float>(nextRand() % kWavePoints);
  p.y = baseY - unit() * 10.0f - 1.0f;
  p.vx = signedUnit() * 0.125f;
  p.vy = 0.0f;
  p.brightness = 90;
  p.lifetime = static_cast<uint16_t>(300 + (nextRand() % 600));
  p.role = Role::Ambient;
}

void EchoParticles::step(EchoWave &wave, float baseY, uint32_t frame,
                         float energy) {
  size_t ambients = 0;
  for (const Particle &p : pool_) {
    if (p.alive() && p.role == Role::Ambient) ++ambients;
  }
  if (ambients < kAmbientTarget) {
    const size_t want = kAmbientTarget - ambients;
    for (size_t k = 0; k < (want < kRespawnPerFrame ? want : kRespawnPerFrame);
         ++k) {
      spawnAmbient(baseY);
    }
  }

  const EchoWave::Point *pts = wave.points();
  for (size_t i = 0; i < kParticlePool; ++i) {
    Particle &p = pool_[i];
    if (!p.alive()) continue;
    if (p.role == Role::Ambient) {
      --p.lifetime;
      const int col = static_cast<int>(
          clampf(floorf(p.x), 0.0f, static_cast<float>(kWavePoints - 1)));
      const float surface = baseY + pts[col].y;
      const float dy = surface - p.y - 2.0f;
      p.vy += dy * (1.0f / 60.0f);
      const uint16_t phase = static_cast<uint16_t>(
          static_cast<uint16_t>(frame) * 131 +
          static_cast<uint16_t>(i) * 9241);
      const float drift = phaseSin(phase) * (0.02f + energy * 0.06f);
      p.vx = (p.vx + drift) * 0.94f;
      p.vy *= 0.94f;
      float b = 60.0f + energy * 80.0f;
      if (b > 180.0f) b = 180.0f;
      p.brightness = static_cast<uint8_t>(b);
      if (p.lifetime == 0) continue;
    } else {  // Splash
      --p.lifetime;
      p.vy += 0.09f;
      const int col = static_cast<int>(
          clampf(floorf(p.x), 0.0f, static_cast<float>(kWavePoints - 1)));
      const float surface = baseY + pts[col].y;
      if (p.y >= surface && p.vy > 0.0f) {
        wave.impulse(col, p.vy / 3.0f);
        p.lifetime = 0;
        continue;
      }
      const uint16_t capped = p.lifetime < 90 ? p.lifetime : 90;
      uint8_t fade = static_cast<uint8_t>(capped * 255 / 90);
      p.brightness = fade > 120 ? fade : 120;
    }
    p.x = clampf(p.x + p.vx, -2.0f, 129.0f);
    p.y = clampf(p.y + p.vy, -2.0f, 129.0f);
  }
}

}  // namespace echoform
