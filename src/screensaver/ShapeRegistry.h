#pragma once

#include <cstdint>

#include "screensaver/Screensaver.h"

namespace screensaver {

// 216-point shape buffer. Aliases the exact layout used by
// Screensaver::shapes_[i] so generators can write directly into the storage
// the orchestrator already owns.
using ShapeBuffer = float[Screensaver::kPointCount][3];

// Tiny LCG used by the few generators that need randomness (currently the
// random-cloud shape). State is provided by the caller — the orchestrator
// constructs a deterministic instance per begin() so morph targets are
// reproducible across re-entries and don't depend on global rand() state.
struct ShapeRng {
  uint32_t state;
  float frand() {
    state = state * 1664525u + 1013904223u;
    return static_cast<float>(state >> 8) / 16777216.0f;
  }
};

// Descriptor for one shape. Generators must always write exactly
// Screensaver::kPointCount triplets — partial fills will leave stale data
// from the previous shape and corrupt the morph.
struct ShapeGenerator {
  const char* name;
  void (*generate)(ShapeBuffer& out, ShapeRng& rng);
};

// Returns the registry array, ordered to match the integer shape indices
// used by the timeline. shape 0 = cube grid, 1 = sphere, …, 14 = icosahedron.
// Adding a new shape: write a generator, append a descriptor, bump
// Screensaver::kShapeCount.
const ShapeGenerator* shapeRegistry();
constexpr int shapeRegistryCount() { return Screensaver::kShapeCount; }

}  // namespace screensaver
