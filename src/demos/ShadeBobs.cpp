#include "demos/ShadeBobs.h"

#include <algorithm>
#include <cstring>
#include <esp_heap_caps.h>
#include <math.h>

namespace {

uint32_t splitmix32(uint32_t &state) {
  uint32_t z = (state += 0x9E3779B9u);
  z = (z ^ (z >> 16)) * 0x85EBCA6Bu;
  z = (z ^ (z >> 13)) * 0xC2B2AE35u;
  return z ^ (z >> 16);
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

uint8_t lerp8(uint8_t a, uint8_t b, uint8_t t) {
  return static_cast<uint8_t>(a + ((static_cast<int>(b) - a) * t) / 255);
}

}  // namespace

ShadeBobs::~ShadeBobs() {
  if (canvas_ != nullptr) {
    heap_caps_free(canvas_);
    canvas_ = nullptr;
  }
}

void ShadeBobs::buildPalette(uint32_t seedMs) {
  // 4-stop ramp: black → primary → secondary → white (so trail saturation
  // scales with overlap count). Seed picks the two saturated colors so each
  // session has a different identity.
  uint32_t s = (seedMs != 0) ? seedMs : 0xC0FFEE99u;
  uint8_t stops[4][3];
  stops[0][0] = stops[0][1] = stops[0][2] = 0;            // black
  for (int k = 1; k < 3; ++k) {
    const uint32_t r = splitmix32(s);
    stops[k][0] = static_cast<uint8_t>((r >>  0) & 0xFF);
    stops[k][1] = static_cast<uint8_t>((r >>  8) & 0xFF);
    stops[k][2] = static_cast<uint8_t>((r >> 16) & 0xFF);
    int strongest = 0;
    for (int c = 1; c < 3; ++c) if (stops[k][c] > stops[k][strongest]) strongest = c;
    stops[k][strongest] = 255;  // saturate the dominant channel
  }
  stops[3][0] = stops[3][1] = stops[3][2] = 255;          // white

  for (int i = 0; i < kPaletteSize; ++i) {
    const int seg = (i * 3) / kPaletteSize;             // 0,1,2
    const int segPos = (i * 3) - (seg * kPaletteSize);  // 0..(kPaletteSize-1)
    const uint8_t t = static_cast<uint8_t>((segPos * 255) / (kPaletteSize - 1));
    const uint8_t *a = stops[seg];
    const uint8_t *b = stops[seg + 1];
    palette_[i] = rgb565(lerp8(a[0], b[0], t), lerp8(a[1], b[1], t), lerp8(a[2], b[2], t));
  }
}

void ShadeBobs::begin(uint32_t seedMs) {
  if (canvas_ == nullptr) {
    canvas_ = static_cast<uint8_t *>(
        heap_caps_malloc(kCanvasBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (canvas_ == nullptr) {
      // Last-ditch: try internal RAM. Probably won't fit but at least we
      // bail visibly rather than crashing on first write.
      canvas_ = static_cast<uint8_t *>(heap_caps_malloc(kCanvasBytes, MALLOC_CAP_8BIT));
    }
  }
  if (canvas_ != nullptr) {
    std::memset(canvas_, 0, kCanvasBytes);
  }
  buildPalette(seedMs);

  uint32_t s = seedMs ? seedMs : 0xBADCAFE0u;
  for (int i = 0; i < kBobCount; ++i) {
    phaseAX_[i] = static_cast<float>(splitmix32(s) & 0xFFFF) / 65535.0f * 6.2831853f;
    phaseAY_[i] = static_cast<float>(splitmix32(s) & 0xFFFF) / 65535.0f * 6.2831853f;
    // Per-bob angular speeds in rad/s. Keep them small + irrational-ish so
    // the Lissajous never quite repeats. Seed perturbs slightly.
    const float jx = static_cast<float>(splitmix32(s) & 0xFF) / 255.0f;
    const float jy = static_cast<float>(splitmix32(s) & 0xFF) / 255.0f;
    speedAX_[i] = 0.7f + 0.4f * i + 0.13f * jx;
    speedAY_[i] = 0.9f + 0.3f * i + 0.17f * jy;
    bobNativeX_[i] = kNativeWidth / 2;
    bobNativeY_[i] = kNativeHeight / 2;
  }
  lastMs_ = 0;
}

void ShadeBobs::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 60.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;

  if (canvas_ == nullptr) return;

  // Logical-screen amplitudes — a margin keeps the bobs (radius 9) inside
  // the visible area.
  constexpr int kLogicalWidth = 640;
  constexpr int kLogicalHeight = 172;
  constexpr int kMargin = kBobRadius + 4;
  const float halfX = static_cast<float>(kLogicalWidth / 2 - kMargin);
  const float halfY = static_cast<float>(kLogicalHeight / 2 - kMargin);

  for (int i = 0; i < kBobCount; ++i) {
    phaseAX_[i] += speedAX_[i] * dt;
    phaseAY_[i] += speedAY_[i] * dt;
    const int sx = kLogicalWidth / 2 + static_cast<int>(halfX * sinf(phaseAX_[i]));
    const int sy = kLogicalHeight / 2 + static_cast<int>(halfY * cosf(phaseAY_[i]));
    // Logical → native (same mapping the screensaver uses).
    bobNativeX_[i] = static_cast<int16_t>((kLogicalHeight - 1) - sy);
    bobNativeY_[i] = static_cast<int16_t>(sx);

    // Bresenham-style filled disc of radius kBobRadius — increment each
    // covered pixel. Tight inner loop, no branches in the hot path.
    const int cx = bobNativeX_[i];
    const int cy = bobNativeY_[i];
    for (int dy = -kBobRadius; dy <= kBobRadius; ++dy) {
      const int yy = cy + dy;
      if (yy < 0 || yy >= kNativeHeight) continue;
      // Half-width at this row: floor(sqrt(r² − dy²))
      const int dy2 = dy * dy;
      const int r2 = kBobRadius * kBobRadius;
      if (dy2 > r2) continue;
      int dx = 0;
      while ((dx + 1) * (dx + 1) + dy2 <= r2) ++dx;
      const int xLo = std::max(0, cx - dx);
      const int xHi = std::min(kNativeWidth - 1, cx + dx);
      uint8_t *row = canvas_ + yy * kNativeWidth + xLo;
      for (int x = xLo; x <= xHi; ++x) {
        ++(*row);  // wraps at 256 → returns to black, smooth cycle
        ++row;
      }
    }
  }
}
