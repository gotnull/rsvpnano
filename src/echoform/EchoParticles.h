#pragma once

#include <stddef.h>
#include <stdint.h>

#include "echoform/EchoWave.h"

// Particle pool - C++ port of rusty-nail's crates/fcecho/src/particles.rs,
// Ambient + Splash roles only (no face landmarks or thought orbiters on this
// device). Ambient dust hovers above the wave and drifts with its energy;
// splash droplets arc out of transients and rejoin the surface with an
// impulse - "the loved raindrop bounce".
namespace echoform {

constexpr size_t kParticlePool = 96;

class EchoParticles {
 public:
  enum class Role : uint8_t { Ambient, Splash };

  struct Particle {
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    uint8_t brightness = 0;
    uint16_t lifetime = 0;  // 0 = slot free
    Role role = Role::Ambient;
    bool alive() const { return lifetime > 0; }
  };

  // Spawn one splash droplet at wave column x (surface y in field rows).
  void splash(size_t x, float strength, float waveY);
  // One frame: ambient respawn to target, physics, splash gravity/rejoin.
  void step(EchoWave &wave, float baseY, uint32_t frame, float energy);

  const Particle *pool() const { return pool_; }

 private:
  int allocate();
  uint32_t nextRand();
  float unit();        // [0, 1)
  float signedUnit();  // [-1, 1)
  void spawnAmbient(float baseY);

  Particle pool_[kParticlePool];
  size_t cursor_ = 0;
  uint32_t rng_ = 0x9A971C1Eu;
};

}  // namespace echoform
