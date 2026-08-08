#pragma once

#include <stddef.h>
#include <stdint.h>

// The rusty-nail pixel wave - C++ port of crates/fcecho/src/wave.rs plus the
// audio drive mapping from engine.rs. A 1D shallow-water surface: neighbour
// coupling, restoring spring, damping; audio drives it with impulses
// (transients), excite (sustained energy -> thickness) and turbulence
// (high-band fizz). Y is bounded ±28 in surface units (the original's 128px
// contract); the renderer scales to the panel.
namespace echoform {

constexpr size_t kWavePoints = 128;

class EchoWave {
 public:
  struct Point {
    float y = 0.0f;
    float velocity = 0.0f;
    float energy = 0.0f;
  };

  void reset();
  // Local splash: kick velocity around x with a triangular footprint.
  void impulse(size_t x, float strength);
  // Sustained broadband excitement: thickens the whole ribbon.
  void excite(float level);
  // High-frequency fizz: random velocity jitter across eight columns.
  void turbulence(float level);
  // One integration step (IDLE material, idle harmonics at breathAmp 0..1).
  void step(float breathAmp);

  const Point *points() const { return points_; }

 private:
  uint32_t nextRand();

  Point points_[kWavePoints];
  uint16_t phase_[3] = {0, 17000, 41000};
  uint32_t rng_ = 0x57A7E001u;
};

}  // namespace echoform
