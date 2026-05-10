#pragma once

#include <Arduino.h>

// Full-screen palette plasma. State is just animation phases; the renderer
// composes per-pixel index + LUT lookup directly into native stripes.
//
// Cost note: ~110k pixels/frame × ~30 cycles each = ~14 ms compose budget at
// best. Combined with ~11.5 ms SPI floor → ~25 ms total → caps at ~40 fps.
// We bump the per-frame throttle to 25 ms (40 fps) when this demo is active
// so we don't burn CPU pretending to target 60.
class Plasma {
 public:
  // 256-entry palette (RGB565) cycled through index space. Generated at
  // begin() time so the user never sees a "raw" plasma — it always has a
  // recognisable color scheme.
  static constexpr int kPaletteSize = 256;
  static constexpr int kSinLutSize = 256;

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  // Phase accumulators used by the per-pixel index formula.
  float pa() const { return pa_; }
  float pb() const { return pb_; }
  float pc() const { return pc_; }
  float pd() const { return pd_; }
  // Palette rotation offset (0..255), advanced slowly per frame.
  uint8_t paletteShift() const { return paletteShift_; }

  // RGB565 palette (panel byte order applied at draw time).
  const uint16_t *palette() const { return palette_; }
  // Q.7 fixed-point sine table: sinLut[i] in [-127, 127] for i in [0, 256).
  // Built once on first begin(), shared across all Plasma instances.
  static const int8_t *sinLutPtr();

 private:
  // Ramps a 3-stop gradient into palette_ at begin(). Cheap and gives the
  // demo a strong identity per session.
  void buildPalette(uint32_t seedMs);

  uint16_t palette_[kPaletteSize] = {0};
  float pa_ = 0.0f;
  float pb_ = 0.0f;
  float pc_ = 0.0f;
  float pd_ = 0.0f;
  uint8_t paletteShift_ = 0;
  uint32_t lastMs_ = 0;
  uint32_t lastShiftMs_ = 0;
};
