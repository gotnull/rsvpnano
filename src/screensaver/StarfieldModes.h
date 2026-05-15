#pragma once

#include <cstdint>

#include "screensaver/Screensaver.h"

namespace screensaver {

// PRNG wrapper that lets star-mode strategies share the screensaver's
// xorshift32 state without depending on Screensaver internals. Constructed
// per-call by the orchestrator with a reference to its own prng_ field, so
// determinism (re-entry from the same seed → same sequence) is preserved.
class StarFieldRng {
 public:
  explicit StarFieldRng(uint32_t& state) : state_(state) {}
  uint32_t nextU32();
  float nextFloat();
  // Helper for the very common "pick a tint" pattern.
  uint8_t nextTint() {
    return static_cast<uint8_t>(nextU32() % Screensaver::kStarTintCount);
  }

 private:
  uint32_t& state_;
};

// Strategy descriptor — one per StarMode. init() seeds the stars_ buffer
// for the given mode; update() advances one frame's worth of motion.
struct StarfieldModeDef {
  const char* name;
  void (*init)(Screensaver::Star* stars, int count, StarFieldRng& rng);
  void (*update)(Screensaver::Star* stars, int count, StarFieldRng& rng);
};

// Resolve the active strategy for a mode. The orchestrator does
// `starfieldModeDef(starMode_).init(...)` — no switch statements anywhere
// outside this module.
const StarfieldModeDef& starfieldModeDef(Screensaver::StarMode mode);

}  // namespace screensaver
