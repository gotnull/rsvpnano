#pragma once

#include <cstdint>

namespace screensaver {

// One slot in the multi-segment rotation timeline. The animator spends
// `frames` ticks accumulating per-axis radian deltas, then advances to the
// next segment; the segment table wraps cyclically.
struct RotationSegment {
  uint16_t frames;
  float dx;
  float dy;
  float dz;
};

// Returns the static segment table. Order defines the visit sequence.
const RotationSegment* rotationSegments();
int rotationSegmentCount();

// Total frames in one full rotation-segment cycle. Used for diagnostics /
// future variant scheduling.
uint32_t rotationCycleFrames();

// Frame-stepped accumulator. Owns the per-axis angles and the per-segment
// progression — Screensaver::tick() just calls advance() once per frame and
// reads angle{X,Y,Z}() / buildMatrix().
class RotationAnimator {
 public:
  void reset();
  void advance();  // one tick: apply current segment's deltas, advance segment cursor

  float angleX() const { return angleX_; }
  float angleY() const { return angleY_; }
  float angleZ() const { return angleZ_; }

  // Tait-Bryan ZYX matrix, mirrors the previous in-place builder bit-for-bit.
  // Off-diagonal negations come from the original Equinox derivation.
  void buildMatrix(float &m00, float &m01, float &m02,
                   float &m10, float &m11, float &m12,
                   float &m20, float &m21, float &m22) const;

 private:
  uint8_t segIdx_ = 0;
  uint16_t segFrame_ = 0;
  float angleX_ = 0.0f, angleY_ = 0.0f, angleZ_ = 0.0f;
};

}  // namespace screensaver
