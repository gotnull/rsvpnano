#include "demos/UnlimitedBobs.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <esp_heap_caps.h>

namespace {

// sprites2.gif baked to RGB565. Generated from
// tools/demoref/equinox/unlimitedbobs/sprites2.gif. The 0xC618 ring around
// the edge is the GIF's neutral gray (192,192,192) — stamped together with
// the colourful centre to build the characteristic mesh trail.
constexpr uint16_t kBobSprite[15 * 15] = {
    0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0x0004, 0x3027, 0x70EC, 0x3027, 0x0004, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618,
    0xC618, 0xC618, 0xC618, 0x0006, 0x4869, 0x790C, 0x99F0, 0xAA72, 0x896E, 0x68CB, 0x4869, 0x0006, 0xC618, 0xC618, 0xC618,
    0xC618, 0xC618, 0x3027, 0x812D, 0xAA72, 0xD417, 0xED19, 0xD417, 0xAA72, 0x898F, 0x814D, 0x70CC, 0x3027, 0xC618, 0xC618,
    0xC618, 0x3027, 0x99F0, 0xC375, 0xED19, 0xFE9C, 0xED19, 0xAA72, 0x814D, 0x70CC, 0x70EC, 0x70EC, 0x70CC, 0x0006, 0xC618,
    0x0006, 0x812D, 0xC375, 0xED19, 0xFE9C, 0xFE9C, 0xC375, 0x898F, 0x68CB, 0x68CB, 0x70CC, 0x68CB, 0x790D, 0x3027, 0xC618,
    0x4869, 0xAA72, 0xED19, 0xFE9C, 0xFE9C, 0xED19, 0xAA72, 0x790D, 0x68CB, 0x790D, 0x812D, 0x68CB, 0x70EC, 0x588A, 0x0006,
    0x70CC, 0xC375, 0xFE9C, 0xFE9C, 0xFE9C, 0xD417, 0x99D0, 0x70CC, 0x790D, 0xA211, 0xA211, 0x70EC, 0x70EC, 0x790D, 0x4869,
    0x99D0, 0xED19, 0xFE9C, 0xFE9C, 0xED19, 0xBB14, 0x790D, 0x68CB, 0x812D, 0x99D0, 0x898F, 0x812D, 0xA211, 0xA211, 0x68CB,
    0x790C, 0xC375, 0xED19, 0xED19, 0xD417, 0x99F0, 0x70EC, 0x814D, 0x790D, 0x68CB, 0x70CC, 0x898F, 0xAA72, 0x99D0, 0x4869,
    0x4869, 0x99F0, 0xC375, 0xD417, 0xBB14, 0x814D, 0x812D, 0xAA72, 0x99D0, 0x70EC, 0x812D, 0xAA72, 0xAA72, 0x588A, 0x0004,
    0x0004, 0x68CB, 0xA211, 0x99F0, 0x812D, 0x68CB, 0x812D, 0xAA72, 0x99D0, 0x898F, 0xAA72, 0xC375, 0xAA72, 0x588A, 0xC618,
    0xC618, 0x0006, 0x70EC, 0x790D, 0x68CB, 0x68CB, 0x814D, 0x99D0, 0x99D0, 0xAA72, 0xC375, 0xBB14, 0x896E, 0x0006, 0xC618,
    0xC618, 0xC618, 0x3027, 0x70CC, 0x70EC, 0x790D, 0x814D, 0x898F, 0xA211, 0xC375, 0xBB14, 0x814D, 0x3027, 0xC618, 0xC618,
    0xC618, 0xC618, 0xC618, 0x0006, 0x60AA, 0x790D, 0x898F, 0xA211, 0xAA72, 0xA211, 0x70EC, 0x0006, 0xC618, 0xC618, 0xC618,
    0xC618, 0xC618, 0xC618, 0xC618, 0xC618, 0x4869, 0x4869, 0x70EC, 0x70EC, 0x4869, 0xC618, 0xC618, 0xC618, 0xC618, 0xC618,
};

}  // namespace

UnlimitedBobs::~UnlimitedBobs() {
  if (framebuffer_ != nullptr) {
    heap_caps_free(framebuffer_);
    framebuffer_ = nullptr;
  }
}

void UnlimitedBobs::begin(uint32_t seedMs) {
  if (framebuffer_ == nullptr) {
    framebuffer_ = static_cast<uint16_t *>(
        heap_caps_malloc(kFramebufferBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (framebuffer_ == nullptr) {
      framebuffer_ = static_cast<uint16_t *>(
          heap_caps_malloc(kFramebufferBytes, MALLOC_CAP_8BIT));
    }
  }
  if (framebuffer_ != nullptr) {
    std::memset(framebuffer_, 0, kFramebufferBytes);
  }
  au_ = at_ = as_ = ar_ = 0.0f;
  s_ = 5.0f;
  r_ = 200.0f;
  q_ = 0.0125f;
  p_ = 0.15f;
  // Random starting curve so consecutive launches show different traces.
  curveIndex_ = static_cast<uint8_t>((seedMs ? seedMs : 1u) % kCurveCount);
}

void UnlimitedBobs::cycleCurve() {
  curveIndex_ = (curveIndex_ + 1) % kCurveCount;
  if (framebuffer_ != nullptr) std::memset(framebuffer_, 0, kFramebufferBytes);
  au_ = at_ = as_ = ar_ = 0.0f;
}

void UnlimitedBobs::runCurveStep() {
  // a.g()'s switch(n) — six different sinusoid-driven curves for the sprite
  // stamp position. `t=0` in eqx=1 mode so we drop that subtraction.
  const int halfW = kFrameWidth / 2 - kSpriteSize / 2;
  const int halfH = kFrameHeight / 2 - kSpriteSize / 2;
  const float ampX = static_cast<float>(halfW - 8);
  const float ampY = static_cast<float>(halfH - 8);
  switch (curveIndex_) {
    case 0:
      stampX_ = static_cast<int16_t>(ampX * cosf(au_) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.02f;
      stampY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.02f;
      break;
    case 1:
      stampX_ = static_cast<int16_t>(
          ampX * cosf(au_ * 3.0f + 1.57f) * cosf(au_ / 50.0f) + halfW);
      au_ -= 0.015f;
      stampY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.01f) * cosf(au_ / 80.0f) + halfH);
      at_ -= 0.02f;
      break;
    case 2:
      stampX_ = static_cast<int16_t>(
          ampX * (cosf(au_) * cosf(au_ / 80.0f) + sinf(as_ / 5.0f)) / 2.0f + halfW);
      au_ += 0.04f;
      as_ += 0.003f;
      stampY_ = static_cast<int16_t>(
          ampY * (sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + cosf(ar_ / 5.0f)) / 2.0f + halfH);
      at_ += 0.038f;
      ar_ += 0.00305f;
      break;
    case 3:
      stampX_ = static_cast<int16_t>(ampX * cosf(cosf(au_)) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.02f;
      stampY_ = static_cast<int16_t>(ampY * sinf(au_ * 1.02f) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.02f;
      break;
    case 4:
      stampX_ = static_cast<int16_t>(
          ampX * (cosf(au_ * 0.99f) * sinf(au_ / 80.0f + at_) + sinf(as_ / 5.0f)) / 2.0f
          + halfW);
      au_ += 0.04f;
      as_ += 0.003f;
      stampY_ = static_cast<int16_t>(
          ampY * (sinf(au_ * 0.98f) * cosf(au_ / 60.0f) + cosf(ar_ / 5.0f)) / 2.0f + halfH);
      at_ += 0.038f;
      ar_ += 0.00305f;
      break;
    case 5:
      stampX_ = static_cast<int16_t>(ampX * cosf(au_) * cosf(au_ / 80.0f) + halfW);
      au_ += 0.0095f;
      stampY_ = static_cast<int16_t>(
          ampY * cosf(sinf(au_ * 1.02f) * 2.0f + at_) * cosf(au_ / 60.0f) + halfH);
      at_ += 0.01f;
      break;
  }
  // Amplitude oscillation (preserved for fidelity; doesn't affect stamp
  // position because `t=0` in eqx=1).
  s_ += q_;
  r_ += p_;
  if (s_ < 3.0f || s_ > 175.0f) q_ = -q_;
  if (r_ < 3.0f || r_ > 250.0f) p_ = -p_;
}

void UnlimitedBobs::stampSprite(int cx, int cy) {
  if (framebuffer_ == nullptr) return;
  for (int sy = 0; sy < kSpriteSize; ++sy) {
    const int dy = cy + sy;
    if (dy < 0 || dy >= kFrameHeight) continue;
    uint16_t *dst = framebuffer_ + dy * kFrameWidth + cx;
    const uint16_t *src = kBobSprite + sy * kSpriteSize;
    int x0 = 0, x1 = kSpriteSize;
    if (cx < 0) x0 = -cx;
    if (cx + kSpriteSize > kFrameWidth) x1 = kFrameWidth - cx;
    for (int sx = x0; sx < x1; ++sx) {
      dst[sx] = src[sx];
    }
  }
}

void UnlimitedBobs::tick(uint32_t nowMs) {
  (void)nowMs;
  if (framebuffer_ == nullptr) return;
  for (int i = 0; i < kStepsPerFrame; ++i) {
    runCurveStep();
    stampSprite(stampX_, stampY_);
  }
}
