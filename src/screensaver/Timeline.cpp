#include "screensaver/Timeline.h"

namespace screensaver {
namespace {

// Default timing — preserves the previous monolithic implementation exactly
// (kHoldFrames=360 + kMorphFrames=120). Each keyframe references its shape by
// integer index into the shape registry. To re-order, retime, or insert a
// custom transition for one shape, edit a single row here.
constexpr uint16_t kDefaultHold = 360;
constexpr uint16_t kDefaultMorph = 120;

#define KF(ix) {ix, kDefaultHold, kDefaultMorph, EaseCurve::Linear, MorphStyle::Linear}

constexpr ShapeKeyframe kTimeline[] = {
    KF(0),   // cube
    KF(1),   // sphere
    KF(2),   // torus
    KF(3),   // helix
    KF(4),   // double helix
    KF(5),   // random cloud
    KF(6),   // wave plane
    KF(7),   // Lissajous
    KF(8),   // octahedron edges
    KF(9),   // trefoil
    KF(10),  // Möbius
    KF(11),  // hyperboloid
    KF(12),  // saddle
    KF(13),  // pyramid edges
    KF(14),  // icosahedron edges
};

#undef KF

static_assert(sizeof(kTimeline) / sizeof(kTimeline[0]) == Screensaver::kShapeCount,
              "timeline length must currently match the shape registry");

float smoothstep01(float t) {
  if (t <= 0.0f) return 0.0f;
  if (t >= 1.0f) return 1.0f;
  return t * t * (3.0f - 2.0f * t);
}

}  // namespace

const ShapeKeyframe* timelineKeyframes() { return kTimeline; }

int timelineKeyframeCount() {
  return static_cast<int>(sizeof(kTimeline) / sizeof(kTimeline[0]));
}

uint32_t timelineCycleFrames() {
  uint32_t total = 0;
  for (const auto& kf : kTimeline) {
    total += static_cast<uint32_t>(kf.holdFrames) +
             static_cast<uint32_t>(kf.morphFrames);
  }
  return total;
}

TimelineSample sampleTimeline(uint32_t globalFrame) {
  const uint32_t cycle = timelineCycleFrames();
  const uint32_t cycleFrame = (cycle == 0) ? 0 : (globalFrame % cycle);

  uint32_t cursor = 0;
  const int count = timelineKeyframeCount();
  for (int k = 0; k < count; ++k) {
    const auto& kf = kTimeline[k];
    const uint32_t holdEnd = cursor + kf.holdFrames;
    const uint32_t morphEnd = holdEnd + kf.morphFrames;
    if (cycleFrame < morphEnd) {
      TimelineSample s;
      s.shapeIdx = kf.shapeIndex;
      const auto& next = kTimeline[(k + 1) % count];
      s.nextShapeIdx = next.shapeIndex;
      s.morphStyle = kf.morphStyle;
      if (cycleFrame < holdEnd || kf.morphFrames == 0) {
        s.inMorph = false;
        s.rawProgress = 0.0f;
        s.eased = 0.0f;
      } else {
        s.inMorph = true;
        s.rawProgress = static_cast<float>(cycleFrame - holdEnd) /
                        static_cast<float>(kf.morphFrames);
        s.eased = (kf.ease == EaseCurve::SmoothStep)
                      ? smoothstep01(s.rawProgress)
                      : s.rawProgress;
      }
      return s;
    }
    cursor = morphEnd;
  }
  // Defensive fallback — cycle math above guarantees we don't reach here.
  TimelineSample s{};
  s.shapeIdx = kTimeline[0].shapeIndex;
  s.nextShapeIdx = s.shapeIdx;
  s.morphStyle = MorphStyle::Linear;
  return s;
}

void applyMorphStyle(MorphStyle style,
                     const float from[3],
                     const float to[3],
                     float t,
                     int /*pointIndex*/,
                     uint32_t /*seed*/,
                     float out[3]) {
  switch (style) {
    case MorphStyle::Linear:
    default:
      // Identical to the previous clerp() — preserved byte-for-byte.
      out[0] = from[0] + t * (to[0] - from[0]);
      out[1] = from[1] + t * (to[1] - from[1]);
      out[2] = from[2] + t * (to[2] - from[2]);
      break;
  }
}

void resolvePoint(uint32_t globalFrame,
                  int pointIndex,
                  const float shapes[Screensaver::kShapeCount]
                                    [Screensaver::kPointCount][3],
                  uint32_t morphSeed,
                  float out[3]) {
  const TimelineSample s = sampleTimeline(globalFrame);
  const auto& from = shapes[s.shapeIdx][pointIndex];
  if (!s.inMorph) {
    out[0] = from[0];
    out[1] = from[1];
    out[2] = from[2];
    return;
  }
  const auto& to = shapes[s.nextShapeIdx][pointIndex];
  applyMorphStyle(s.morphStyle, from, to, s.eased, pointIndex, morphSeed, out);
}

}  // namespace screensaver
