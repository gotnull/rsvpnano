#pragma once

#include <Arduino.h>

// "ShadeBobs" — additive Lissajous bobs writing palette indices into a
// persistent 1-byte-per-pixel canvas. Overlapping bob pixels increment the
// canvas, walking through a smooth gradient palette so trails appear in the
// touched regions. Classic 90s-era PC demo effect; we render it at full
// 640×172 native resolution.
//
// State (canvas + palette) is owned by this object. The canvas is allocated
// in PSRAM at begin() time (110 KB) and reused for the lifetime of the
// instance — no per-frame heap traffic.
class ShadeBobs {
 public:
  // Native panel orientation. Storing the canvas in native order means the
  // render path can do sequential PSRAM reads (fast) instead of strided
  // logical-→-native lookups.
  static constexpr int kNativeWidth = 172;
  static constexpr int kNativeHeight = 640;
  static constexpr size_t kCanvasBytes =
      static_cast<size_t>(kNativeWidth) * kNativeHeight;
  static constexpr int kPaletteSize = 256;

  // 3 bobs is the sweet spot — fewer feels sparse, more turns the screen into
  // a flat smear within seconds.
  static constexpr int kBobCount = 3;
  static constexpr int kBobRadius = 9;

  ~ShadeBobs();

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  const uint8_t *canvas() const { return canvas_; }
  const uint16_t *palette() const { return palette_; }

  // Per-bob native pixel center for the current frame. The renderer calls
  // these to compute which canvas cells need incrementing.
  int bobNativeX(int i) const { return bobNativeX_[i]; }
  int bobNativeY(int i) const { return bobNativeY_[i]; }

 private:
  void buildPalette(uint32_t seedMs);

  // Lazily allocated in begin() so the global App constructor stays cheap.
  uint8_t *canvas_ = nullptr;
  uint16_t palette_[kPaletteSize] = {0};

  // Per-bob phase pairs — each bob orbits on (sin(a), cos(b)) with
  // independent angular speeds for non-repeating Lissajous patterns.
  float phaseAX_[kBobCount] = {0};
  float phaseAY_[kBobCount] = {0};
  float speedAX_[kBobCount] = {0};
  float speedAY_[kBobCount] = {0};
  int16_t bobNativeX_[kBobCount] = {0};
  int16_t bobNativeY_[kBobCount] = {0};
  uint32_t lastMs_ = 0;
};
