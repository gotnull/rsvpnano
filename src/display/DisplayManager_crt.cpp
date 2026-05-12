// CRT-shader post-process for DisplayManager. Called by demo renderers after
// they fill a native-stripe txBuffer_ chunk; dims alternate logical-Y rows
// (= alternate native columns on the rotated panel) so the panel looks like
// a CRT/LCD with horizontal scanlines. No bezel, no curvature — just the
// scanline darkening. Cheap: ~4 K pixel writes per 47-row stripe.

#include "display/DisplayManager.h"

#include "board/BoardConfig.h"

namespace {
constexpr int kPanelNativeWidth = BoardConfig::PANEL_NATIVE_WIDTH;
}

void DisplayManager::applyCrtToStripe(int stripeRows) {
  if (!crtShaderEnabled_) return;
  if (txBuffer_ == nullptr) return;
  // Walk every other native-X column (= every other logical row) and dim its
  // pixels by 5/8. Panel format is byte-swapped RGB565, so we un-swap to
  // get the channels, scale, re-swap.
  for (int row = 0; row < stripeRows; ++row) {
    uint16_t *line = txBuffer_ + row * kPanelNativeWidth;
    for (int nx = 1; nx < kPanelNativeWidth; nx += 2) {
      const uint16_t p = line[nx];
      const uint16_t rgb = static_cast<uint16_t>((p << 8) | (p >> 8));
      uint8_t r = (rgb >> 11) & 0x1F;
      uint8_t g = (rgb >> 5) & 0x3F;
      uint8_t b = rgb & 0x1F;
      r = static_cast<uint8_t>((r * 5) >> 3);
      g = static_cast<uint8_t>((g * 5) >> 3);
      b = static_cast<uint8_t>((b * 5) >> 3);
      const uint16_t dim = static_cast<uint16_t>((r << 11) | (g << 5) | b);
      line[nx] = static_cast<uint16_t>((dim << 8) | (dim >> 8));
    }
  }
}
