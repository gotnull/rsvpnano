// Retro post-process effects for native-stripe rendering. Called by demo /
// GIF / camera renderers via DisplayManager::applyEffectsToStripe() after
// the stripe's txBuffer_ chunk has been composed and before drawBitmap.
//
// Layer order (matches the Byte-90 reference): tint → dot-matrix →
// scanlines → glitch. Each layer is independently toggleable.
//
// All math operates on panel-encoded RGB565: txBuffer_ stores bytes in the
// swapped order the SPI peripheral pushes to the panel. Each pixel is
// byte-unswapped → manipulated → re-swapped.
//
// Geometry: a stripe is `kPanelNativeWidth (172) × stripeRows` pixels in
// row-major order. The panel is rotated 180° in landscape, so:
//   logicalY = (kPanelNativeWidth - 1) - nativeX   (when uiRotated_)
//   logicalX = nativeY                              (when uiRotated_)
// The fall-through path (uiRotated_ == false) mirrors both axes. The screensaver
// patterns doc has the full mapping derivation.

#include "display/DisplayManager.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "board/BoardConfig.h"

namespace {

constexpr int kPanelNativeWidth = BoardConfig::PANEL_NATIVE_WIDTH;
constexpr int kPanelNativeHeight = BoardConfig::PANEL_NATIVE_HEIGHT;

// Byte-swap to/from panel-encoded RGB565. The op is its own inverse.
inline uint16_t swap565(uint16_t p) {
  return static_cast<uint16_t>((p << 8) | (p >> 8));
}

inline uint16_t pack565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>((r << 11) | (g << 5) | b);
}

// rgb888 → rgb565 constexpr — same idiom as Byte-90's retro_tints helper.
constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

// Tint accent colors. RGB565 (not panel-swapped) — applyEffectsToStripe
// handles the swap when blending into the panel buffer.
constexpr uint16_t kTintGreen = rgb565(0x33, 0xFF, 0x66);
constexpr uint16_t kTintAmber = rgb565(0xFF, 0xB0, 0x20);
constexpr uint16_t kTintBlue = rgb565(0x30, 0x80, 0xFF);
constexpr uint16_t kTintMagenta = rgb565(0xFF, 0x40, 0xE0);

uint16_t tintAccent(DisplayManager::TintColor t) {
  switch (t) {
    case DisplayManager::TintColor::Green:   return kTintGreen;
    case DisplayManager::TintColor::Amber:   return kTintAmber;
    case DisplayManager::TintColor::Blue:    return kTintBlue;
    case DisplayManager::TintColor::Magenta: return kTintMagenta;
    default:                                 return 0;
  }
}

// LCG step. Matches the Byte-90 generator constants so the on-screen
// "feel" is the same; we just consume one stream per call rather than
// per-pixel. Quality is fine for visual jitter.
inline uint32_t lcgNext(uint32_t &state) {
  state = state * 1103515245u + 12345u;
  return state;
}

}  // namespace

void DisplayManager::applyEffectsToStripe(int stripeStart, int stripeRows) {
  if (txBuffer_ == nullptr || stripeRows <= 0) return;
  if (!isAnyEffectEnabled()) return;

  // -- Per-frame glitch seed. Reseed only on the first stripe so all stripes
  //    in a frame see the same `glitchBand*` arrays. The seed mixes millis()
  //    so each frame gets a different pattern; the frame counter feeds the
  //    LCG to give the glitch a recognisable "stutter" rather than pure noise.
  if (stripeStart == 0 && effects_.glitchOn) {
    ++effects_.glitchFrameCounter;
    effects_.glitchFrameSeed ^=
        static_cast<uint32_t>(millis()) ^ (effects_.glitchFrameCounter * 0x9E3779B1u);
    // Probability that any glitch happens this frame, scaled by intensity.
    // intensity=100 → tear every frame; intensity=0 → never.
    const uint32_t roll = lcgNext(effects_.glitchFrameSeed) % 100u;
    if (roll < effects_.glitchPct) {
      // 1..3 bands depending on intensity. Each band shifts a contiguous run
      // of native-Y rows horizontally (= a vertical tear in logical view).
      const uint8_t bandCount =
          1 + static_cast<uint8_t>((lcgNext(effects_.glitchFrameSeed) %
                                    (1u + (effects_.glitchPct / 35u))));
      effects_.glitchBandCount = std::min<uint8_t>(bandCount, EffectsState::kMaxBands);
      const int maxShift = 1 + (effects_.glitchPct * 7) / 100;  // 1..8 px
      for (uint8_t i = 0; i < effects_.glitchBandCount; ++i) {
        const uint32_t r1 = lcgNext(effects_.glitchFrameSeed);
        const uint32_t r2 = lcgNext(effects_.glitchFrameSeed);
        const uint32_t r3 = lcgNext(effects_.glitchFrameSeed);
        effects_.glitchBandStartNy[i] =
            static_cast<int16_t>(r1 % static_cast<uint32_t>(kPanelNativeHeight));
        effects_.glitchBandHeight[i] = static_cast<int8_t>(2 + (r2 % 5));  // 2..6
        int shift = static_cast<int>(r3 % static_cast<uint32_t>(maxShift * 2 + 1)) - maxShift;
        if (shift == 0) shift = 1;
        effects_.glitchBandShift[i] = static_cast<int8_t>(shift);
      }
    } else {
      effects_.glitchBandCount = 0;
    }
  }

  // -- Tint. Per-pixel luminance → tint blend. Skipped entirely when None.
  if (effects_.tint != TintColor::None && effects_.tintPct > 0) {
    const uint16_t accent = tintAccent(effects_.tint);
    const uint8_t tr = (accent >> 11) & 0x1F;
    const uint8_t tg = (accent >> 5) & 0x3F;
    const uint8_t tb = accent & 0x1F;
    // Convert tint % → Q8 ratio once per stripe.
    const uint16_t k = static_cast<uint16_t>((effects_.tintPct * 256u) / 100u);
    const uint16_t kInv = 256u - k;
    for (int row = 0; row < stripeRows; ++row) {
      uint16_t *line = txBuffer_ + row * kPanelNativeWidth;
      for (int nx = 0; nx < kPanelNativeWidth; ++nx) {
        const uint16_t rgb = swap565(line[nx]);
        const uint8_t r = (rgb >> 11) & 0x1F;
        const uint8_t g = (rgb >> 5) & 0x3F;
        const uint8_t b = rgb & 0x1F;
        // Approximate luminance in 5-bit space. We desaturate toward gray
        // (so the tint reads through dark scenes too) and modulate the
        // blend by brightness so blacks stay black.
        const uint16_t luma8 = static_cast<uint16_t>((r * 76u + (g >> 1) * 150u + b * 29u) >> 8);
        const uint8_t lr = static_cast<uint8_t>((luma8 * 31u) / 255u);
        const uint8_t lg = static_cast<uint8_t>((luma8 * 63u) / 255u);
        const uint8_t lb = lr;
        // Modulate blend strength by luma — dark pixels barely tint.
        const uint16_t kEff = static_cast<uint16_t>((k * luma8) >> 8);
        const uint16_t kEffInv = 256u - kEff;
        const uint8_t mr = static_cast<uint8_t>((lr * kEffInv + tr * kEff) >> 8);
        const uint8_t mg = static_cast<uint8_t>((lg * kEffInv + tg * kEff) >> 8);
        const uint8_t mb = static_cast<uint8_t>((lb * kEffInv + tb * kEff) >> 8);
        // Mix the desaturated tinted color back with the original by tintPct
        // so partial intensity preserves some original color information.
        const uint8_t fr = static_cast<uint8_t>((r * kInv + mr * k) >> 8);
        const uint8_t fg = static_cast<uint8_t>((g * kInv + mg * k) >> 8);
        const uint8_t fb = static_cast<uint8_t>((b * kInv + mb * k) >> 8);
        line[nx] = swap565(pack565(std::min<uint8_t>(fr, 31), std::min<uint8_t>(fg, 63),
                                   std::min<uint8_t>(fb, 31)));
      }
    }
  }

  // -- Dot matrix. Treat the panel as a grid of dots in *logical* space:
  //    pixels on the dot get a slight brightness lift; off-dot pixels are
  //    dimmed proportional to how "off" they are. Logical (lx, ly) maps to
  //    native via the rotated transform (see header comment).
  if (effects_.dotMatrixOn) {
    const int grid = effects_.dotMatrixSize;
    const bool rotated = uiRotated_;
    // Precompute per-column "is on a dot Y" (since logicalY depends only on
    // nativeX). Saves a modulo per pixel.
    uint8_t litCol[kPanelNativeWidth];
    for (int nx = 0; nx < kPanelNativeWidth; ++nx) {
      const int ly = rotated ? (kPanelNativeWidth - 1 - nx) : nx;
      litCol[nx] = static_cast<uint8_t>((ly % grid) == 0);
    }
    for (int row = 0; row < stripeRows; ++row) {
      const int ny = stripeStart + row;
      const int lx = rotated ? ny : (kPanelNativeHeight - 1 - ny);
      const bool litRow = (lx % grid) == 0;
      uint16_t *line = txBuffer_ + row * kPanelNativeWidth;
      for (int nx = 0; nx < kPanelNativeWidth; ++nx) {
        const bool onDot = litRow && litCol[nx];
        if (onDot) continue;  // keep at full brightness — the dot.
        const uint16_t rgb = swap565(line[nx]);
        uint8_t r = (rgb >> 11) & 0x1F;
        uint8_t g = (rgb >> 5) & 0x3F;
        uint8_t b = rgb & 0x1F;
        // Off-dot pixels: dim by ~70% (3/8 of original brightness).
        r = static_cast<uint8_t>((r * 3) >> 3);
        g = static_cast<uint8_t>((g * 3) >> 3);
        b = static_cast<uint8_t>((b * 3) >> 3);
        line[nx] = swap565(pack565(r, g, b));
      }
    }
  }

  // -- Scanlines. Dim alternate logical-Y rows. Logical-Y maps to native-X
  //    so we dim every other native column. Intensity 0..100 → dim factor
  //    in 0..1; we apply `out = src * (1 - k)` per channel, computed in Q8.
  if (effects_.scanlinesOn && effects_.scanlinesPct > 0) {
    const uint16_t k = static_cast<uint16_t>((effects_.scanlinesPct * 256u) / 100u);
    const uint16_t kInv = 256u - k;
    // Match the legacy shader's parity (dim odd native columns). uiRotated_
    // flips parity; we read the flag here so the visual pattern stays the
    // same as the prior CRT shader after migration.
    const int parity = uiRotated_ ? 1 : 0;
    for (int row = 0; row < stripeRows; ++row) {
      uint16_t *line = txBuffer_ + row * kPanelNativeWidth;
      for (int nx = parity; nx < kPanelNativeWidth; nx += 2) {
        const uint16_t rgb = swap565(line[nx]);
        uint8_t r = (rgb >> 11) & 0x1F;
        uint8_t g = (rgb >> 5) & 0x3F;
        uint8_t b = rgb & 0x1F;
        r = static_cast<uint8_t>((r * kInv) >> 8);
        g = static_cast<uint8_t>((g * kInv) >> 8);
        b = static_cast<uint8_t>((b * kInv) >> 8);
        line[nx] = swap565(pack565(r, g, b));
      }
    }
  }

  // -- Glitch. Tear contiguous bands of native-Y rows horizontally along
  //    nativeX. A band that straddles the stripe boundary is partially
  //    shifted in this stripe and the rest in the next; the seam is hidden
  //    by the underlying scene movement. Empty edges are filled with the
  //    panel-encoded black.
  if (effects_.glitchBandCount > 0) {
    const uint16_t blackPanel = swap565(0x0000);
    const int stripeEnd = stripeStart + stripeRows;
    for (uint8_t b = 0; b < effects_.glitchBandCount; ++b) {
      const int bandStart = effects_.glitchBandStartNy[b];
      const int bandEnd = bandStart + effects_.glitchBandHeight[b];
      const int rowFirst = std::max(bandStart, stripeStart);
      const int rowLast = std::min(bandEnd, stripeEnd);
      if (rowFirst >= rowLast) continue;
      const int shift = effects_.glitchBandShift[b];
      for (int ny = rowFirst; ny < rowLast; ++ny) {
        uint16_t *line = txBuffer_ + (ny - stripeStart) * kPanelNativeWidth;
        if (shift > 0) {
          // Shift right (positive nativeX direction). Move data, blacken left.
          for (int nx = kPanelNativeWidth - 1; nx >= shift; --nx) {
            line[nx] = line[nx - shift];
          }
          for (int nx = 0; nx < shift; ++nx) line[nx] = blackPanel;
        } else {
          const int s = -shift;
          for (int nx = 0; nx < kPanelNativeWidth - s; ++nx) {
            line[nx] = line[nx + s];
          }
          for (int nx = kPanelNativeWidth - s; nx < kPanelNativeWidth; ++nx) {
            line[nx] = blackPanel;
          }
        }
      }
    }
  }
}
