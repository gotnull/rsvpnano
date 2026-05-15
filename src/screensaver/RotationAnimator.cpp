#include "screensaver/RotationAnimator.h"

#include <math.h>

namespace screensaver {
namespace {

// Multi-segment rotation timeline — preserves the previous parallel-array
// values verbatim. Total cycle: 320+240+280+200+320+260+220 = 1840 frames
// ≈ 31 s @ 60 fps. Vary which axis dominates over time so the cluster
// doesn't feel like a single uniform spin.
constexpr RotationSegment kSegments[] = {
    {320, 0.012f, 0.018f, 0.004f},
    {240, 0.008f, 0.022f, 0.012f},
    {280, 0.018f, 0.010f, 0.020f},
    {200, 0.005f, 0.030f, 0.008f},
    {320, 0.010f, 0.014f, 0.006f},
    {260, 0.022f, 0.006f, 0.018f},
    {220, 0.006f, 0.024f, 0.014f},
};

}  // namespace

const RotationSegment* rotationSegments() { return kSegments; }

int rotationSegmentCount() {
  return static_cast<int>(sizeof(kSegments) / sizeof(kSegments[0]));
}

uint32_t rotationCycleFrames() {
  uint32_t total = 0;
  for (const auto& seg : kSegments) total += seg.frames;
  return total;
}

void RotationAnimator::reset() {
  segIdx_ = 0;
  segFrame_ = 0;
  angleX_ = angleY_ = angleZ_ = 0.0f;
}

void RotationAnimator::advance() {
  const auto& seg = kSegments[segIdx_];
  angleX_ += seg.dx;
  angleY_ += seg.dy;
  angleZ_ += seg.dz;
  if (++segFrame_ >= seg.frames) {
    segFrame_ = 0;
    segIdx_ = static_cast<uint8_t>(
        (segIdx_ + 1) % rotationSegmentCount());
  }
}

void RotationAnimator::buildMatrix(float &m00, float &m01, float &m02,
                                   float &m10, float &m11, float &m12,
                                   float &m20, float &m21, float &m22) const {
  const float f1 = cosf(angleX_), f2 = sinf(angleX_);
  const float f3 = cosf(angleY_), f4 = sinf(angleY_);
  const float f5 = cosf(angleZ_), f6 = sinf(angleZ_);
  m00 = f1 * f3;
  m01 = -(f1 * f4 * f6 + f2 * f5);
  m02 = -(f1 * f4 * f5 - f2 * f6);
  m10 = f2 * f3;
  m11 = -(f2 * f4 * f6 - f1 * f5);
  m12 = -(f2 * f4 * f5 + f1 * f6);
  m20 = f4;
  m21 = f3 * f6;
  m22 = f3 * f5;
}

}  // namespace screensaver
