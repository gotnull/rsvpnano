// UnlimitedBobs renderer for DisplayManager — extracted from
// DisplayManager.cpp so the main file isn't padded further. Sources its
// 320×200 RGB565 framebuffer from UnlimitedBobs::framebuffer() and
// letterboxes it to the 640×172 panel preserving aspect ratio.

#include "display/DisplayManager.h"

#include <algorithm>
#include <cstring>

#include "board/BoardConfig.h"
#include "demos/UnlimitedBobs.h"

namespace {
constexpr int kDisplayWidth = BoardConfig::DISPLAY_WIDTH;
constexpr int kDisplayHeight = BoardConfig::DISPLAY_HEIGHT;
constexpr int kPanelNativeWidth = BoardConfig::PANEL_NATIVE_WIDTH;
constexpr int kPanelNativeHeight = BoardConfig::PANEL_NATIVE_HEIGHT;
// Match the stripe height used by every other native-stripe renderer in
// DisplayManager. 47 keeps txBuffer_ (172*47 px) under the SPI chunk limit.
constexpr int kMaxChunkPhysicalRows = 47;

// Local copy of DisplayManager.cpp's anonymous-namespace panelColor — the
// AXS15231B expects RGB565 with the byte order swapped.
inline uint16_t panelColor(uint16_t rgb565) {
  return static_cast<uint16_t>((rgb565 << 8) | (rgb565 >> 8));
}
}  // namespace

void DisplayManager::renderUnlimitedBobsFrame(const UnlimitedBobs &ub) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  const uint16_t *frame = ub.framebuffer();
  if (frame == nullptr) {
    for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
         stripeStart += kMaxChunkPhysicalRows) {
      const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
      std::memset(txBuffer_, 0,
                  static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t));
      if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
        return;
      }
    }
    return;
  }

  // Letterbox 320×200 → 640×172 preserving aspect: scale = 172/200 → visible
  // 275×172, centred horizontally with ~183 px black bars each side.
  constexpr int kVisibleW = 275;
  constexpr int kLeftMargin = (kDisplayWidth - kVisibleW) / 2;
  constexpr int kRightEdge = kLeftMargin + kVisibleW;
  uint16_t srcXForCol[kVisibleW];
  for (int c = 0; c < kVisibleW; ++c) {
    int sx = (c * UnlimitedBobs::kFrameWidth) / kVisibleW;
    if (sx < 0) sx = 0;
    if (sx >= UnlimitedBobs::kFrameWidth) sx = UnlimitedBobs::kFrameWidth - 1;
    srcXForCol[c] = static_cast<uint16_t>(sx);
  }
  uint16_t srcYForLogicalY[kDisplayHeight];
  for (int y = 0; y < kDisplayHeight; ++y) {
    int sy = (y * UnlimitedBobs::kFrameHeight) / kDisplayHeight;
    if (sy < 0) sy = 0;
    if (sy >= UnlimitedBobs::kFrameHeight) sy = UnlimitedBobs::kFrameHeight - 1;
    srcYForLogicalY[y] = static_cast<uint16_t>(sy);
  }
  const uint16_t kBlack = panelColor(0x0000);

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
      const int ly = (kDisplayHeight - 1) - nativeX;
      const uint16_t *srcRow = frame + srcYForLogicalY[ly] * UnlimitedBobs::kFrameWidth;
      for (int localY = 0; localY < rows; ++localY) {
        const int lx = stripeStart + localY;
        uint16_t color;
        if (lx < kLeftMargin || lx >= kRightEdge) {
          color = kBlack;
        } else {
          color = panelColor(srcRow[srcXForCol[lx - kLeftMargin]]);
        }
        txBuffer_[localY * kPanelNativeWidth + nativeX] = color;
      }
    }
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}
