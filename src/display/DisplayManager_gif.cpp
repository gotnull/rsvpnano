// Native-stripe GIF frame renderer for DisplayManager. The GifPlayer hands
// us a decoded RGB565 source canvas in logical (little-endian) orientation;
// this fits-it to the 640×172 panel preserving aspect ratio, centers with
// black letterbox/pillarbox, and runs everything through the same stripe
// pipeline used by the demos. The retro-effects overlay applies after the
// stripe is composed, so scanlines/tint/dot-matrix/glitch all stack on top
// of GIF playback for free.
//
// Mapping (rotated landscape — uiRotated_ == true):
//   logicalX = nativeY                      [0..639]
//   logicalY = (kPanelNativeWidth-1) - nativeX  [0..171, mirrored on X]
// (uiRotated_ == false flips both axes; we honour the flag.)
//
// Cost: one Q16 inverse-scale multiply per panel column for srcY, one per
// stripe row for srcX, plus one memory read per pixel into the source
// buffer. Roughly 1 ms compose per stripe at 128×128 GIFs; comfortably
// inside the 5 ms compose budget at 60 fps.

#include "display/DisplayManager.h"

#include <algorithm>
#include <cstring>

#include "board/BoardConfig.h"

namespace {

constexpr int kDisplayWidth = BoardConfig::DISPLAY_WIDTH;
constexpr int kDisplayHeight = BoardConfig::DISPLAY_HEIGHT;
constexpr int kPanelNativeWidth = BoardConfig::PANEL_NATIVE_WIDTH;
constexpr int kPanelNativeHeight = BoardConfig::PANEL_NATIVE_HEIGHT;
constexpr int kMaxChunkBytes = 16 * 1024;
constexpr int kBytesPerPixel = 2;
constexpr int kTxBufferWidth = kPanelNativeWidth;
constexpr int kMaxChunkPhysicalRows = kMaxChunkBytes / (kTxBufferWidth * kBytesPerPixel);

inline uint16_t panelEncode(uint16_t rgb565) {
  return static_cast<uint16_t>((rgb565 << 8) | (rgb565 >> 8));
}

}  // namespace

void DisplayManager::renderGifFrame(const uint16_t *frame, int sourceWidth, int sourceHeight) {
  if (!initialized_) return;
  lastRenderKey_ = "";
  if (frame == nullptr || sourceWidth <= 0 || sourceHeight <= 0) {
    fillScreen(0x0000);
    return;
  }

  // Aspect-preserving fit using integer (Q16) inverse-scale math. We pick
  // the smaller of horizontal/vertical scales so the GIF never overflows
  // the panel; the unused axis gets centered black bars.
  // dstW/dstH = floor(min(panelW/srcW, panelH/srcH) * src{W,H}).
  const int scaleNumX = kDisplayWidth;
  const int scaleNumY = kDisplayHeight;
  // Compare srcH * panelW vs srcW * panelH — picking the side that
  // saturates determines whether we letterbox horizontally or vertically.
  const long long horizCap = static_cast<long long>(sourceHeight) * scaleNumX;
  const long long vertCap = static_cast<long long>(sourceWidth) * scaleNumY;
  int dstW = kDisplayWidth;
  int dstH = kDisplayHeight;
  if (horizCap <= vertCap) {
    // Vertical axis is the limit → fit by height.
    dstH = kDisplayHeight;
    dstW = static_cast<int>(static_cast<long long>(sourceWidth) * kDisplayHeight / sourceHeight);
  } else {
    dstW = kDisplayWidth;
    dstH = static_cast<int>(static_cast<long long>(sourceHeight) * kDisplayWidth / sourceWidth);
  }
  if (dstW < 1) dstW = 1;
  if (dstH < 1) dstH = 1;
  const int dstXMin = (kDisplayWidth - dstW) / 2;
  const int dstYMin = (kDisplayHeight - dstH) / 2;
  const int dstXMax = dstXMin + dstW;
  const int dstYMax = dstYMin + dstH;

  // Q16 inverse-scale steps: srcX = (lx - dstXMin) * srcW / dstW. Precompute
  // a step value and apply via 32-bit multiply per column/row.
  const uint32_t stepXQ16 =
      (static_cast<uint32_t>(sourceWidth) << 16) / static_cast<uint32_t>(dstW);
  const uint32_t stepYQ16 =
      (static_cast<uint32_t>(sourceHeight) << 16) / static_cast<uint32_t>(dstH);

  const bool rotated = uiRotated_;
  const uint16_t blackPanel = panelEncode(0x0000);

  // Precompute per-native-column the srcY (or -1 if this column is in the
  // letterbox). Native X iterates 172 columns total.
  int16_t srcYForCol[kPanelNativeWidth];
  for (int c = 0; c < kPanelNativeWidth; ++c) {
    const int ly = rotated ? (kPanelNativeWidth - 1 - c) : c;
    if (ly < dstYMin || ly >= dstYMax) {
      srcYForCol[c] = -1;
      continue;
    }
    const uint32_t srcYQ16 = static_cast<uint32_t>(ly - dstYMin) * stepYQ16;
    int sy = static_cast<int>(srcYQ16 >> 16);
    if (sy < 0) sy = 0;
    if (sy >= sourceHeight) sy = sourceHeight - 1;
    srcYForCol[c] = static_cast<int16_t>(sy);
  }

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    for (int r = 0; r < rows; ++r) {
      const int ny = stripeStart + r;
      const int lx = rotated ? ny : (kDisplayWidth - 1 - ny);
      uint16_t *out = txBuffer_ + r * kPanelNativeWidth;
      if (lx < dstXMin || lx >= dstXMax) {
        // Pillarbox row — solid black.
        for (int c = 0; c < kPanelNativeWidth; ++c) out[c] = blackPanel;
        continue;
      }
      const uint32_t srcXQ16 = static_cast<uint32_t>(lx - dstXMin) * stepXQ16;
      int sx = static_cast<int>(srcXQ16 >> 16);
      if (sx < 0) sx = 0;
      if (sx >= sourceWidth) sx = sourceWidth - 1;
      const uint16_t *srcRowBase = frame + sx;  // column-stable per row
      // For nearest-neighbour with srcX constant per native-Y row, only srcY
      // varies along nativeX. Walk columns, sampling srcRow[srcY * srcW].
      for (int c = 0; c < kPanelNativeWidth; ++c) {
        const int16_t sy = srcYForCol[c];
        if (sy < 0) {
          out[c] = blackPanel;
        } else {
          out[c] = panelEncode(srcRowBase[static_cast<ptrdiff_t>(sy) * sourceWidth]);
        }
      }
    }

    applyEffectsToStripe(stripeStart, rows);
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}
