#pragma once

#include <cstdint>

#include "screensaver/Screensaver.h"
#include "screensaver/ShapeRegistry.h"

namespace screensaver {

// How morph progress is shaped on its way from 0 → 1. Linear preserves the
// behaviour we shipped pre-refactor; SmoothStep is wired but currently unused
// (no keyframes pick it) so the visual is byte-identical to the previous build.
enum class EaseCurve : uint8_t {
  Linear,
  SmoothStep,
};

// How the from/to positions for one point are blended once we have an eased t.
// Linear is the only style today; the rest are placeholders so future styles
// (explode, spiral, axis-sweep, noise-dissolve, collapse-to-centre) can be
// added without touching the timeline or shape generators.
enum class MorphStyle : uint8_t {
  Linear,
  // Future:
  // ExplodeThenSettle,
  // Spiral,
  // AxisSweep,
  // NoiseDissolve,
  // CollapseToCenter,
};

// One slot in the shape timeline. Each keyframe says "hold this shape for
// holdFrames, then morph to the next keyframe's shape over morphFrames".
struct ShapeKeyframe {
  uint8_t shapeIndex;
  uint16_t holdFrames;
  uint16_t morphFrames;
  EaseCurve ease;
  MorphStyle morphStyle;
};

// Read-only view of the timeline. Returned by reference so the table stays
// in flash. Order defines the visit sequence — adjacent entries determine the
// (from, to) pair for morphing.
const ShapeKeyframe* timelineKeyframes();
int timelineKeyframeCount();

// Total frames the full cycle covers — sum of (hold + morph) for every
// keyframe. Used to wrap the global frame counter onto the timeline.
uint32_t timelineCycleFrames();

// Resolved state at a given global frame, computed once per tick.
struct TimelineSample {
  uint8_t shapeIdx;       // keyframe currently visible (hold or morph FROM)
  uint8_t nextShapeIdx;   // keyframe we're morphing into (== shapeIdx when not morphing)
  bool inMorph;           // true if we're past holdFrames within this keyframe
  float rawProgress;      // 0..1 across the morph window (0 outside morph)
  float eased;            // rawProgress after EaseCurve has been applied
  MorphStyle morphStyle;  // style to feed to applyMorphStyle()
};

TimelineSample sampleTimeline(uint32_t globalFrame);

// Apply the chosen morph style to interpolate one point from `from` to `to`
// using eased progress `t`. pointIndex + seed are passed so future styles can
// vary per-point (e.g. explode adds a per-point offset) without changing the
// caller. For MorphStyle::Linear they're ignored.
void applyMorphStyle(MorphStyle style,
                     const float from[3],
                     const float to[3],
                     float t,
                     int pointIndex,
                     uint32_t seed,
                     float out[3]);

// Convenience: resolve the model-space position of point i at globalFrame,
// pulling from a 2D shape array. Centralises the "hold vs morph + interpolate"
// branch that lived inside Screensaver::shapeAt().
void resolvePoint(uint32_t globalFrame,
                  int pointIndex,
                  const float shapes[Screensaver::kShapeCount]
                                    [Screensaver::kPointCount][3],
                  uint32_t morphSeed,
                  float out[3]);

}  // namespace screensaver
