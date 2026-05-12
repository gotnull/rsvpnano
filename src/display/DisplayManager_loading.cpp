// Loading-overlay renderer for DisplayManager.
//
// Extracted from DisplayManager.cpp to keep new additions out of the 5k-line
// main file. Pattern to follow for future new methods: one file per cohesive
// feature, included via the standard DisplayManager.h header.

#include "display/DisplayManager.h"

#include <Arduino.h>
#include <cmath>

#include "board/BoardConfig.h"

namespace {

// Local copies of the tiny-font metrics + display geometry used by this
// overlay. Kept tiny so this file stays self-contained; if/when we extract
// more methods we'll fold these into a shared DisplayManagerInternal.h.
constexpr int kDisplayWidth = BoardConfig::DISPLAY_WIDTH;
constexpr int kDisplayHeight = BoardConfig::DISPLAY_HEIGHT;
constexpr int kTinyGlyphHeight = 7;
constexpr int kTinyScale = 2;

}  // namespace

void DisplayManager::renderLoadingOverlay(const String &title, const String &detail,
                                          uint32_t tickMs) {
  // Always repaints — caller drives the spinner phase via `tickMs`. Used when
  // the caller is about to do slow synchronous work and wants the screen to
  // land instantly with a clear visual indicator.
  lastRenderKey_ = "";

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  clearVirtualBuffer(virtualWidth, virtualHeight);

  // Dot-ring spinner: 8 dots arranged in a small circle, one accent dot.
  constexpr int kDotCount = 8;
  constexpr int kDotRadius = 3;
  const int ringRadius = 18;
  const int cx = virtualWidth / 2;
  const int cy = virtualHeight / 2 - 12;
  const int accent = static_cast<int>((tickMs / 100U) % kDotCount);
  const uint16_t baseColor = dimColor();
  const uint16_t accentColor = focusColor();
  for (int i = 0; i < kDotCount; ++i) {
    const float a = static_cast<float>(i) / kDotCount * 6.28318f;
    const int dx = cx + static_cast<int>(std::cos(a) * ringRadius) - kDotRadius;
    const int dy = cy + static_cast<int>(std::sin(a) * ringRadius) - kDotRadius;
    fillVirtualRect(dx, dy, kDotRadius * 2, kDotRadius * 2,
                    i == accent ? accentColor : baseColor);
  }

  const int titleY = cy + ringRadius + 12;
  const int textMaxWidth = virtualWidth - 24;
  if (!title.isEmpty()) {
    drawTinyTextCentered(fitTinyText(title, textMaxWidth, kTinyScale), titleY,
                         focusColor(), kTinyScale);
  }
  if (!detail.isEmpty()) {
    drawTinyTextCentered(fitTinyText(detail, textMaxWidth, kTinyScale),
                         titleY + kTinyGlyphHeight * kTinyScale + 6,
                         dimColor(), kTinyScale);
  }

  flushScaledFrame(scale, virtualWidth, virtualHeight);
}
