#include "display/DisplayManager.h"

#include "demos/Plasma.h"
#include "demos/Rasterbars.h"
#include "demos/ShadeBobs.h"
#include "demos/SineScroller.h"
#include "demos/Starfield.h"
#include "demos/UnlimitedBobs.h"
#include "demos/Vectorball.h"
#include "demos/VectorballData.h"
#include "screensaver/Screensaver.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#include <esp_heap_caps.h>
#include <esp_log.h>

#include "board/BoardConfig.h"
#include "display/EmbeddedSerifFont.h"
#include "display/EmbeddedSerifFont70.h"
#include "display/axs15231b.h"

namespace {
constexpr int kDisplayWidth = BoardConfig::DISPLAY_WIDTH;
constexpr int kDisplayHeight = BoardConfig::DISPLAY_HEIGHT;
constexpr int kPanelNativeWidth = BoardConfig::PANEL_NATIVE_WIDTH;
constexpr int kPanelNativeHeight = BoardConfig::PANEL_NATIVE_HEIGHT;

constexpr int kBaseGlyphHeight = kEmbeddedSerifHeight;
constexpr int kMinTextScale = 1;
constexpr int kMaxTextScale = 1;
constexpr uint8_t kGlyphAlphaThreshold = 16;
constexpr uint16_t kTrueBlack = 0x0000;
constexpr uint16_t kPureWhite = 0xFFFF;
constexpr uint16_t kDarkWordColor = 0xFFFF;
constexpr uint16_t kLightWordColor = 0x0000;
constexpr uint16_t kFocusLetterColor = 0xF800;
constexpr uint16_t kNightWordColor = 0xFCE0;
constexpr uint16_t kNightFocusColor = 0xFA80;
constexpr uint16_t kDarkMenuDimColor = 0x8410;
constexpr uint16_t kLightMenuDimColor = 0x6B4D;
constexpr uint16_t kDarkFooterColor = 0x528A;
constexpr uint16_t kLightFooterColor = 0x5ACB;
constexpr uint8_t kNightDimAlpha = 92;
constexpr uint8_t kNightFooterAlpha = 132;
constexpr int kRsvpSideMargin = 12;
constexpr int kRsvpGuideTickHeight = 5;
constexpr int kRsvpGuideTopOffset = 7;
constexpr int kRsvpGuideBottomOffset = 7;
constexpr int kWpmFeedbackBottomMargin = 16;
constexpr int kTinyGlyphWidth = 5;
constexpr int kTinyGlyphHeight = 7;
constexpr int kTinyGlyphSpacing = 1;
constexpr int kTinyScale = 2;
constexpr int kFooterMarginX = 12;
constexpr int kFooterMarginBottom = 8;
constexpr int kCompactMenuRowHeight = 22;
constexpr int kCompactMenuX = 28;

// Tabbed-picker band layout. Shared between the full-menu renderer and the
// partial-strip overlay used during slide animations so the two stay in sync.
// Underline TOP sits 1 px above the gray divider so the 2-px bar reads as
// "sitting on the line", not as a thicker line. Divider sits 2 px below the
// bottom of the scale=2 tab label (y=4..17) so it doesn't kiss the glyphs.
constexpr int kTabBandHeight = 21;
constexpr int kTabLabelY = 4;
constexpr int kTabDividerY = 19;
constexpr int kTabUnderlineH = 2;
// Underline top sits ON the divider row (z-ordered above it so the highlight
// overlaps + replaces the gray), extending 1 px below for the 2-px bar.
constexpr int kTabUnderlineY = kTabDividerY;
// Symmetric side margins for the gray divider hairline. The right side has
// to clear the right-aligned battery chip (chip right edge =
// panelWidth − kFooterMarginX − kLibraryLetterStripWidth, widest chip "100%"
// is ~58 px wide + small visual gap = ~100 px). The left mirrors it so the
// divider reads as a centred element on the band.
constexpr int kTabDividerSideMarginPx = 12 + 22 + 58 + 8;  // = 100
constexpr int kTabDividerLeftMarginPx = kTabDividerSideMarginPx;
constexpr int kTabDividerRightMarginPx = kTabDividerSideMarginPx;
constexpr int kTabMenuGapPx = 4;
constexpr int kLibraryRowHeight = 38;
constexpr int kLibraryInsetX = 26;
constexpr int kLibraryTitleYOffset = 4;
constexpr int kLibrarySubtitleYOffset = 20;
constexpr int kLibraryScreenPaddingY = 28;
constexpr uint8_t kLibrarySubtitleAlpha = 120;
constexpr int kLibraryChipPadX = 6;
constexpr int kLibraryChipPadY = 3;
constexpr int kLibraryChipGap = 4;
constexpr int kLibraryChipsRightMargin = 14;
constexpr uint8_t kLibraryChipBgAlpha = 56;
constexpr int kScrubFocusPadX = 10;
constexpr int kScrubFocusPadY = 4;
constexpr int kScrubFocusMaxRadius = 10;
constexpr uint8_t kScrubFocusBgAlpha = 110;
constexpr uint8_t kWpmHighlightBgAlpha = 200;
constexpr int kContextMarginX = 18;
constexpr int kContextTop = 8;
constexpr int kContextLineHeight = 23;
constexpr int kContextParagraphGap = 7;
constexpr int kContextParagraphIndent = 22;
constexpr int kContextSpaceWidth = 8;
constexpr int kContextSerifDivisor = 3;
constexpr size_t kContextTargetLines = 6;
constexpr int kPhantomCurrentGapLarge = 30;
constexpr int kPhantomCurrentGapMedium = 24;
constexpr int kPhantomCurrentGapSmall = 20;
constexpr uint8_t kPhantomAlphaLarge = 54;
constexpr uint8_t kPhantomAlphaMedium = 62;
constexpr uint8_t kPhantomAlphaSmall = 72;
constexpr int kTypographyTrackingMin = -2;
constexpr int kTypographyTrackingMax = 3;
constexpr int kTypographyAnchorMin = 30;
constexpr int kTypographyAnchorMax = 40;
constexpr int kTypographyGuideHalfWidthMin = 12;
constexpr int kTypographyGuideHalfWidthMax = 30;
constexpr int kTypographyGuideGapMin = 2;
constexpr int kTypographyGuideGapMax = 8;
constexpr int kOpticalLetterGapPx = 2;

constexpr int kVirtualBufferWidth = (kDisplayWidth + kMinTextScale - 1) / kMinTextScale;
constexpr int kVirtualBufferHeight = (kDisplayHeight + kMinTextScale - 1) / kMinTextScale;

constexpr size_t kBytesPerPixel = sizeof(uint16_t);
constexpr size_t kMaxChunkBytes = 16 * 1024;
// drawBitmap() always ships kPanelNativeWidth-wide rows, so size the DMA chunk
// to that width — the previous max(display, native) sizing wasted ~73% of the
// chunk and limited us to 12 native rows per SPI transaction. With native width
// (172), the same kMaxChunkBytes fits ~47 rows → ~4× fewer SPI calls per frame.
constexpr int kTxBufferWidth = kPanelNativeWidth;
constexpr int kMaxChunkPhysicalRows = kMaxChunkBytes / (kTxBufferWidth * kBytesPerPixel);
static_assert(kMaxChunkPhysicalRows > 0, "Display chunk buffer must hold at least one row");

constexpr size_t kTxBufferPixels = static_cast<size_t>(kTxBufferWidth) * kMaxChunkPhysicalRows;

struct TinyGlyph {
  char c;
  uint8_t rows[kTinyGlyphHeight];
};

DisplayManager::TypographyConfig &activeTypographyConfig() {
  static DisplayManager::TypographyConfig config;
  return config;
}

int clampTypographyTracking(int value) {
  return std::max(kTypographyTrackingMin, std::min(kTypographyTrackingMax, value));
}

constexpr int kMarqueePixelsPerSecond = 60;
constexpr uint32_t kMarqueeEdgePauseMs = 1200;

int marqueePingPongOffset(int maxOffset) {
  if (maxOffset <= 0) return 0;
  const uint32_t slideMs =
      static_cast<uint32_t>((maxOffset * 1000) / kMarqueePixelsPerSecond);
  const uint32_t cycleMs = (slideMs + kMarqueeEdgePauseMs) * 2;
  const uint32_t phase = millis() % cycleMs;
  if (phase < kMarqueeEdgePauseMs) {
    return 0;
  }
  if (phase < kMarqueeEdgePauseMs + slideMs) {
    const uint32_t slidIn = phase - kMarqueeEdgePauseMs;
    return static_cast<int>((slidIn * maxOffset) / std::max<uint32_t>(1, slideMs));
  }
  if (phase < kMarqueeEdgePauseMs + slideMs + kMarqueeEdgePauseMs) {
    return maxOffset;
  }
  const uint32_t slidBack = phase - kMarqueeEdgePauseMs - slideMs - kMarqueeEdgePauseMs;
  return maxOffset -
         static_cast<int>((slidBack * maxOffset) / std::max<uint32_t>(1, slideMs));
}

String formatCompactCount(uint32_t value) {
  if (value >= 1000000U) {
    const uint32_t tenths = (value + 50000U) / 100000U;
    String out(tenths / 10U);
    if (tenths % 10U != 0U) {
      out += ".";
      out += String(tenths % 10U);
    }
    out += "M";
    return out;
  }
  if (value >= 1000U) {
    return String((value + 500U) / 1000U) + "k";
  }
  return String(value);
}

int clampTypographyAnchorPercent(int value) {
  return std::max(kTypographyAnchorMin, std::min(kTypographyAnchorMax, value));
}

int clampTypographyGuideHalfWidth(int value) {
  return std::max(kTypographyGuideHalfWidthMin, std::min(kTypographyGuideHalfWidthMax, value));
}

int clampTypographyGuideGap(int value) {
  return std::max(kTypographyGuideGapMin, std::min(kTypographyGuideGapMax, value));
}

int currentTypographyTrackingPx() {
  return clampTypographyTracking(activeTypographyConfig().trackingPx);
}

int currentAnchorPercent() {
  return clampTypographyAnchorPercent(activeTypographyConfig().anchorPercent);
}

int currentGuideHalfWidth() {
  return clampTypographyGuideHalfWidth(activeTypographyConfig().guideHalfWidth);
}

int currentGuideGap() {
  return clampTypographyGuideGap(activeTypographyConfig().guideGap);
}

struct ReaderTextStyle {
  uint8_t scalePercent;
  int currentGap;
  uint8_t alpha;
};

constexpr TinyGlyph kTinyGlyphs[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {'!', {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}},
    {'"', {0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00}},
    {'#', {0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A}},
    {'%', {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13}},
    {'&', {0x0C, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0D}},
    {'\'', {0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00}},
    {'(', {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02}},
    {')', {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08}},
    {'*', {0x00, 0x15, 0x0E, 0x1F, 0x0E, 0x15, 0x00}},
    {'+', {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}},
    {',', {0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x08}},
    {'-', {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}},
    {'.', {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C}},
    {'/', {0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10}},
    {'0', {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}},
    {'1', {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}},
    {'2', {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}},
    {'3', {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E}},
    {'4', {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}},
    {'5', {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E}},
    {'6', {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E}},
    {'7', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
    {'8', {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}},
    {'9', {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E}},
    {':', {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00}},
    {';', {0x00, 0x0C, 0x0C, 0x00, 0x06, 0x04, 0x08}},
    {'?', {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04}},
    {'>', {0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10}},
    {'A', {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {'B', {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}},
    {'C', {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
    {'D', {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}},
    {'E', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}},
    {'F', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}},
    {'G', {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F}},
    {'H', {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {'I', {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}},
    {'J', {0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C}},
    {'K', {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}},
    {'L', {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}},
    {'M', {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}},
    {'N', {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}},
    {'O', {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {'P', {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}},
    {'Q', {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}},
    {'R', {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}},
    {'S', {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E}},
    {'T', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {'U', {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {'V', {0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04}},
    {'W', {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}},
    {'X', {0x11, 0x0A, 0x04, 0x04, 0x04, 0x0A, 0x11}},
    {'Y', {0x11, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {'Z', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}},
    {'_', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F}},
};

const EmbeddedSerifGlyph &glyphFor(char c) {
  if (c < static_cast<char>(kEmbeddedSerifFirstChar) ||
      c > static_cast<char>(kEmbeddedSerifLastChar)) {
    c = '?';
  }

  return kEmbeddedSerifGlyphs[static_cast<uint8_t>(c) - kEmbeddedSerifFirstChar];
}

const EmbeddedSerif70Glyph &glyph70For(char c) {
  if (c < static_cast<char>(kEmbeddedSerif70FirstChar) ||
      c > static_cast<char>(kEmbeddedSerif70LastChar)) {
    c = '?';
  }

  return kEmbeddedSerif70Glyphs[static_cast<uint8_t>(c) - kEmbeddedSerif70FirstChar];
}

const uint8_t *tinyRowsFor(char c) {
  if (c >= 'a' && c <= 'z') {
    c = static_cast<char>(c - 'a' + 'A');
  }

  for (const TinyGlyph &glyph : kTinyGlyphs) {
    if (glyph.c == c) {
      return glyph.rows;
    }
  }

  for (const TinyGlyph &glyph : kTinyGlyphs) {
    if (glyph.c == ' ') {
      return glyph.rows;
    }
  }

  return kTinyGlyphs[0].rows;
}

uint16_t panelColor(uint16_t rgb565) {
  return static_cast<uint16_t>((rgb565 << 8) | (rgb565 >> 8));
}

bool isWordCharacter(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

int scaledAdvance(int value, int divisor) {
  divisor = std::max(1, divisor);
  return std::max(1, (value + divisor - 1) / divisor);
}

int scaledSignedAdvance(int value, int divisor) {
  divisor = std::max(1, divisor);
  if (value >= 0) {
    return value / divisor;
  }
  return -(((-value) + divisor - 1) / divisor);
}

int scaledPercentDimension(int value, uint8_t scalePercent) {
  if (scalePercent == 0) {
    scalePercent = 1;
  }
  return std::max(1, (value * static_cast<int>(scalePercent) + 99) / 100);
}

int scaledSignedPercent(int value, uint8_t scalePercent) {
  if (scalePercent == 0) {
    scalePercent = 1;
  }
  if (value >= 0) {
    return (value * static_cast<int>(scalePercent) + 50) / 100;
  }
  return -(((-value) * static_cast<int>(scalePercent) + 50) / 100);
}

int trackedAdvance(int advance, size_t index, size_t length) {
  if (index + 1 >= length) {
    return advance;
  }
  return std::max(1, advance + currentTypographyTrackingPx());
}

int trackedAdvanceScaled(int advance, int divisor, size_t index, size_t length) {
  const int scaled = scaledAdvance(advance, divisor);
  if (index + 1 >= length) {
    return scaled;
  }
  return std::max(1, scaled + scaledSignedAdvance(currentTypographyTrackingPx(), divisor));
}

int trackedAdvanceScaledPercent(int advance, uint8_t scalePercent, size_t index, size_t length) {
  const int scaled = scaledPercentDimension(advance, scalePercent);
  if (index + 1 >= length) {
    return scaled;
  }
  return std::max(1, scaled + scaledSignedPercent(currentTypographyTrackingPx(), scalePercent));
}

int opticalKerningAdjustment(char currentChar, char nextChar, int currentXOffset, int currentWidth,
                             int trackedAdvanceValue, int nextXOffset, int desiredGap) {
  if (!isWordCharacter(currentChar) || !isWordCharacter(nextChar) || currentWidth <= 0) {
    return 0;
  }

  desiredGap = std::max(1, desiredGap);
  const int visibleGap =
      trackedAdvanceValue + nextXOffset - (currentXOffset + currentWidth);
  if (visibleGap <= desiredGap) {
    return 0;
  }

  return std::min(visibleGap - desiredGap, std::max(0, trackedAdvanceValue - 1));
}

int regularDesiredGap() { return std::max(1, kOpticalLetterGapPx + currentTypographyTrackingPx()); }

int scaledDesiredGap(int divisor) {
  return std::max(1, scaledAdvance(kOpticalLetterGapPx, divisor) +
                         scaledSignedAdvance(currentTypographyTrackingPx(), divisor));
}

int scaledPercentDesiredGap(uint8_t scalePercent) {
  return std::max(1, scaledPercentDimension(kOpticalLetterGapPx, scalePercent) +
                         scaledSignedPercent(currentTypographyTrackingPx(), scalePercent));
}

struct TextLayoutMetrics {
  int minX = 0;
  int maxX = 0;
  int focusCenterX = 0;
  bool hasPixels = false;
};

void updateTextLayoutBounds(TextLayoutMetrics &layout, int left, int width) {
  if (width <= 0) {
    return;
  }

  const int right = left + width;
  if (!layout.hasPixels) {
    layout.minX = left;
    layout.maxX = right;
    layout.hasPixels = true;
    return;
  }

  layout.minX = std::min(layout.minX, left);
  layout.maxX = std::max(layout.maxX, right);
}

int textLayoutWidth(const TextLayoutMetrics &layout) {
  if (!layout.hasPixels) {
    return 0;
  }
  return std::max(0, layout.maxX - layout.minX);
}

TextLayoutMetrics serifWordLayout(const String &word, int focusIndex, int divisor = 1) {
  TextLayoutMetrics layout;
  int cursorX = 0;
  const bool trackFocus = focusIndex >= 0;

  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(word[i]);
    const int xOffset = scaledSignedAdvance(static_cast<int>(glyph.xOffset), divisor);
    const int width = glyph.width == 0 ? 0 : scaledAdvance(static_cast<int>(glyph.width), divisor);
    const int advance = scaledAdvance(static_cast<int>(glyph.xAdvance), divisor);
    const int left = cursorX + xOffset;
    updateTextLayoutBounds(layout, left, width);

    if (trackFocus && static_cast<int>(i) == focusIndex) {
      layout.focusCenterX = width > 0 ? left + (width / 2) : cursorX + (advance / 2);
    }

    int tracked = trackedAdvanceScaled(static_cast<int>(glyph.xAdvance), divisor, i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(word[i + 1]);
      tracked -= opticalKerningAdjustment(
          word[i], word[i + 1], xOffset, width, tracked,
          scaledSignedAdvance(static_cast<int>(nextGlyph.xOffset), divisor), scaledDesiredGap(divisor));
    }
    cursorX += std::max(1, tracked);
  }

  if (!trackFocus && layout.hasPixels) {
    layout.focusCenterX = layout.minX + (textLayoutWidth(layout) / 2);
  }

  return layout;
}

TextLayoutMetrics serifWordLayoutScaledPercent(const String &word, int focusIndex,
                                               uint8_t scalePercent) {
  TextLayoutMetrics layout;
  int cursorX = 0;
  const bool trackFocus = focusIndex >= 0;

  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(word[i]);
    const int xOffset = scaledSignedPercent(static_cast<int>(glyph.xOffset), scalePercent);
    const int width =
        glyph.width == 0 ? 0 : scaledPercentDimension(static_cast<int>(glyph.width), scalePercent);
    const int advance = scaledPercentDimension(static_cast<int>(glyph.xAdvance), scalePercent);
    const int left = cursorX + xOffset;
    updateTextLayoutBounds(layout, left, width);

    if (trackFocus && static_cast<int>(i) == focusIndex) {
      layout.focusCenterX = width > 0 ? left + (width / 2) : cursorX + (advance / 2);
    }

    int tracked =
        trackedAdvanceScaledPercent(static_cast<int>(glyph.xAdvance), scalePercent, i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(word[i + 1]);
      tracked -= opticalKerningAdjustment(
          word[i], word[i + 1], xOffset, width, tracked,
          scaledSignedPercent(static_cast<int>(nextGlyph.xOffset), scalePercent),
          scaledPercentDesiredGap(scalePercent));
    }
    cursorX += std::max(1, tracked);
  }

  if (!trackFocus && layout.hasPixels) {
    layout.focusCenterX = layout.minX + (textLayoutWidth(layout) / 2);
  }

  return layout;
}

TextLayoutMetrics serif70WordLayout(const String &word, int focusIndex) {
  TextLayoutMetrics layout;
  int cursorX = 0;
  const bool trackFocus = focusIndex >= 0;

  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerif70Glyph &glyph = glyph70For(word[i]);
    const int left = cursorX + static_cast<int>(glyph.xOffset);
    const int width = static_cast<int>(glyph.width);
    const int advance = static_cast<int>(glyph.xAdvance);
    updateTextLayoutBounds(layout, left, width);

    if (trackFocus && static_cast<int>(i) == focusIndex) {
      layout.focusCenterX = width > 0 ? left + (width / 2) : cursorX + (advance / 2);
    }

    int tracked = trackedAdvance(advance, i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerif70Glyph &nextGlyph = glyph70For(word[i + 1]);
      tracked -= opticalKerningAdjustment(word[i], word[i + 1], static_cast<int>(glyph.xOffset),
                                          width, tracked, static_cast<int>(nextGlyph.xOffset),
                                          regularDesiredGap());
    }
    cursorX += std::max(1, tracked);
  }

  if (!trackFocus && layout.hasPixels) {
    layout.focusCenterX = layout.minX + (textLayoutWidth(layout) / 2);
  }

  return layout;
}

int serifWordWidth(const String &word) { return textLayoutWidth(serifWordLayout(word, -1)); }

int scaledWordWidth(const String &word, int divisor) {
  return textLayoutWidth(serifWordLayout(word, -1, divisor));
}

int scaledWordWidthPercent(const String &word, uint8_t scalePercent) {
  return textLayoutWidth(serifWordLayoutScaledPercent(word, -1, scalePercent));
}

ReaderTextStyle readerTextStyle(uint8_t fontSizeLevel) {
  static constexpr ReaderTextStyle kStyles[] = {
      {100, kPhantomCurrentGapLarge, kPhantomAlphaLarge},
      {70, kPhantomCurrentGapMedium, kPhantomAlphaMedium},
      {50, kPhantomCurrentGapSmall, kPhantomAlphaSmall},
  };

  const size_t styleCount = sizeof(kStyles) / sizeof(kStyles[0]);
  if (fontSizeLevel >= styleCount) {
    fontSizeLevel = 0;
  }
  return kStyles[fontSizeLevel];
}

int orpOrdinalForLength(int length) {
  if (length <= 1) {
    return 0;
  }
  if (length <= 5) {
    return 1;
  }
  if (length <= 9) {
    return 2;
  }
  if (length <= 13) {
    return 3;
  }
  return 4;
}

int findFocusLetterIndex(const String &word) {
  int wordCharacterCount = 0;
  for (size_t i = 0; i < word.length(); ++i) {
    if (isWordCharacter(word[i])) {
      ++wordCharacterCount;
    }
  }

  if (wordCharacterCount == 0) {
    return word.length() > 0 ? 0 : -1;
  }

  const int targetOrdinal = std::min(orpOrdinalForLength(wordCharacterCount), wordCharacterCount - 1);
  int currentOrdinal = 0;
  for (size_t i = 0; i < word.length(); ++i) {
    if (!isWordCharacter(word[i])) {
      continue;
    }
    if (currentOrdinal == targetOrdinal) {
      return static_cast<int>(i);
    }
    ++currentOrdinal;
  }

  return 0;
}

int rsvpStartX(const String &word, int focusIndex, int virtualWidth, int divisor = 1,
               bool clampToMargins = true) {
  const TextLayoutMetrics layout = serifWordLayout(word, focusIndex, divisor);
  const int wordWidth = textLayoutWidth(layout);
  if (focusIndex < 0) {
    return ((virtualWidth - wordWidth) / 2) - layout.minX;
  }

  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
  const int x = anchorX - layout.focusCenterX;
  if (!clampToMargins) {
    return x;
  }
  const int minX = kRsvpSideMargin - layout.minX;
  const int maxX = virtualWidth - kRsvpSideMargin - layout.maxX;

  if (maxX < minX) {
    return x;
  }

  return std::max(minX, std::min(maxX, x));
}

int rsvpStartXScaledPercent(const String &word, int focusIndex, int virtualWidth,
                            uint8_t scalePercent, bool clampToMargins = true) {
  const TextLayoutMetrics layout = serifWordLayoutScaledPercent(word, focusIndex, scalePercent);
  const int wordWidth = textLayoutWidth(layout);
  if (focusIndex < 0) {
    return ((virtualWidth - wordWidth) / 2) - layout.minX;
  }

  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
  const int x = anchorX - layout.focusCenterX;
  if (!clampToMargins) {
    return x;
  }
  const int minX = kRsvpSideMargin - layout.minX;
  const int maxX = virtualWidth - kRsvpSideMargin - layout.maxX;

  if (maxX < minX) {
    return x;
  }

  return std::max(minX, std::min(maxX, x));
}

int serif70WordWidth(const String &word) { return textLayoutWidth(serif70WordLayout(word, -1)); }

int rsvpStartX70(const String &word, int focusIndex, int virtualWidth, bool clampToMargins = true) {
  const TextLayoutMetrics layout = serif70WordLayout(word, focusIndex);
  const int wordWidth = textLayoutWidth(layout);
  if (focusIndex < 0) {
    return ((virtualWidth - wordWidth) / 2) - layout.minX;
  }

  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
  const int x = anchorX - layout.focusCenterX;
  if (!clampToMargins) {
    return x;
  }

  const int minX = kRsvpSideMargin - layout.minX;
  const int maxX = virtualWidth - kRsvpSideMargin - layout.maxX;
  if (maxX < minX) {
    return x;
  }

  return std::max(minX, std::min(maxX, x));
}

}  // namespace

static const char *kDisplayTag = "display";

DisplayManager::~DisplayManager() {
  if (virtualFrame_ != nullptr) {
    heap_caps_free(virtualFrame_);
    virtualFrame_ = nullptr;
  }

  if (txBuffer_ != nullptr) {
    heap_caps_free(txBuffer_);
    txBuffer_ = nullptr;
  }
}

void DisplayManager::setBatteryLabel(const String &label) {
  if (batteryLabel_ == label) {
    return;
  }

  batteryLabel_ = label;
  lastRenderKey_ = "";
}

void DisplayManager::setCurrentWpm(uint16_t wpm) {
  if (currentWpm_ == wpm) return;
  currentWpm_ = wpm;
  lastRenderKey_ = "";
}

void DisplayManager::setWpmHighlightUntil(uint32_t deadlineMs) {
  wpmHighlightUntilMs_ = deadlineMs;
  lastRenderKey_ = "";
}

void DisplayManager::setBrightnessPercent(uint8_t percent) {
  if (percent == 0) {
    percent = 1;
  } else if (percent > 100) {
    percent = 100;
  }

  brightnessPercent_ = percent;
  if (initialized_) {
    applyBrightness();
  }
}

void DisplayManager::setDarkMode(bool darkMode) {
  if (darkMode_ == darkMode) {
    return;
  }

  darkMode_ = darkMode;
  lastRenderKey_ = "";
}

void DisplayManager::setNightMode(bool nightMode) {
  if (nightMode_ == nightMode) {
    return;
  }

  nightMode_ = nightMode;
  lastRenderKey_ = "";
}

void DisplayManager::setTypographyConfig(const TypographyConfig &config) {
  TypographyConfig next;
  next.trackingPx = static_cast<int8_t>(clampTypographyTracking(config.trackingPx));
  next.anchorPercent = static_cast<uint8_t>(clampTypographyAnchorPercent(config.anchorPercent));
  next.guideHalfWidth =
      static_cast<uint8_t>(clampTypographyGuideHalfWidth(config.guideHalfWidth));
  next.guideGap = static_cast<uint8_t>(clampTypographyGuideGap(config.guideGap));

  TypographyConfig &current = activeTypographyConfig();
  if (current.trackingPx == next.trackingPx && current.anchorPercent == next.anchorPercent &&
      current.guideHalfWidth == next.guideHalfWidth && current.guideGap == next.guideGap) {
    return;
  }

  current = next;
  lastRenderKey_ = "";
}

DisplayManager::TypographyConfig DisplayManager::typographyConfig() const {
  return activeTypographyConfig();
}

bool DisplayManager::darkMode() const { return darkMode_; }

bool DisplayManager::nightMode() const { return nightMode_; }

bool DisplayManager::begin() {
  ESP_LOGI(kDisplayTag, "Begin");

  if (!allocateBuffers()) {
    ESP_LOGE(kDisplayTag, "Buffer allocation failed");
    return false;
  }
  ESP_LOGI(kDisplayTag, "Buffers ready");

  if (!initPanel()) {
    ESP_LOGE(kDisplayTag, "Panel init failed");
    return false;
  }

  initialized_ = true;
  lastRenderKey_ = "";
  fillScreen(backgroundColor());
  applyBrightness();
  ESP_LOGI(kDisplayTag, "AXS15231B LCD initialized");
  return true;
}

void DisplayManager::prepareForSleep() {
  if (!initialized_) {
    return;
  }

  fillScreen(kTrueBlack);
  axs15231bSleep();
  initialized_ = false;
  lastRenderKey_ = "";
}

bool DisplayManager::wakeFromSleep() {
  if (!allocateBuffers()) {
    ESP_LOGE(kDisplayTag, "Buffer allocation failed after wake");
    return false;
  }

  axs15231bWake();
  initialized_ = true;
  lastRenderKey_ = "";
  applyBrightness();
  return true;
}

bool DisplayManager::allocateBuffers() {
  if (virtualFrame_ == nullptr) {
    virtualFrame_ = static_cast<uint16_t *>(
        heap_caps_malloc(kVirtualBufferWidth * kVirtualBufferHeight * sizeof(uint16_t),
                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (virtualFrame_ == nullptr) {
      virtualFrame_ = static_cast<uint16_t *>(
          heap_caps_malloc(kVirtualBufferWidth * kVirtualBufferHeight * sizeof(uint16_t),
                           MALLOC_CAP_8BIT));
    }
  }

  if (txBuffer_ == nullptr) {
    txBufferBytes_ = kTxBufferPixels * sizeof(uint16_t);
    txBuffer_ = static_cast<uint16_t *>(
        heap_caps_malloc(txBufferBytes_, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
  }

  return virtualFrame_ != nullptr && txBuffer_ != nullptr;
}

bool DisplayManager::initPanel() {
  axs15231bInit();
  ESP_LOGI(kDisplayTag, "Panel init sequence complete");
  return true;
}

bool DisplayManager::drawBitmap(int xStart, int yStart, int xEnd, int yEnd, const void *colorData) {
  if (colorData == nullptr || xEnd <= xStart || yEnd <= yStart) {
    return false;
  }

  axs15231bPushColors(static_cast<uint16_t>(xStart), static_cast<uint16_t>(yStart),
                      static_cast<uint16_t>(xEnd - xStart),
                      static_cast<uint16_t>(yEnd - yStart),
                      static_cast<const uint16_t *>(colorData));
  return true;
}

void DisplayManager::fillScreen(uint16_t color) {
  if (txBuffer_ == nullptr) {
    return;
  }

  const size_t pixelsPerChunk = static_cast<size_t>(kPanelNativeWidth) * kMaxChunkPhysicalRows;
  for (size_t i = 0; i < pixelsPerChunk; ++i) {
    txBuffer_[i] = panelColor(color);
  }

  for (int yStart = 0; yStart < kPanelNativeHeight; yStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - yStart);
    if (!drawBitmap(0, yStart, kPanelNativeWidth, yStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::clearVirtualBuffer(int width, int height) {
  const uint16_t background = panelColor(backgroundColor());
  for (int row = 0; row < height; ++row) {
    std::fill_n(virtualFrame_ + row * kVirtualBufferWidth, width, background);
  }
}

uint16_t DisplayManager::backgroundColor() const {
  if (nightMode_) {
    return kTrueBlack;
  }
  return darkMode_ ? kTrueBlack : kPureWhite;
}

uint16_t DisplayManager::wordColor() const {
  if (nightMode_) {
    return kNightWordColor;
  }
  return darkMode_ ? kDarkWordColor : kLightWordColor;
}

uint16_t DisplayManager::focusColor() const {
  if (nightMode_) {
    return kNightFocusColor;
  }
  return kFocusLetterColor;
}

uint16_t DisplayManager::dimColor() const {
  if (nightMode_) {
    return blendOverBackground(wordColor(), kNightDimAlpha);
  }
  return darkMode_ ? kDarkMenuDimColor : kLightMenuDimColor;
}

uint16_t DisplayManager::footerColor() const {
  if (nightMode_) {
    return blendOverBackground(wordColor(), kNightFooterAlpha);
  }
  return darkMode_ ? kDarkFooterColor : kLightFooterColor;
}

uint16_t DisplayManager::selectedBarColor() const {
  return nightMode_ ? focusColor() : kFocusLetterColor;
}

uint16_t DisplayManager::blendOverBackground(uint16_t rgb565, uint8_t alpha) const {
  if (alpha >= 250) {
    return rgb565;
  }

  const uint16_t bg = backgroundColor();
  const uint32_t inverseAlpha = 255U - alpha;
  const uint32_t r =
      ((((rgb565 >> 11) & 0x1F) * alpha) + (((bg >> 11) & 0x1F) * inverseAlpha)) / 255U;
  const uint32_t g =
      ((((rgb565 >> 5) & 0x3F) * alpha) + (((bg >> 5) & 0x3F) * inverseAlpha)) / 255U;
  const uint32_t b = (((rgb565 & 0x1F) * alpha) + ((bg & 0x1F) * inverseAlpha)) / 255U;
  return static_cast<uint16_t>((r << 11) | (g << 5) | b);
}

int DisplayManager::chooseTextScale(const String &word) const {
  const int usableWidth = std::max(1, measureTextWidth(word));
  const int maxScaleX = kDisplayWidth / usableWidth;
  const int maxScaleY = kDisplayHeight / kBaseGlyphHeight;
  const int maxScale = std::min(kMaxTextScale, std::min(maxScaleX, maxScaleY));
  return std::max(1, maxScale);
}

int DisplayManager::measureTextWidth(const String &word) const {
  return textLayoutWidth(serifWordLayout(word, -1));
}

int DisplayManager::measureSerifTextWidth(const String &text, int divisor) const {
  return textLayoutWidth(serifWordLayout(text, -1, divisor));
}

int DisplayManager::measureSerif70TextWidth(const String &text) const {
  return textLayoutWidth(serif70WordLayout(text, -1));
}

int DisplayManager::measureSerifTextWidthScaled(const String &text, uint8_t scalePercent) const {
  return textLayoutWidth(serifWordLayoutScaledPercent(text, -1, scalePercent));
}

int DisplayManager::measureTinyTextWidth(const String &text, int scale) const {
  if (text.isEmpty()) {
    return 0;
  }
  return static_cast<int>(text.length()) * (kTinyGlyphWidth + kTinyGlyphSpacing) * scale -
         kTinyGlyphSpacing * scale;
}

String DisplayManager::fitSerifText(const String &text, int maxWidth, int divisor) const {
  if (measureSerifTextWidth(text, divisor) <= maxWidth) {
    return text;
  }

  String fitted = text;
  const String ellipsis = "...";
  while (!fitted.isEmpty() && measureSerifTextWidth(fitted + ellipsis, divisor) > maxWidth) {
    fitted.remove(fitted.length() - 1);
  }
  fitted.trim();
  return fitted.isEmpty() ? ellipsis : fitted + ellipsis;
}

String DisplayManager::fitTinyText(const String &text, int maxWidth, int scale) const {
  if (measureTinyTextWidth(text, scale) <= maxWidth) {
    return text;
  }

  String fitted = text;
  const String ellipsis = "...";
  while (!fitted.isEmpty() && measureTinyTextWidth(fitted + ellipsis, scale) > maxWidth) {
    fitted.remove(fitted.length() - 1);
  }
  fitted.trim();
  return fitted.isEmpty() ? ellipsis : fitted + ellipsis;
}

void DisplayManager::drawGlyph(int x, int y, char c, uint16_t color) {
  const EmbeddedSerifGlyph &glyph = glyphFor(c);
  if (glyph.width == 0) {
    return;
  }

  const uint8_t *bitmap = kEmbeddedSerifBitmaps + glyph.bitmapOffset;
  for (int row = 0; row < kBaseGlyphHeight; ++row) {
    const int dstY = y + row;
    if (dstY < 0 || dstY >= kVirtualBufferHeight) {
      continue;
    }

    for (int col = 0; col < glyph.width; ++col) {
      const int dstX = x + col;
      if (dstX < 0 || dstX >= kVirtualBufferWidth) {
        continue;
      }

      const uint8_t alpha = bitmap[row * glyph.width + col];
      if (alpha < kGlyphAlphaThreshold) {
        continue;
      }

      virtualFrame_[dstY * kVirtualBufferWidth + dstX] =
          panelColor(blendOverBackground(color, alpha));
    }
  }
}

void DisplayManager::drawSerifGlyphScaled(int x, int y, char c, uint16_t color, int divisor) {
  divisor = std::max(1, divisor);
  const EmbeddedSerifGlyph &glyph = glyphFor(c);
  if (glyph.width == 0) {
    return;
  }

  const uint8_t *bitmap = kEmbeddedSerifBitmaps + glyph.bitmapOffset;
  const int scaledWidth = std::max(1, (glyph.width + divisor - 1) / divisor);
  const int scaledHeight = std::max(1, (kBaseGlyphHeight + divisor - 1) / divisor);

  for (int dstRow = 0; dstRow < scaledHeight; ++dstRow) {
    const int dstY = y + dstRow;
    if (dstY < 0 || dstY >= kVirtualBufferHeight) {
      continue;
    }

    const int sourceYStart = dstRow * divisor;
    const int sourceYEnd = std::min(kBaseGlyphHeight, sourceYStart + divisor);
    for (int dstCol = 0; dstCol < scaledWidth; ++dstCol) {
      const int dstX = x + dstCol;
      if (dstX < 0 || dstX >= kVirtualBufferWidth) {
        continue;
      }

      const int sourceXStart = dstCol * divisor;
      const int sourceXEnd = std::min(static_cast<int>(glyph.width), sourceXStart + divisor);
      uint32_t alphaSum = 0;
      uint32_t sampleCount = 0;
      for (int sourceY = sourceYStart; sourceY < sourceYEnd; ++sourceY) {
        for (int sourceX = sourceXStart; sourceX < sourceXEnd; ++sourceX) {
          alphaSum += bitmap[sourceY * glyph.width + sourceX];
          ++sampleCount;
        }
      }

      const uint8_t alpha =
          sampleCount == 0 ? 0 : static_cast<uint8_t>(alphaSum / sampleCount);
      if (alpha < kGlyphAlphaThreshold) {
        continue;
      }

      virtualFrame_[dstY * kVirtualBufferWidth + dstX] =
          panelColor(blendOverBackground(color, alpha));
    }
  }
}

void DisplayManager::drawSerif70Glyph(int x, int y, char c, uint16_t color) {
  const EmbeddedSerif70Glyph &glyph = glyph70For(c);
  if (glyph.width == 0) {
    return;
  }

  const uint8_t *bitmap = kEmbeddedSerif70Bitmaps + glyph.bitmapOffset;
  for (int row = 0; row < kEmbeddedSerif70Height; ++row) {
    const int dstY = y + row;
    if (dstY < 0 || dstY >= kVirtualBufferHeight) {
      continue;
    }

    for (int col = 0; col < glyph.width; ++col) {
      const int dstX = x + col;
      if (dstX < 0 || dstX >= kVirtualBufferWidth) {
        continue;
      }

      const uint8_t alpha = bitmap[row * glyph.width + col];
      if (alpha < kGlyphAlphaThreshold) {
        continue;
      }

      virtualFrame_[dstY * kVirtualBufferWidth + dstX] =
          panelColor(blendOverBackground(color, alpha));
    }
  }
}

void DisplayManager::drawSerifGlyphScaledPercent(int x, int y, char c, uint16_t color,
                                                 uint8_t scalePercent) {
  if (scalePercent >= 100) {
    drawGlyph(x, y, c, color);
    return;
  }

  const EmbeddedSerifGlyph &glyph = glyphFor(c);
  if (glyph.width == 0) {
    return;
  }

  const uint8_t *bitmap = kEmbeddedSerifBitmaps + glyph.bitmapOffset;
  const int scaledWidth = scaledPercentDimension(glyph.width, scalePercent);
  const int scaledHeight = scaledPercentDimension(kBaseGlyphHeight, scalePercent);

  for (int dstRow = 0; dstRow < scaledHeight; ++dstRow) {
    const int dstY = y + dstRow;
    if (dstY < 0 || dstY >= kVirtualBufferHeight) {
      continue;
    }

    const int sourceYStart = (dstRow * kBaseGlyphHeight) / scaledHeight;
    const int sourceYEnd =
        std::min(kBaseGlyphHeight, ((dstRow + 1) * kBaseGlyphHeight + scaledHeight - 1) / scaledHeight);
    for (int dstCol = 0; dstCol < scaledWidth; ++dstCol) {
      const int dstX = x + dstCol;
      if (dstX < 0 || dstX >= kVirtualBufferWidth) {
        continue;
      }

      const int sourceXStart = (dstCol * glyph.width) / scaledWidth;
      const int sourceXEnd =
          std::min(static_cast<int>(glyph.width),
                   ((dstCol + 1) * glyph.width + scaledWidth - 1) / scaledWidth);
      uint32_t alphaSum = 0;
      uint32_t sampleCount = 0;
      for (int sourceY = sourceYStart; sourceY < sourceYEnd; ++sourceY) {
        for (int sourceX = sourceXStart; sourceX < sourceXEnd; ++sourceX) {
          alphaSum += bitmap[sourceY * glyph.width + sourceX];
          ++sampleCount;
        }
      }

      const uint8_t alpha =
          sampleCount == 0 ? 0 : static_cast<uint8_t>(alphaSum / sampleCount);
      if (alpha < kGlyphAlphaThreshold) {
        continue;
      }

      virtualFrame_[dstY * kVirtualBufferWidth + dstX] =
          panelColor(blendOverBackground(color, alpha));
    }
  }
}

void DisplayManager::fillRoundedRect(int x, int y, int width, int height, int radius,
                                     uint16_t color) {
  if (width <= 0 || height <= 0) return;
  if (radius < 1) {
    fillVirtualRect(x, y, width, height, color);
    return;
  }
  const int r = std::min(radius, std::min(width, height) / 2);
  fillVirtualRect(x + r, y, width - 2 * r, height, color);
  fillVirtualRect(x, y + r, r, height - 2 * r, color);
  fillVirtualRect(x + width - r, y + r, r, height - 2 * r, color);
  const int r2 = r * r;
  for (int dy = 0; dy < r; ++dy) {
    const int distY = r - dy;
    int firstX = r;
    for (int dx = 0; dx < r; ++dx) {
      const int distX = r - dx;
      if (distX * distX + distY * distY <= r2) {
        firstX = dx;
        break;
      }
    }
    const int span = r - firstX;
    if (span <= 0) continue;
    fillVirtualRect(x + firstX, y + dy, span, 1, color);
    fillVirtualRect(x + width - firstX - span, y + dy, span, 1, color);
    fillVirtualRect(x + firstX, y + height - 1 - dy, span, 1, color);
    fillVirtualRect(x + width - firstX - span, y + height - 1 - dy, span, 1, color);
  }
}

void DisplayManager::fillVirtualRect(int x, int y, int width, int height, uint16_t color) {
  const uint16_t panel = panelColor(color);
  const int xEnd = std::min(kVirtualBufferWidth, x + width);
  const int yEnd = std::min(kVirtualBufferHeight, y + height);
  x = std::max(0, x);
  y = std::max(0, y);

  for (int row = y; row < yEnd; ++row) {
    for (int col = x; col < xEnd; ++col) {
      virtualFrame_[row * kVirtualBufferWidth + col] = panel;
    }
  }
}

void DisplayManager::drawSerifTextAt(const String &text, int x, int y, uint16_t color,
                                     int divisor) {
  divisor = std::max(1, divisor);
  int cursorX = x;
  for (size_t i = 0; i < text.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(text[i]);
    const int xOffset = scaledSignedAdvance(static_cast<int>(glyph.xOffset), divisor);
    const int width = glyph.width == 0 ? 0 : scaledAdvance(static_cast<int>(glyph.width), divisor);
    drawSerifGlyphScaled(cursorX + xOffset, y, text[i], color, divisor);
    int tracked = trackedAdvanceScaled(static_cast<int>(glyph.xAdvance), divisor, i, text.length());
    if (i + 1 < text.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(text[i + 1]);
      tracked -= opticalKerningAdjustment(
          text[i], text[i + 1], xOffset, width, tracked,
          scaledSignedAdvance(static_cast<int>(nextGlyph.xOffset), divisor), scaledDesiredGap(divisor));
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawSerif70TextAt(const String &text, int x, int y, uint16_t color) {
  int cursorX = x;
  for (size_t i = 0; i < text.length(); ++i) {
    const EmbeddedSerif70Glyph &glyph = glyph70For(text[i]);
    drawSerif70Glyph(cursorX + static_cast<int>(glyph.xOffset), y, text[i], color);
    int tracked = trackedAdvance(static_cast<int>(glyph.xAdvance), i, text.length());
    if (i + 1 < text.length()) {
      const EmbeddedSerif70Glyph &nextGlyph = glyph70For(text[i + 1]);
      tracked -= opticalKerningAdjustment(text[i], text[i + 1], static_cast<int>(glyph.xOffset),
                                          static_cast<int>(glyph.width), tracked,
                                          static_cast<int>(nextGlyph.xOffset), regularDesiredGap());
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawSerifTextScaledAt(const String &text, int x, int y, uint16_t color,
                                           uint8_t scalePercent) {
  int cursorX = x;
  for (size_t i = 0; i < text.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(text[i]);
    const int xOffset = scaledSignedPercent(static_cast<int>(glyph.xOffset), scalePercent);
    const int width = glyph.width == 0
                          ? 0
                          : scaledPercentDimension(static_cast<int>(glyph.width), scalePercent);
    drawSerifGlyphScaledPercent(cursorX + xOffset, y, text[i], color, scalePercent);
    int tracked =
        trackedAdvanceScaledPercent(static_cast<int>(glyph.xAdvance), scalePercent, i, text.length());
    if (i + 1 < text.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(text[i + 1]);
      tracked -= opticalKerningAdjustment(
          text[i], text[i + 1], xOffset, width, tracked,
          scaledSignedPercent(static_cast<int>(nextGlyph.xOffset), scalePercent),
          scaledPercentDesiredGap(scalePercent));
    }
    cursorX += std::max(1, tracked);
  }
}

namespace {

uint16_t blendRgb565(uint16_t fg, uint16_t bg, uint8_t alpha) {
  if (alpha >= 250) return fg;
  if (alpha == 0) return bg;
  const uint32_t inv = 255U - alpha;
  const uint32_t r = ((((fg >> 11) & 0x1FU) * alpha) + (((bg >> 11) & 0x1FU) * inv)) / 255U;
  const uint32_t g = ((((fg >> 5) & 0x3FU) * alpha) + (((bg >> 5) & 0x3FU) * inv)) / 255U;
  const uint32_t b = (((fg & 0x1FU) * alpha) + ((bg & 0x1FU) * inv)) / 255U;
  return static_cast<uint16_t>((r << 11) | (g << 5) | b);
}

}  // namespace

void DisplayManager::drawTinyMarquee(const String &text, int leftX, int rightX, int textY,
                                     uint16_t color, uint16_t fadeColor) {
  if (text.isEmpty() || rightX <= leftX) return;
  const int maxWidth = rightX - leftX;
  const int textWidth = measureTinyTextWidth(text, kTinyScale);
  if (textWidth <= maxWidth) {
    drawTinyTextAt(text, leftX, textY, color, kTinyScale);
    return;
  }
  const int charPitch = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;
  const int charBodyWidth = kTinyGlyphWidth * kTinyScale;
  const int maxOffset = textWidth - maxWidth;
  const int offsetPx = marqueePingPongOffset(maxOffset);
  const int fadeWidthPx = std::min<int>(10, maxWidth / 5);
  for (size_t ci = 0; ci < text.length(); ++ci) {
    const int charX = leftX - offsetPx + static_cast<int>(ci) * charPitch;
    const int charRight = charX + charBodyWidth;
    if (charX >= rightX) break;
    if (charRight <= leftX) continue;
    drawTinyGlyphFaded(charX, textY, text[ci], color, kTinyScale,
                       leftX, rightX, fadeWidthPx, fadeColor);
  }
}

void DisplayManager::drawTinyGlyphFaded(int x, int y, char c, uint16_t color, int scale,
                                        int clipLeftX, int clipRightX, int fadeWidth,
                                        uint16_t fadeColor) {
  const uint8_t *rows = tinyRowsFor(c);

  // Pre-compute per-column alpha within the fade zones so we don't redo the
  // math for every glyph row. Outside the clip window the column is skipped
  // entirely.
  for (int row = 0; row < kTinyGlyphHeight; ++row) {
    for (int col = 0; col < kTinyGlyphWidth; ++col) {
      if ((rows[row] & (1 << (kTinyGlyphWidth - 1 - col))) == 0) continue;
      for (int yy = 0; yy < scale; ++yy) {
        const int dstY = y + row * scale + yy;
        if (dstY < 0 || dstY >= kVirtualBufferHeight) continue;
        for (int xx = 0; xx < scale; ++xx) {
          const int dstX = x + col * scale + xx;
          if (dstX < 0 || dstX >= kVirtualBufferWidth) continue;
          if (dstX < clipLeftX || dstX >= clipRightX) continue;

          uint8_t alpha = 255;
          if (fadeWidth > 0) {
            const int distLeft = dstX - clipLeftX;
            const int distRight = clipRightX - 1 - dstX;
            const int edge = std::min(distLeft, distRight);
            if (edge < fadeWidth) {
              const int a = (edge * 255) / std::max(1, fadeWidth);
              alpha = static_cast<uint8_t>(std::max(0, std::min(255, a)));
            }
          }
          const uint16_t blended = blendRgb565(color, fadeColor, alpha);
          virtualFrame_[dstY * kVirtualBufferWidth + dstX] = panelColor(blended);
        }
      }
    }
  }
}

void DisplayManager::drawTinyGlyphClipped(int x, int y, char c, uint16_t color, int scale,
                                          int clipLeftX, int clipRightX) {
  const uint8_t *rows = tinyRowsFor(c);
  const uint16_t panel = panelColor(color);

  for (int row = 0; row < kTinyGlyphHeight; ++row) {
    for (int col = 0; col < kTinyGlyphWidth; ++col) {
      if ((rows[row] & (1 << (kTinyGlyphWidth - 1 - col))) == 0) {
        continue;
      }
      for (int yy = 0; yy < scale; ++yy) {
        const int dstY = y + row * scale + yy;
        if (dstY < 0 || dstY >= kVirtualBufferHeight) continue;
        for (int xx = 0; xx < scale; ++xx) {
          const int dstX = x + col * scale + xx;
          if (dstX < 0 || dstX >= kVirtualBufferWidth) continue;
          if (dstX < clipLeftX || dstX >= clipRightX) continue;
          virtualFrame_[dstY * kVirtualBufferWidth + dstX] = panel;
        }
      }
    }
  }
}

void DisplayManager::drawTinyGlyph(int x, int y, char c, uint16_t color, int scale) {
  const uint8_t *rows = tinyRowsFor(c);
  const uint16_t panel = panelColor(color);

  for (int row = 0; row < kTinyGlyphHeight; ++row) {
    for (int col = 0; col < kTinyGlyphWidth; ++col) {
      if ((rows[row] & (1 << (kTinyGlyphWidth - 1 - col))) == 0) {
        continue;
      }

      for (int yy = 0; yy < scale; ++yy) {
        const int dstY = y + row * scale + yy;
        if (dstY < 0 || dstY >= kVirtualBufferHeight) {
          continue;
        }

        for (int xx = 0; xx < scale; ++xx) {
          const int dstX = x + col * scale + xx;
          if (dstX < 0 || dstX >= kVirtualBufferWidth) {
            continue;
          }
          virtualFrame_[dstY * kVirtualBufferWidth + dstX] = panel;
        }
      }
    }
  }
}

void DisplayManager::drawTinyTextAt(const String &text, int x, int y, uint16_t color, int scale) {
  int cursorX = x;
  for (size_t i = 0; i < text.length(); ++i) {
    drawTinyGlyph(cursorX, y, text[i], color, scale);
    cursorX += (kTinyGlyphWidth + kTinyGlyphSpacing) * scale;
  }
}

void DisplayManager::drawTinyTextCentered(const String &text, int y, uint16_t color, int scale) {
  const int textWidth = measureTinyTextWidth(text, scale);
  drawTinyTextAt(text, std::max(0, (kVirtualBufferWidth - textWidth) / 2), y, color, scale);
}

int DisplayManager::drawScrollingChipText(const String &text, int leftX, int textY,
                                          int maxChipWidth, uint16_t textColor,
                                          uint16_t bgColor) {
  if (text.isEmpty()) return leftX;
  const int chipPadX = 5;
  const int chipPadY = 2;
  const int chipH = kTinyGlyphHeight * kTinyScale + chipPadY * 2;
  const int textW = measureTinyTextWidth(text, kTinyScale);
  const int chipMaxW = std::max(chipPadX * 2 + 8, maxChipWidth);
  const int contentMaxW = chipMaxW - chipPadX * 2;
  if (textW <= contentMaxW) {
    return drawChipText(text, leftX, textY, textColor, bgColor);
  }

  const int chipW = chipMaxW;
  const int chipY = textY - chipPadY;
  fillRoundedRect(leftX, chipY, chipW, chipH, std::min(6, chipH / 2), bgColor);

  const int contentLeft = leftX + chipPadX;
  const int contentRight = leftX + chipW - chipPadX;
  const int charPitch = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;
  const int charBodyWidth = kTinyGlyphWidth * kTinyScale;
  const int maxOffset = textW - (contentRight - contentLeft);
  const int offsetPx = marqueePingPongOffset(maxOffset);
  // Soft edge fade — text dissolves into the chip's own fill color so the
  // scroll feels continuous instead of clipping abruptly.
  const int fadeWidthPx = std::min<int>(8, (contentRight - contentLeft) / 4);
  for (size_t i = 0; i < text.length(); ++i) {
    const int charX = contentLeft - offsetPx + static_cast<int>(i) * charPitch;
    const int charRight = charX + charBodyWidth;
    if (charX >= contentRight) break;
    if (charRight <= contentLeft) continue;
    drawTinyGlyphFaded(charX, textY, text[i], textColor, kTinyScale, contentLeft, contentRight,
                       fadeWidthPx, bgColor);
  }
  lastRenderKey_ = "";
  return leftX + chipW;
}

int DisplayManager::drawChipText(const String &text, int leftX, int textY, uint16_t textColor,
                                 uint16_t bgColor, bool rightAlign, int rightX) {
  if (text.isEmpty()) return rightAlign ? rightX : leftX;
  const int chipPadX = 5;
  const int chipPadY = 2;
  const int textWidth = measureTinyTextWidth(text, kTinyScale);
  const int chipW = textWidth + chipPadX * 2;
  const int chipH = kTinyGlyphHeight * kTinyScale + chipPadY * 2;
  const int chipX = rightAlign ? (rightX - chipW) : leftX;
  const int chipY = textY - chipPadY;
  fillRoundedRect(chipX, chipY, chipW, chipH, std::min(6, chipH / 2), bgColor);
  drawTinyTextAt(text, chipX + chipPadX, textY, textColor, kTinyScale);
  return rightAlign ? chipX : (chipX + chipW);
}

void DisplayManager::drawBatteryBadge(bool leftAlign) {
  if (batteryLabel_.isEmpty()) {
    return;
  }
  const uint16_t chipBg = blendOverBackground(footerColor(), kLibraryChipBgAlpha);
  if (leftAlign) {
    drawChipText(batteryLabel_, kFooterMarginX, kFooterMarginBottom, footerColor(), chipBg);
  } else {
    const int rightInset = kFooterMarginX + kLibraryLetterStripWidth;
    drawChipText(batteryLabel_, 0, kFooterMarginBottom, footerColor(), chipBg, true,
                 kDisplayWidth - rightInset);
  }
}

void DisplayManager::drawFooter(const String &chapterLabel, uint8_t progressPercent) {
  const int textY = kDisplayHeight - kTinyGlyphHeight * kTinyScale - kFooterMarginBottom;
  const String chapter = chapterLabel.isEmpty() ? String("START") : chapterLabel;
  const uint16_t chipBg = blendOverBackground(footerColor(), kLibraryChipBgAlpha);

  // Reserve room for the WPM chip on the right (if any) so the chapter chip
  // can stretch across the rest of the bottom row.
  int wpmChipReserve = 0;
  if (currentWpm_ > 0) {
    const String wpmLabel = String(currentWpm_) + "wpm";
    const int wpmTextWidth = measureTinyTextWidth(wpmLabel, kTinyScale);
    const int wpmChipPadX = 5;
    wpmChipReserve = wpmTextWidth + wpmChipPadX * 2 + kFooterMarginX + 8;
  }
  const int maxChapterChipWidth =
      std::max(0, kDisplayWidth - kFooterMarginX - wpmChipReserve);
  drawScrollingChipText(chapter, kFooterMarginX, textY, maxChapterChipWidth, footerColor(),
                        chipBg);

  if (currentWpm_ > 0) {
    const String wpmLabel = String(currentWpm_) + "wpm";
    const uint32_t now = millis();
    const bool highlighted = now < wpmHighlightUntilMs_;
    // Inverted-pill look while highlighted: solid focus colour fill + background
    // colour text so it pops against any other chip on screen.
    const uint16_t wpmTextColor = highlighted ? backgroundColor() : footerColor();
    const uint16_t wpmChipBg = highlighted ? focusColor() : chipBg;
    drawChipText(wpmLabel, 0, textY, wpmTextColor, wpmChipBg, true,
                 kDisplayWidth - kFooterMarginX);
    if (highlighted) {
      // Force the next render to redraw so we revert when the deadline lapses.
      lastRenderKey_ = "";
    }
  }

  const int batteryWidth =
      batteryLabel_.isEmpty() ? 0 : measureTinyTextWidth(batteryLabel_, kTinyScale);
  const int barHeight = 3;
  const int barY = kFooterMarginBottom + (kTinyGlyphHeight * kTinyScale - barHeight) / 2;
  const int barLeft = kFooterMarginX + batteryWidth + (batteryWidth > 0 ? 12 : 0);
  const int barRight = kDisplayWidth - kFooterMarginX - kLibraryLetterStripWidth;
  const int barWidth = std::max(0, barRight - barLeft);
  if (barWidth <= 0) {
    return;
  }
  const uint16_t trackColor = blendOverBackground(footerColor(), 60);
  const uint16_t fillColor = focusColor();
  const uint16_t tickColor = blendOverBackground(footerColor(), 200);

  fillVirtualRect(barLeft, barY, barWidth, barHeight, trackColor);
  const int filledWidth =
      std::max(0, std::min(barWidth, (barWidth * static_cast<int>(progressPercent)) / 100));
  if (filledWidth > 0) {
    fillVirtualRect(barLeft, barY, filledWidth, barHeight, fillColor);
  }
  for (float fraction : chapterFractions_) {
    if (fraction <= 0.0f || fraction >= 1.0f) continue;
    const int tickX = barLeft + static_cast<int>(fraction * barWidth);
    fillVirtualRect(tickX, barY - 1, 1, barHeight + 2, tickColor);
  }
}

void DisplayManager::setChapterFractions(const std::vector<float> &fractions) {
  if (chapterFractions_ == fractions) {
    return;
  }
  chapterFractions_ = fractions;
  lastRenderKey_ = "";
}

void DisplayManager::setUiRotated(bool rotated) {
  if (uiRotated_ == rotated) return;
  uiRotated_ = rotated;
  lastRenderKey_ = "";
}

bool DisplayManager::uiRotated() const { return uiRotated_; }

void DisplayManager::drawRsvpAnchorGuide(int anchorX, int textY, int textHeight) {
  const int topY = std::max(2, textY - kRsvpGuideTopOffset);
  const int bottomY = std::min(kVirtualBufferHeight - 3, textY + textHeight + kRsvpGuideBottomOffset);
  const int guideHalfWidth = currentGuideHalfWidth();
  const int guideGap = currentGuideGap();
  const int leftX = std::max(0, anchorX - guideHalfWidth);
  const int rightX = std::min(kVirtualBufferWidth - 1, anchorX + guideHalfWidth);
  const int leftWidth = std::max(0, (anchorX - guideGap) - leftX);
  const int rightWidth = std::max(0, rightX - (anchorX + guideGap) + 1);
  const uint16_t guideColor = blendOverBackground(wordColor(), nightMode_ ? 136 : 96);

  fillVirtualRect(leftX, topY, leftWidth, 1, guideColor);
  fillVirtualRect(anchorX + guideGap, topY, rightWidth, 1, guideColor);
  fillVirtualRect(leftX, bottomY, leftWidth, 1, guideColor);
  fillVirtualRect(anchorX + guideGap, bottomY, rightWidth, 1, guideColor);
  fillVirtualRect(anchorX, topY, 1, kRsvpGuideTickHeight, focusColor());
  fillVirtualRect(anchorX, bottomY - kRsvpGuideTickHeight + 1, 1, kRsvpGuideTickHeight,
                  focusColor());
}

void DisplayManager::drawWordAt(const String &word, int x, int y, uint16_t color) {
  int cursorX = x;
  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(word[i]);
    drawGlyph(cursorX + static_cast<int>(glyph.xOffset), y, word[i], color);
    int tracked = trackedAdvance(static_cast<int>(glyph.xAdvance), i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(word[i + 1]);
      tracked -= opticalKerningAdjustment(word[i], word[i + 1], static_cast<int>(glyph.xOffset),
                                          static_cast<int>(glyph.width), tracked,
                                          static_cast<int>(nextGlyph.xOffset), regularDesiredGap());
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawRsvpWordScaledAt(const String &word, int x, int y, int focusIndex,
                                          int divisor) {
  divisor = std::max(1, divisor);
  int cursorX = x;
  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(word[i]);
    const uint16_t color = (static_cast<int>(i) == focusIndex) ? focusColor() : wordColor();
    const int xOffset = scaledSignedAdvance(static_cast<int>(glyph.xOffset), divisor);
    const int width =
        glyph.width == 0 ? 0 : scaledAdvance(static_cast<int>(glyph.width), divisor);
    drawSerifGlyphScaled(cursorX + xOffset, y, word[i], color, divisor);
    int tracked = trackedAdvanceScaled(static_cast<int>(glyph.xAdvance), divisor, i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(word[i + 1]);
      tracked -= opticalKerningAdjustment(
          word[i], word[i + 1], xOffset, width, tracked,
          scaledSignedAdvance(static_cast<int>(nextGlyph.xOffset), divisor), scaledDesiredGap(divisor));
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawRsvp70WordAt(const String &word, int x, int y, int focusIndex) {
  int cursorX = x;
  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerif70Glyph &glyph = glyph70For(word[i]);
    const uint16_t color = (static_cast<int>(i) == focusIndex) ? focusColor() : wordColor();
    drawSerif70Glyph(cursorX + static_cast<int>(glyph.xOffset), y, word[i], color);
    int tracked = trackedAdvance(static_cast<int>(glyph.xAdvance), i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerif70Glyph &nextGlyph = glyph70For(word[i + 1]);
      tracked -= opticalKerningAdjustment(word[i], word[i + 1], static_cast<int>(glyph.xOffset),
                                          static_cast<int>(glyph.width), tracked,
                                          static_cast<int>(nextGlyph.xOffset), regularDesiredGap());
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawRsvpWordScaledPercentAt(const String &word, int x, int y, int focusIndex,
                                                 uint8_t scalePercent) {
  int cursorX = x;
  for (size_t i = 0; i < word.length(); ++i) {
    const EmbeddedSerifGlyph &glyph = glyphFor(word[i]);
    const uint16_t color = (static_cast<int>(i) == focusIndex) ? focusColor() : wordColor();
    const int xOffset = scaledSignedPercent(static_cast<int>(glyph.xOffset), scalePercent);
    const int width = glyph.width == 0
                          ? 0
                          : scaledPercentDimension(static_cast<int>(glyph.width), scalePercent);
    drawSerifGlyphScaledPercent(cursorX + xOffset, y, word[i], color, scalePercent);
    int tracked =
        trackedAdvanceScaledPercent(static_cast<int>(glyph.xAdvance), scalePercent, i, word.length());
    if (i + 1 < word.length()) {
      const EmbeddedSerifGlyph &nextGlyph = glyphFor(word[i + 1]);
      tracked -= opticalKerningAdjustment(
          word[i], word[i + 1], xOffset, width, tracked,
          scaledSignedPercent(static_cast<int>(nextGlyph.xOffset), scalePercent),
          scaledPercentDesiredGap(scalePercent));
    }
    cursorX += std::max(1, tracked);
  }
}

void DisplayManager::drawRsvpWordAt(const String &word, int x, int y, int focusIndex) {
  drawRsvpWordScaledAt(word, x, y, focusIndex, 1);
}

void DisplayManager::drawWordLine(const String &word, int y, uint16_t color) {
  const TextLayoutMetrics layout = serifWordLayout(word, -1);
  const int textWidth = textLayoutWidth(layout);
  const int x = std::max(0, ((kVirtualBufferWidth - textWidth) / 2) - layout.minX);
  drawWordAt(word, x, y, color);
}

void DisplayManager::drawMenuItem(const String &item, int y, bool selected) {
  drawWordLine(item, y, selected ? focusColor() : dimColor());
}

void DisplayManager::applyBrightness() {
  axs15231bSetBrightnessPercent(brightnessPercent_);
  axs15231bSetBacklight(true);
}

void DisplayManager::flushScaledFrame(int scale, int virtualWidth, int virtualHeight) {
  // Diagnostic: log the start (and later the duration) of every flush. Helps
  // identify which renderer wedges the device when the marquee triggers a
  // continuous re-render storm.
  const uint32_t flushStartMs = millis();
  static uint32_t sFlushCount = 0;
  ++sFlushCount;
  Serial.printf("[flush] in #%lu w=%d h=%d scale=%d\n",
                static_cast<unsigned long>(sFlushCount),
                virtualWidth, virtualHeight, scale);
  // Fast path: rotated panel (the production orientation), scale=1, and the
  // virtual buffer is the full logical screen. We can hoist the rotation math,
  // drop the per-pixel bounds check, and reorder the inner loop so each PSRAM
  // read is sequential along a virtualFrame_ row instead of stepping
  // kVirtualBufferWidth bytes per pixel.
  const bool fastPath = (scale == 1 && uiRotated_ &&
                         virtualWidth == kDisplayWidth && virtualHeight == kDisplayHeight);

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  uint32_t composeUs = 0;
  uint32_t spiUs = 0;
#endif

  for (int nativeYStart = 0; nativeYStart < kPanelNativeHeight;
       nativeYStart += kMaxChunkPhysicalRows) {
    const int nativeRows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - nativeYStart);
    const size_t chunkBytes = static_cast<size_t>(nativeRows) * kPanelNativeWidth * sizeof(uint16_t);
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    const uint32_t cBegin = micros();
#endif
    std::memset(txBuffer_, 0, chunkBytes);

    if (fastPath) {
      // For each native column (= one virtualFrame_ row), copy `nativeRows`
      // sequential source pixels and stride them out into the chunk's column.
      for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
        const int logicalY = kDisplayHeight - 1 - nativeX;
        const uint16_t *src = virtualFrame_ + logicalY * kVirtualBufferWidth + nativeYStart;
        uint16_t *dst = txBuffer_ + nativeX;
        for (int localY = 0; localY < nativeRows; ++localY) {
          dst[localY * kPanelNativeWidth] = src[localY];
        }
      }
    } else {
      for (int localNativeY = 0; localNativeY < nativeRows; ++localNativeY) {
        const int nativeY = nativeYStart + localNativeY;
        uint16_t *dstRow = txBuffer_ + localNativeY * kPanelNativeWidth;

        for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
          int logicalX = kDisplayWidth - 1 - nativeY;
          int logicalY = nativeX;
          if (uiRotated_) {
            logicalX = nativeY;
            logicalY = kDisplayHeight - 1 - nativeX;
          }
          const int sourceX = logicalX / scale;
          const int sourceY = logicalY / scale;

          if (sourceX >= 0 && sourceX < virtualWidth && sourceY >= 0 && sourceY < virtualHeight) {
            dstRow[nativeX] = virtualFrame_[sourceY * kVirtualBufferWidth + sourceX];
          }
        }
      }
    }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    const uint32_t cEnd = micros();
    composeUs += cEnd - cBegin;
    const uint32_t sBegin = micros();
#endif
    if (!drawBitmap(0, nativeYStart, kPanelNativeWidth, nativeYStart + nativeRows, txBuffer_)) {
      return;
    }
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    spiUs += micros() - sBegin;
#endif
  }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  if (fastPath) {
    static uint32_t sLastLogMs = 0;
    if (millis() - sLastLogMs >= 2000) {
      sLastLogMs = millis();
      Serial.printf("[flush] compose=%lu spi=%lu us\n",
                    static_cast<unsigned long>(composeUs),
                    static_cast<unsigned long>(spiUs));
    }
  }
#else
  (void)fastPath;
#endif
  // Diagnostic: confirm we got back out of the flush. If "[flush] in" appears
  // without a matching "[flush] out", the lock-up is somewhere inside the
  // virtualFrame_ flush path (transpose + drawBitmap chunks).
  Serial.printf("[flush] out dt=%lu ms\n",
                static_cast<unsigned long>(millis() - flushStartMs));
}

void DisplayManager::renderCenteredWord(const String &word, uint16_t color) {
  String normalized = word;
  const uint16_t renderColor = (color == kPureWhite) ? wordColor() : color;
  const String renderKey = "center|" + normalized + "|" + String(renderColor) + "|b:" +
                           batteryLabel_ + "|d:" + String(darkMode_ ? 1 : 0) + "|n:" +
                           String(nightMode_ ? 1 : 0);

  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = chooseTextScale(normalized);
  const int virtualWidth = (kDisplayWidth + scale - 1) / scale;
  const int virtualHeight = (kDisplayHeight + scale - 1) / scale;

  clearVirtualBuffer(virtualWidth, virtualHeight);
  const int y = std::max(0, (virtualHeight - kBaseGlyphHeight) / 2);
  drawWordLine(normalized, y, renderColor);
  drawBatteryBadge(true);

  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderRsvpWord(const String &word, const String &chapterLabel,
                                    uint8_t progressPercent, bool showFooter) {
  const String renderKey =
      "rsvp|" + word + "|" + chapterLabel + "|" + String(progressPercent) + "|" +
      String(showFooter ? 1 : 0) + "|b:" + batteryLabel_ + "|d:" +
      String(darkMode_ ? 1 : 0) + "|n:" + String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int y = std::max(0, (virtualHeight - kBaseGlyphHeight) / 2);
  const int focusIndex = findFocusLetterIndex(word);
  const int x = rsvpStartX(word, focusIndex, virtualWidth, 1, false);
  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawRsvpAnchorGuide(anchorX, y, kBaseGlyphHeight);
  drawRsvpWordAt(word, x, y, focusIndex);
  if (showFooter) {
    drawFooter(chapterLabel, progressPercent);
  }
  drawBatteryBadge(true);
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderRsvpWordWithWpm(const String &word, uint16_t wpm,
                                           const String &chapterLabel, uint8_t progressPercent,
                                           bool showFooter) {
  const String wpmText = String(wpm) + " WPM";
  const String renderKey =
      "rsvp_wpm|" + word + "|" + wpmText + "|" + chapterLabel + "|" +
      String(progressPercent) + "|" + String(showFooter ? 1 : 0) + "|b:" + batteryLabel_ +
      "|d:" + String(darkMode_ ? 1 : 0) + "|n:" + String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int wordY = std::max(0, (virtualHeight - kBaseGlyphHeight) / 2);
  const int wpmY =
      std::max(0, virtualHeight - kTinyGlyphHeight * kTinyScale - kWpmFeedbackBottomMargin - 24);
  const int focusIndex = findFocusLetterIndex(word);
  const int x = rsvpStartX(word, focusIndex, virtualWidth, 1, false);
  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawRsvpAnchorGuide(anchorX, wordY, kBaseGlyphHeight);
  drawRsvpWordAt(word, x, wordY, focusIndex);
  drawTinyTextCentered(wpmText, wpmY, focusColor(), kTinyScale);
  if (showFooter) {
    drawFooter(chapterLabel, progressPercent);
  }
  drawBatteryBadge(true);
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderPhantomRsvpWord(const String &beforeText, const String &word,
                                           const String &afterText, uint8_t fontSizeLevel,
                                           const String &chapterLabel, uint8_t progressPercent,
                                           bool showFooter) {
  const String renderKey =
      "rsvp_phantom|" + beforeText + "|" + word + "|" + afterText + "|s:" +
      String(fontSizeLevel) + "|" + chapterLabel + "|" + String(progressPercent) + "|" +
      String(showFooter ? 1 : 0) + "|b:" + batteryLabel_ + "|d:" +
      String(darkMode_ ? 1 : 0) + "|n:" + String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  if (fontSizeLevel == 1) {
    const int scale = 1;
    const int virtualWidth = kDisplayWidth;
    const int virtualHeight = kDisplayHeight;
    const int textY = std::max(0, (virtualHeight - kEmbeddedSerif70Height) / 2);
    const int focusIndex = findFocusLetterIndex(word);
    const int currentX = rsvpStartX70(word, focusIndex, virtualWidth, false);
    const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
    const TextLayoutMetrics currentLayout = serif70WordLayout(word, focusIndex);
    const uint16_t phantomColor = blendOverBackground(wordColor(), kPhantomAlphaMedium);

    clearVirtualBuffer(virtualWidth, virtualHeight);
    drawRsvpAnchorGuide(anchorX, textY, kEmbeddedSerif70Height);
    if (!beforeText.isEmpty()) {
      const TextLayoutMetrics beforeLayout = serif70WordLayout(beforeText, -1);
      const int beforeX =
          currentX + currentLayout.minX - kPhantomCurrentGapMedium - beforeLayout.maxX;
      drawSerif70TextAt(beforeText, beforeX, textY, phantomColor);
    }
    drawRsvp70WordAt(word, currentX, textY, focusIndex);
    if (!afterText.isEmpty()) {
      const TextLayoutMetrics afterLayout = serif70WordLayout(afterText, -1);
      const int afterX =
          currentX + currentLayout.maxX + kPhantomCurrentGapMedium - afterLayout.minX;
      drawSerif70TextAt(afterText, afterX, textY, phantomColor);
    }
    if (showFooter) {
      drawFooter(chapterLabel, progressPercent);
    }
    drawBatteryBadge(true);
    flushScaledFrame(scale, virtualWidth, virtualHeight);
    return;
  }

  const ReaderTextStyle style = readerTextStyle(fontSizeLevel);
  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int textHeight = scaledPercentDimension(kBaseGlyphHeight, style.scalePercent);
  const int textY = std::max(0, (virtualHeight - textHeight) / 2);
  const int focusIndex = findFocusLetterIndex(word);
  const int currentX =
      rsvpStartXScaledPercent(word, focusIndex, virtualWidth, style.scalePercent, false);
  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
  const TextLayoutMetrics currentLayout =
      serifWordLayoutScaledPercent(word, focusIndex, style.scalePercent);
  const uint16_t phantomColor = blendOverBackground(wordColor(), style.alpha);

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawRsvpAnchorGuide(anchorX, textY, textHeight);
  if (!beforeText.isEmpty()) {
    const TextLayoutMetrics beforeLayout =
        serifWordLayoutScaledPercent(beforeText, -1, style.scalePercent);
    const int beforeX = currentX + currentLayout.minX - style.currentGap - beforeLayout.maxX;
    drawSerifTextScaledAt(beforeText, beforeX, textY, phantomColor, style.scalePercent);
  }
  drawRsvpWordScaledPercentAt(word, currentX, textY, focusIndex, style.scalePercent);
  if (!afterText.isEmpty()) {
    const TextLayoutMetrics afterLayout =
        serifWordLayoutScaledPercent(afterText, -1, style.scalePercent);
    const int afterX = currentX + currentLayout.maxX + style.currentGap - afterLayout.minX;
    drawSerifTextScaledAt(afterText, afterX, textY, phantomColor, style.scalePercent);
  }
  if (showFooter) {
    drawFooter(chapterLabel, progressPercent);
  }
  drawBatteryBadge(true);
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderTypographyPreview(const String &beforeText, const String &word,
                                             const String &afterText, uint8_t fontSizeLevel,
                                             const String &title, const String &line1,
                                             const String &line2) {
  const TypographyConfig config = activeTypographyConfig();
  const String renderKey =
      "typography_preview|" + beforeText + "|" + word + "|" + afterText + "|s:" +
      String(fontSizeLevel) + "|" + title + "|" + line1 + "|" + line2 + "|t:" +
      String(static_cast<int>(config.trackingPx)) + "|a:" +
      String(static_cast<unsigned int>(config.anchorPercent)) + "|w:" +
      String(static_cast<unsigned int>(config.guideHalfWidth)) + "|g:" +
      String(static_cast<unsigned int>(config.guideGap)) + "|b:" + batteryLabel_ + "|d:" +
      String(darkMode_ ? 1 : 0) + "|n:" + String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int tinyHeight = kTinyGlyphHeight * kTinyScale;
  const int titleY = 14;
  const int line2Y = std::max(titleY + tinyHeight + 1, virtualHeight - tinyHeight - 12);
  const int line1Y = std::max(titleY + tinyHeight + 1, line2Y - tinyHeight - 8);
  const int textTop = titleY + tinyHeight + 12;
  const int textBottom = std::max(textTop + 1, line1Y - 14);
  const int maxLabelWidth = virtualWidth - 24;

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawTinyTextCentered(fitTinyText(title, maxLabelWidth, kTinyScale), titleY, wordColor(),
                       kTinyScale);

  if (fontSizeLevel == 1) {
    const int textHeight = kEmbeddedSerif70Height;
    int textY = (textTop + textBottom - textHeight) / 2;
    textY = std::max(textTop, std::min(textY, textBottom - textHeight));
    const int focusIndex = findFocusLetterIndex(word);
    const int currentX = rsvpStartX70(word, focusIndex, virtualWidth, false);
    const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
    const TextLayoutMetrics currentLayout = serif70WordLayout(word, focusIndex);
    const uint16_t phantomColor = blendOverBackground(wordColor(), kPhantomAlphaMedium);

    drawRsvpAnchorGuide(anchorX, textY, textHeight);
    if (!beforeText.isEmpty()) {
      const TextLayoutMetrics beforeLayout = serif70WordLayout(beforeText, -1);
      const int beforeX =
          currentX + currentLayout.minX - kPhantomCurrentGapMedium - beforeLayout.maxX;
      drawSerif70TextAt(beforeText, beforeX, textY, phantomColor);
    }
    drawRsvp70WordAt(word, currentX, textY, focusIndex);
    if (!afterText.isEmpty()) {
      const TextLayoutMetrics afterLayout = serif70WordLayout(afterText, -1);
      const int afterX =
          currentX + currentLayout.maxX + kPhantomCurrentGapMedium - afterLayout.minX;
      drawSerif70TextAt(afterText, afterX, textY, phantomColor);
    }
  } else {
    const ReaderTextStyle style = readerTextStyle(fontSizeLevel);
    const int textHeight = scaledPercentDimension(kBaseGlyphHeight, style.scalePercent);
    int textY = (textTop + textBottom - textHeight) / 2;
    textY = std::max(textTop, std::min(textY, textBottom - textHeight));
    const int focusIndex = findFocusLetterIndex(word);
    const int currentX =
        rsvpStartXScaledPercent(word, focusIndex, virtualWidth, style.scalePercent, false);
    const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
    const TextLayoutMetrics currentLayout =
        serifWordLayoutScaledPercent(word, focusIndex, style.scalePercent);
    const uint16_t phantomColor = blendOverBackground(wordColor(), style.alpha);

    drawRsvpAnchorGuide(anchorX, textY, textHeight);
    if (!beforeText.isEmpty()) {
      const TextLayoutMetrics beforeLayout =
          serifWordLayoutScaledPercent(beforeText, -1, style.scalePercent);
      const int beforeX = currentX + currentLayout.minX - style.currentGap - beforeLayout.maxX;
      drawSerifTextScaledAt(beforeText, beforeX, textY, phantomColor, style.scalePercent);
    }
    drawRsvpWordScaledPercentAt(word, currentX, textY, focusIndex, style.scalePercent);
    if (!afterText.isEmpty()) {
      const TextLayoutMetrics afterLayout =
          serifWordLayoutScaledPercent(afterText, -1, style.scalePercent);
      const int afterX = currentX + currentLayout.maxX + style.currentGap - afterLayout.minX;
      drawSerifTextScaledAt(afterText, afterX, textY, phantomColor, style.scalePercent);
    }
  }

  if (!line1.isEmpty()) {
    drawTinyTextCentered(fitTinyText(line1, maxLabelWidth, kTinyScale), line1Y, focusColor(),
                         kTinyScale);
  }
  if (!line2.isEmpty()) {
    drawTinyTextCentered(fitTinyText(line2, maxLabelWidth, kTinyScale), line2Y, dimColor(),
                         kTinyScale);
  }
  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderPhantomRsvpWordWithWpm(const String &beforeText, const String &word,
                                                  const String &afterText, uint8_t fontSizeLevel,
                                                  uint16_t wpm, const String &chapterLabel,
                                                  uint8_t progressPercent, bool showFooter) {
  const String wpmText = String(wpm) + " WPM";
  const String renderKey =
      "rsvp_phantom_wpm|" + beforeText + "|" + word + "|" + afterText + "|s:" +
      String(fontSizeLevel) + "|" + wpmText + "|" + chapterLabel + "|" +
      String(progressPercent) + "|" + String(showFooter ? 1 : 0) + "|b:" + batteryLabel_ +
      "|d:" + String(darkMode_ ? 1 : 0) + "|n:" + String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  if (fontSizeLevel == 1) {
    const int scale = 1;
    const int virtualWidth = kDisplayWidth;
    const int virtualHeight = kDisplayHeight;
    const int textY = std::max(0, (virtualHeight - kEmbeddedSerif70Height) / 2);
    const int wpmY =
        std::max(0, virtualHeight - kTinyGlyphHeight * kTinyScale - kWpmFeedbackBottomMargin - 24);
    const int focusIndex = findFocusLetterIndex(word);
    const int currentX = rsvpStartX70(word, focusIndex, virtualWidth, false);
    const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
    const TextLayoutMetrics currentLayout = serif70WordLayout(word, focusIndex);
    const uint16_t phantomColor = blendOverBackground(wordColor(), kPhantomAlphaMedium);

    clearVirtualBuffer(virtualWidth, virtualHeight);
    drawRsvpAnchorGuide(anchorX, textY, kEmbeddedSerif70Height);
    if (!beforeText.isEmpty()) {
      const TextLayoutMetrics beforeLayout = serif70WordLayout(beforeText, -1);
      const int beforeX =
          currentX + currentLayout.minX - kPhantomCurrentGapMedium - beforeLayout.maxX;
      drawSerif70TextAt(beforeText, beforeX, textY, phantomColor);
    }
    drawRsvp70WordAt(word, currentX, textY, focusIndex);
    if (!afterText.isEmpty()) {
      const TextLayoutMetrics afterLayout = serif70WordLayout(afterText, -1);
      const int afterX =
          currentX + currentLayout.maxX + kPhantomCurrentGapMedium - afterLayout.minX;
      drawSerif70TextAt(afterText, afterX, textY, phantomColor);
    }
    drawTinyTextCentered(wpmText, wpmY, focusColor(), kTinyScale);
    if (showFooter) {
      drawFooter(chapterLabel, progressPercent);
    }
    drawBatteryBadge(true);
    flushScaledFrame(scale, virtualWidth, virtualHeight);
    return;
  }

  const ReaderTextStyle style = readerTextStyle(fontSizeLevel);
  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int textHeight = scaledPercentDimension(kBaseGlyphHeight, style.scalePercent);
  const int textY = std::max(0, (virtualHeight - textHeight) / 2);
  const int wpmY =
      std::max(0, virtualHeight - kTinyGlyphHeight * kTinyScale - kWpmFeedbackBottomMargin - 24);
  const int focusIndex = findFocusLetterIndex(word);
  const int currentX =
      rsvpStartXScaledPercent(word, focusIndex, virtualWidth, style.scalePercent, false);
  const int anchorX = (virtualWidth * currentAnchorPercent()) / 100;
  const TextLayoutMetrics currentLayout =
      serifWordLayoutScaledPercent(word, focusIndex, style.scalePercent);
  const uint16_t phantomColor = blendOverBackground(wordColor(), style.alpha);

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawRsvpAnchorGuide(anchorX, textY, textHeight);
  if (!beforeText.isEmpty()) {
    const TextLayoutMetrics beforeLayout =
        serifWordLayoutScaledPercent(beforeText, -1, style.scalePercent);
    const int beforeX = currentX + currentLayout.minX - style.currentGap - beforeLayout.maxX;
    drawSerifTextScaledAt(beforeText, beforeX, textY, phantomColor, style.scalePercent);
  }
  drawRsvpWordScaledPercentAt(word, currentX, textY, focusIndex, style.scalePercent);
  if (!afterText.isEmpty()) {
    const TextLayoutMetrics afterLayout =
        serifWordLayoutScaledPercent(afterText, -1, style.scalePercent);
    const int afterX = currentX + currentLayout.maxX + style.currentGap - afterLayout.minX;
    drawSerifTextScaledAt(afterText, afterX, textY, phantomColor, style.scalePercent);
  }
  drawTinyTextCentered(wpmText, wpmY, focusColor(), kTinyScale);
  if (showFooter) {
    drawFooter(chapterLabel, progressPercent);
  }
  drawBatteryBadge(true);
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderContextView(const std::vector<ContextWord> &words,
                                       const String &chapterLabel, uint8_t progressPercent) {
  if (words.empty()) {
    renderRsvpWord("", chapterLabel, progressPercent, true);
    return;
  }

  String renderKey = "context|" + chapterLabel + "|" + String(progressPercent);
  renderKey += "|b:";
  renderKey += batteryLabel_;
  renderKey += "|d:";
  renderKey += String(darkMode_ ? 1 : 0);
  renderKey += "|n:";
  renderKey += String(nightMode_ ? 1 : 0);
  for (const ContextWord &word : words) {
    renderKey += "|";
    renderKey += word.current ? "*" : "";
    renderKey += word.paragraphStart ? ">" : "";
    renderKey += word.text;
  }

  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  struct ContextLine {
    size_t start = 0;
    size_t end = 0;
    bool paragraphStart = false;
    bool containsCurrent = false;
  };

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int textBottom =
      virtualHeight - kTinyGlyphHeight * kTinyScale - kFooterMarginBottom - 6;
  const int contextGlyphHeight =
      std::max(1, (kBaseGlyphHeight + kContextSerifDivisor - 1) / kContextSerifDivisor);
  const int maxLineWidth = virtualWidth - (kContextMarginX * 2);
  std::vector<ContextLine> lines;
  lines.reserve(16);

  size_t index = 0;
  int currentLine = 0;
  bool foundCurrentLine = false;
  while (index < words.size()) {
    ContextLine line;
    line.start = index;
    line.paragraphStart = words[index].paragraphStart;
    int lineWidth = line.paragraphStart ? kContextParagraphIndent : 0;

    while (index < words.size()) {
      if (index > line.start && words[index].paragraphStart) {
        break;
      }

      const int wordWidth = measureSerifTextWidth(words[index].text, kContextSerifDivisor);
      const int gap = (index == line.start) ? 0 : kContextSpaceWidth;
      if (index > line.start && lineWidth + gap + wordWidth > maxLineWidth) {
        break;
      }

      lineWidth += gap + wordWidth;
      line.containsCurrent = line.containsCurrent || words[index].current;
      ++index;

      if (lineWidth >= maxLineWidth) {
        break;
      }
    }

    line.end = std::max(line.start + 1, index);
    if (line.end > words.size()) {
      line.end = words.size();
    }
    if (line.containsCurrent && !foundCurrentLine) {
      currentLine = static_cast<int>(lines.size());
      foundCurrentLine = true;
    }
    lines.push_back(line);

    if (line.end == line.start) {
      ++index;
    }
  }

  size_t firstLine = 0;
  if (currentLine > 2) {
    firstLine = static_cast<size_t>(currentLine - 2);
  }
  if (firstLine + kContextTargetLines > lines.size() && lines.size() > kContextTargetLines) {
    firstLine = lines.size() - kContextTargetLines;
  }

  clearVirtualBuffer(virtualWidth, virtualHeight);

  int y = kContextTop;
  for (size_t lineIndex = firstLine; lineIndex < lines.size(); ++lineIndex) {
    const ContextLine &line = lines[lineIndex];
    if (lineIndex != firstLine && line.paragraphStart) {
      y += kContextParagraphGap;
    }
    if (y + contextGlyphHeight > textBottom) {
      break;
    }

    int x = kContextMarginX + (line.paragraphStart ? kContextParagraphIndent : 0);
    for (size_t wordIndex = line.start; wordIndex < line.end && wordIndex < words.size();
         ++wordIndex) {
      const ContextWord &word = words[wordIndex];
      const uint16_t color = word.current ? focusColor() : wordColor();
      const String visibleWord = fitSerifText(word.text, virtualWidth - x - kContextMarginX,
                                              kContextSerifDivisor);
      drawSerifTextAt(visibleWord, x, y, color, kContextSerifDivisor);
      x += measureSerifTextWidth(visibleWord, kContextSerifDivisor) + kContextSpaceWidth;
    }

    y += kContextLineHeight;
  }

  (void)progressPercent;
  const int textY = kDisplayHeight - kTinyGlyphHeight * kTinyScale - kFooterMarginBottom;
  const int maxChapterChipWidth = std::max(0, virtualWidth / 2 - 2 * kFooterMarginX);
  const String chapter = chapterLabel.isEmpty() ? String("START") : chapterLabel;
  const uint16_t chipBg = blendOverBackground(footerColor(), kLibraryChipBgAlpha);
  drawScrollingChipText(chapter, kFooterMarginX, textY, maxChapterChipWidth, footerColor(),
                        chipBg);
  if (!batteryLabel_.isEmpty()) {
    drawChipText(batteryLabel_, 0, textY, footerColor(), chipBg, true,
                 virtualWidth - kFooterMarginX);
  }
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderMenu(const char *const *items, size_t itemCount, size_t selectedIndex) {
  renderMenuWithAccent(items, itemCount, selectedIndex, SIZE_MAX, "");
}

void DisplayManager::renderMenuWithAccent(const char *const *items, size_t itemCount,
                                          size_t selectedIndex, size_t accentRow,
                                          const String &accentText,
                                          const std::vector<String> &accentChips,
                                          const std::vector<bool> &chevronRows) {
  if (items == nullptr || itemCount == 0) {
    renderCenteredWord("MENU");
    return;
  }

  if (selectedIndex >= itemCount) {
    selectedIndex = itemCount - 1;
  }

  String renderKey = "menua|";
  renderKey += String(selectedIndex);
  renderKey += "|b:";
  renderKey += batteryLabel_;
  renderKey += "|d:";
  renderKey += String(darkMode_ ? 1 : 0);
  renderKey += "|n:";
  renderKey += String(nightMode_ ? 1 : 0);
  for (size_t i = 0; i < itemCount; ++i) {
    renderKey += "|";
    renderKey += (items[i] == nullptr ? "" : items[i]);
  }
  if (accentRow < itemCount && (!accentText.isEmpty() || !accentChips.empty())) {
    renderKey += "|a:";
    renderKey += String(accentRow);
    renderKey += ":";
    renderKey += accentText;
    for (const String &c : accentChips) {
      renderKey += "|c:";
      renderKey += c;
    }
    if (measureTinyTextWidth(accentText, kTinyScale) > 200) {
      renderKey += "|p:";
      renderKey += String(millis() / 30);
    }
  }

  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }
  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const size_t visibleCount =
      std::min(itemCount, static_cast<size_t>(std::max(1, virtualHeight / kCompactMenuRowHeight)));
  size_t firstVisible = 0;
  if (selectedIndex >= visibleCount / 2) {
    firstVisible = selectedIndex - visibleCount / 2;
  }
  if (firstVisible + visibleCount > itemCount) {
    firstVisible = itemCount - visibleCount;
  }

  const int rowHeight = kCompactMenuRowHeight;
  // Top-align — Back/first item lives at the top of every menu, never
  // centred vertically. Keeps muscle memory consistent across pickers.
  int y = 0;

  clearVirtualBuffer(virtualWidth, virtualHeight);

  const int chevronWidth = measureTinyTextWidth(">", kTinyScale);
  const int chevronSpacingPx = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;
  const int batteryChipWidth =
      batteryLabel_.isEmpty()
          ? 0
          : measureTinyTextWidth(batteryLabel_, kTinyScale) + 5 * 2;
  const int batteryReserve = batteryChipWidth > 0 ? batteryChipWidth + 8 : 0;
  const int accentRightInset =
      kFooterMarginX + kLibraryLetterStripWidth + batteryReserve;
  const uint16_t accentColor = darkMode_ ? 0xFFE0 : 0xFB00;

  // Chevron column sits two spaces past the widest item so all chevrons align.
  int maxItemTextWidth = 0;
  for (size_t i = 0; i < itemCount; ++i) {
    if (items[i] == nullptr) continue;
    const int w = measureTinyTextWidth(String(items[i]), kTinyScale);
    if (w > maxItemTextWidth) maxItemTextWidth = w;
  }
  const int chevronColumnX = kCompactMenuX + maxItemTextWidth + chevronSpacingPx * 2;

  for (size_t row = 0; row < visibleCount; ++row) {
    const size_t itemIndex = firstVisible + row;
    const bool selected = itemIndex == selectedIndex;
    const uint16_t color = selected ? focusColor() : dimColor();
    const String &itemText = items[itemIndex] == nullptr ? String("") : String(items[itemIndex]);

    const int textX = kCompactMenuX;
    if (selected) {
      fillVirtualRect(10, y + 2, 5, kTinyGlyphHeight * kTinyScale + 2, selectedBarColor());
    }
    const int itemWidth = measureTinyTextWidth(itemText, kTinyScale);
    const bool hasAccent = itemIndex == accentRow &&
                           (!accentText.isEmpty() || !accentChips.empty());
    const bool hasChevron = itemIndex < chevronRows.size() && chevronRows[itemIndex];
    const int rowRightLimit = chevronColumnX - chevronSpacingPx;
    const int maxItemWidth =
        std::max(0, rowRightLimit - textX - (hasAccent ? 4 : 0));
    const String fittedItem = fitTinyText(itemText, maxItemWidth, kTinyScale);
    drawTinyTextAt(fittedItem, textX, y + 3, color, kTinyScale);
    if (hasChevron) {
      drawTinyTextAt(">", chevronColumnX, y + 3, color, kTinyScale);
    }

    if (hasAccent) {
      const int chipPadX = 5;
      const int chipPadY = 2;
      const int chipH = kTinyGlyphHeight * kTinyScale + chipPadY * 2;
      const int chipY = y + 3 - chipPadY;
      const uint16_t chipBg = blendOverBackground(accentColor, kLibraryChipBgAlpha);
      const int chipRadius = std::min(6, chipH / 2);
      const int chipGap = 4;

      const int titleStartX = textX + std::max(itemWidth, 0) + 14;
      const int rightLimit = virtualWidth - accentRightInset;
      const int titleW = measureTinyTextWidth(accentText, kTinyScale);

      int chipsTotalWidth = 0;
      for (const String &c : accentChips) {
        const int textW = measureTinyTextWidth(c, kTinyScale);
        chipsTotalWidth += textW + chipPadX * 2 + chipGap;
      }

      const int spaceAvailable = std::max(0, rightLimit - titleStartX);
      const int needsForBoth = titleW + 8 + chipsTotalWidth;
      const bool inlineLayout = !accentText.isEmpty() && needsForBoth <= spaceAvailable;

      int titleEndX;
      int chipsLeftStartX;
      if (inlineLayout) {
        titleEndX = titleStartX + titleW;
        chipsLeftStartX = titleEndX + 8;
      } else {
        chipsLeftStartX = std::max(titleStartX, rightLimit - chipsTotalWidth);
        titleEndX = chipsLeftStartX - chipGap;
      }

      int chipCursor = chipsLeftStartX;
      for (const String &chip : accentChips) {
        const int textW = measureTinyTextWidth(chip, kTinyScale);
        const int chipW = textW + chipPadX * 2;
        if (chipCursor + chipW > rightLimit) break;
        fillRoundedRect(chipCursor, chipY, chipW, chipH, chipRadius, chipBg);
        drawTinyTextAt(chip, chipCursor + chipPadX, y + 3, accentColor, kTinyScale);
        chipCursor += chipW + chipGap;
      }

      const int titleMaxWidth = std::max(0, titleEndX - titleStartX);
      if (titleMaxWidth > 0 && !accentText.isEmpty()) {
        // Diagnostic: log the marquee window so we can see if the math goes
        // pathological (negative widths, gigantic offsets, etc.) when the
        // device locks up landing on Resume with a long book title.
        Serial.printf("[marquee] start=%d end=%d w=%d titleLen=%u accW=%d\n",
                      titleStartX, titleEndX, titleMaxWidth,
                      static_cast<unsigned>(accentText.length()),
                      measureTinyTextWidth(accentText, kTinyScale));
        drawTinyMarquee(accentText, titleStartX, titleEndX, y + 3, accentColor,
                        backgroundColor());
      }
    }
    y += rowHeight;
  }

  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderMenu(const std::vector<String> &items, size_t selectedIndex) {
  renderMenu(items, selectedIndex, std::vector<bool>());
}

void DisplayManager::renderMenu(const std::vector<String> &items, size_t selectedIndex,
                                const std::vector<bool> &chevronRows) {
  if (items.empty()) {
    renderCenteredWord("MENU");
    return;
  }

  if (selectedIndex >= items.size()) {
    selectedIndex = items.size() - 1;
  }

  String renderKey = "menuv|";
  renderKey += String(selectedIndex);
  renderKey += "|b:";
  renderKey += batteryLabel_;
  renderKey += "|d:";
  renderKey += String(darkMode_ ? 1 : 0);
  renderKey += "|n:";
  renderKey += String(nightMode_ ? 1 : 0);
  for (const String &item : items) {
    renderKey += "|";
    renderKey += item;
  }
  for (size_t i = 0; i < chevronRows.size(); ++i) {
    if (chevronRows[i]) {
      renderKey += "|cr";
      renderKey += String(static_cast<unsigned int>(i));
    }
  }

  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const size_t itemCount = items.size();
  const size_t visibleCount =
      std::min(itemCount, static_cast<size_t>(std::max(1, virtualHeight / kCompactMenuRowHeight)));
  size_t firstVisible = 0;
  if (selectedIndex >= visibleCount / 2) {
    firstVisible = selectedIndex - visibleCount / 2;
  }
  if (firstVisible + visibleCount > itemCount) {
    firstVisible = itemCount - visibleCount;
  }

  const int rowHeight = kCompactMenuRowHeight;
  // Top-align — Back/first item lives at the top of every menu, never
  // centred vertically. Keeps muscle memory consistent across pickers.
  int y = 0;

  clearVirtualBuffer(virtualWidth, virtualHeight);

  const int chevronWidth = measureTinyTextWidth(">", kTinyScale);
  const int spaceWidth = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;

  // Chevron column sits two spaces past the widest item in this menu so every
  // chevron lines up regardless of which row it's on — but is capped at a
  // safe right-side position so menus containing very long items (e.g. the
  // Modules picker with 100+ .xm filenames) don't push the chevron off-screen
  // and squeeze every row's text width to zero.
  int maxItemTextWidth = 0;
  for (size_t i = 0; i < itemCount; ++i) {
    const int w = measureTinyTextWidth(items[i], kTinyScale);
    if (w > maxItemTextWidth) maxItemTextWidth = w;
  }
  const int chevronCapX = virtualWidth - chevronWidth - kFooterMarginX - 4;
  const int chevronColumnX = std::min(
      kCompactMenuX + maxItemTextWidth + spaceWidth * 2, chevronCapX);

  for (size_t row = 0; row < visibleCount; ++row) {
    const size_t itemIndex = firstVisible + row;
    const bool selected = itemIndex == selectedIndex;
    const uint16_t color = selected ? focusColor() : dimColor();
    const bool hasChevron = itemIndex < chevronRows.size() && chevronRows[itemIndex];
    // Text runs from leftX up to one space before the chevron column. This
    // keeps maxWidth strictly positive when chevronColumnX is capped at the
    // right edge for long-item menus.
    const int maxWidth = std::max(0, chevronColumnX - kCompactMenuX - spaceWidth);
    if (selected) {
      fillVirtualRect(10, y + 2, 5, kTinyGlyphHeight * kTinyScale + 2, selectedBarColor());
    }
    const String &itemText = items[itemIndex];
    const int itemTextWidth = measureTinyTextWidth(itemText, kTinyScale);
    const int textY = y + 3;
    const int leftX = kCompactMenuX;
    const int rightX = leftX + maxWidth;
    if (itemTextWidth <= maxWidth) {
      drawTinyTextAt(itemText, leftX, textY, color, kTinyScale);
    } else if (selected) {
      // Selected row scrolls; unselected rows truncate (multiple simultaneous
      // marquees would be visual chaos).
      drawTinyMarquee(itemText, leftX, rightX, textY, color, backgroundColor());
    } else {
      const String fittedItem = fitTinyText(itemText, maxWidth, kTinyScale);
      drawTinyTextAt(fittedItem, leftX, textY, color, kTinyScale);
    }
    if (hasChevron) {
      drawTinyTextAt(">", chevronColumnX, textY, color, kTinyScale);
    }
    y += rowHeight;
  }

  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

int DisplayManager::tabUnderlineWidth(int tabCount) {
  if (tabCount <= 0) return 0;
  const int grayW = kDisplayWidth - kTabDividerLeftMarginPx - kTabDividerRightMarginPx;
  return grayW > 0 ? grayW / tabCount : 0;
}

int DisplayManager::tabUnderlineXForTab(int tabIdx, int tabCount) {
  if (tabCount <= 0) return kTabDividerLeftMarginPx;
  return kTabDividerLeftMarginPx + tabIdx * tabUnderlineWidth(tabCount);
}

void DisplayManager::renderMenuWithTabs(const std::vector<String> &items,
                                        size_t selectedIndex,
                                        const std::vector<bool> &chevronRows,
                                        const std::vector<String> &tabLabels,
                                        int activeTabIdx,
                                        int underlineXPx,
                                        int underlineWPx) {
  if (items.empty()) {
    renderCenteredWord("MENU");
    return;
  }
  if (selectedIndex >= items.size()) selectedIndex = items.size() - 1;

  // Tab band is animated — bypass the lastRenderKey_ cache so each call
  // (especially during slide animations) repaints.
  lastRenderKey_ = "";

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;

  // Tab band layout constants live at file scope so renderTabUnderlineStrip()
  // (the partial-strip animation overlay) shares the exact same geometry.
  clearVirtualBuffer(virtualWidth, virtualHeight);

  // ---- Tab band ----
  const int tabCount = static_cast<int>(tabLabels.size());
  if (tabCount > 0) {
    const int slotW = virtualWidth / tabCount;
    for (int i = 0; i < tabCount; ++i) {
      const String &label = tabLabels[i];
      const int labelW = measureTinyTextWidth(label, kTinyScale);
      const int slotCenter = i * slotW + slotW / 2;
      const int labelX = std::max(0, slotCenter - labelW / 2);
      const uint16_t color = (i == activeTabIdx) ? focusColor() : dimColor();
      drawTinyTextAt(label, labelX, kTabLabelY, color, kTinyScale);
    }
    // Draw the divider FIRST so the bright underline z-paints on top of it.
    // Hairline is inset on both sides; right inset is wider than purely
    // aesthetic — it has to clear the right-aligned battery chip.
    const int dividerWidth =
        virtualWidth - kTabDividerLeftMarginPx - kTabDividerRightMarginPx;
    if (dividerWidth > 0) {
      fillVirtualRect(kTabDividerLeftMarginPx, kTabDividerY, dividerWidth, 1,
                      dimColor());
    }
    // Active tab's underline — drawn AFTER the divider so it cleanly overlaps
    // (highlights) the corresponding slice of the gray line.
    if (underlineWPx > 0 && underlineXPx >= 0) {
      const int clampedX = std::max(0, std::min(underlineXPx, virtualWidth - 1));
      const int clampedW = std::min(underlineWPx, virtualWidth - clampedX);
      fillVirtualRect(clampedX, kTabUnderlineY, clampedW, kTabUnderlineH,
                      focusColor());
    }
  }

  // ---- Menu rows below the tab band ----
  const int menuTopY = kTabBandHeight + kTabMenuGapPx;
  const int menuHeight = std::max(0, virtualHeight - menuTopY);
  const size_t itemCount = items.size();
  const size_t visibleCount =
      std::min(itemCount, static_cast<size_t>(std::max(1, menuHeight / kCompactMenuRowHeight)));
  size_t firstVisible = 0;
  if (selectedIndex >= visibleCount / 2) {
    firstVisible = selectedIndex - visibleCount / 2;
  }
  if (firstVisible + visibleCount > itemCount) {
    firstVisible = itemCount - visibleCount;
  }

  const int rowHeight = kCompactMenuRowHeight;
  // Top-align — Back (row 0) always sits at the top of the menu area so the
  // user reaches it predictably regardless of how many items are below.
  int y = menuTopY;

  const int chevronWidth = measureTinyTextWidth(">", kTinyScale);
  const int spaceWidth = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;
  int maxItemTextWidth = 0;
  for (size_t i = 0; i < itemCount; ++i) {
    const int w = measureTinyTextWidth(items[i], kTinyScale);
    if (w > maxItemTextWidth) maxItemTextWidth = w;
  }
  const int chevronCapX = virtualWidth - chevronWidth - kFooterMarginX - 4;
  const int chevronColumnX = std::min(
      kCompactMenuX + maxItemTextWidth + spaceWidth * 2, chevronCapX);

  for (size_t row = 0; row < visibleCount; ++row) {
    const size_t itemIndex = firstVisible + row;
    const bool selected = itemIndex == selectedIndex;
    const uint16_t color = selected ? focusColor() : dimColor();
    const bool hasChevron = itemIndex < chevronRows.size() && chevronRows[itemIndex];
    const int maxWidth = std::max(0, chevronColumnX - kCompactMenuX - spaceWidth);
    if (selected) {
      fillVirtualRect(10, y + 2, 5, kTinyGlyphHeight * kTinyScale + 2, selectedBarColor());
    }
    const String &itemText = items[itemIndex];
    const int itemTextWidth = measureTinyTextWidth(itemText, kTinyScale);
    const int textY = y + 3;
    const int leftX = kCompactMenuX;
    const int rightX = leftX + maxWidth;
    if (itemTextWidth <= maxWidth) {
      drawTinyTextAt(itemText, leftX, textY, color, kTinyScale);
    } else if (selected) {
      drawTinyMarquee(itemText, leftX, rightX, textY, color, backgroundColor());
    } else {
      const String fittedItem = fitTinyText(itemText, maxWidth, kTinyScale);
      drawTinyTextAt(fittedItem, leftX, textY, color, kTinyScale);
    }
    if (hasChevron) {
      drawTinyTextAt(">", chevronColumnX, textY, color, kTinyScale);
    }
    y += rowHeight;
  }

  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderTabUnderlineStrip(int underlineXPx, int underlineWPx) {
  if (!initialized_ || txBuffer_ == nullptr) return;

  const bool rotated = uiRotated_;

  // Native-X slice that covers the union of the underline rows and the
  // divider row. Underline now sits 1 px above the divider, so the slice
  // spans logical-Y [kTabUnderlineY, kTabDividerY] inclusive.
  const int logicalSpanMin = std::min(kTabUnderlineY, kTabDividerY);
  const int logicalSpanMax = std::max(kTabUnderlineY + kTabUnderlineH - 1,
                                      kTabDividerY);
  int nxA, nxB;
  if (rotated) {
    nxA = (kDisplayHeight - 1) - logicalSpanMin;
    nxB = (kDisplayHeight - 1) - logicalSpanMax;
  } else {
    nxA = logicalSpanMin;
    nxB = logicalSpanMax;
  }
  int nxStart = std::min(nxA, nxB);
  int nxEnd = std::max(nxA, nxB) + 1;
  if (nxStart < 0) nxStart = 0;
  if (nxEnd > kPanelNativeWidth) nxEnd = kPanelNativeWidth;
  const int cols = nxEnd - nxStart;
  if (cols <= 0) return;
  if (static_cast<size_t>(cols) * kPanelNativeHeight > kTxBufferPixels) return;

  const uint16_t bg = panelColor(backgroundColor());
  const uint16_t dividerColor = panelColor(dimColor());
  const uint16_t underlineColor = panelColor(focusColor());

  const int dividerLeft = kTabDividerLeftMarginPx;
  const int dividerRight = kDisplayWidth - kTabDividerRightMarginPx;
  const int underlineLeft = underlineXPx;
  const int underlineRight = underlineXPx + underlineWPx;

  // For each native row (= logical X), paint the `cols` columns of the strip
  // by deciding pixel-by-pixel which of {bg, divider, underline} applies.
  // ~2 KB of compose for 2 cols × 640 rows — well under 1 ms.
  for (int ny = 0; ny < kPanelNativeHeight; ++ny) {
    const int lx = rotated ? ny : ((kDisplayWidth - 1) - ny);
    uint16_t *row = txBuffer_ + ny * cols;
    const bool inUnderlineX = (lx >= underlineLeft && lx < underlineRight);
    const bool inDividerX = (lx >= dividerLeft && lx < dividerRight);
    for (int c = nxStart; c < nxEnd; ++c) {
      const int ly = rotated ? ((kDisplayHeight - 1) - c) : c;
      uint16_t color = bg;
      if (inUnderlineX && ly >= kTabUnderlineY && ly < kTabUnderlineY + kTabUnderlineH) {
        color = underlineColor;
      } else if (ly == kTabDividerY && inDividerX) {
        color = dividerColor;
      }
      row[c - nxStart] = color;
    }
  }

  drawBitmap(nxStart, 0, nxEnd, kPanelNativeHeight, txBuffer_);
}

void DisplayManager::drawTinyGlyphNativeStripe(int logicalX, int logicalY, char c,
                                               int scale, uint16_t panelEncoded,
                                               int stripeStart, int stripeRows,
                                               int clipLeftLogicalX,
                                               int clipRightLogicalX) {
  // Coordinate mapping mirrors flushScaledFrame's slow path:
  //   uiRotated_  → nativeY = logicalX,                 nativeX = (kDisplayHeight - 1) - logicalY
  //   !uiRotated_ → nativeY = (kDisplayWidth - 1) - lx, nativeX = logicalY
  const uint8_t *gRows = tinyRowsFor(c);
  const int stripeEnd = stripeStart + stripeRows;
  const bool rotated = uiRotated_;
  for (int gy = 0; gy < kTinyGlyphHeight; ++gy) {
    const uint8_t bits = gRows[gy];
    if (bits == 0) continue;
    for (int gx = 0; gx < kTinyGlyphWidth; ++gx) {
      if ((bits & (1 << (kTinyGlyphWidth - 1 - gx))) == 0) continue;
      for (int yy = 0; yy < scale; ++yy) {
        const int ly = logicalY + gy * scale + yy;
        if (ly < 0 || ly >= kDisplayHeight) continue;
        const int nx = rotated ? ((kDisplayHeight - 1) - ly) : ly;
        for (int xx = 0; xx < scale; ++xx) {
          const int lx = logicalX + gx * scale + xx;
          if (lx < clipLeftLogicalX || lx >= clipRightLogicalX) continue;
          const int ny = rotated ? lx : ((kDisplayWidth - 1) - lx);
          if (ny < stripeStart || ny >= stripeEnd) continue;
          txBuffer_[(ny - stripeStart) * kPanelNativeWidth + nx] = panelEncoded;
        }
      }
    }
  }
}

void DisplayManager::drawTinyTextNativeStripe(const String &text, int logicalX,
                                              int logicalY, int scale,
                                              uint16_t panelEncoded,
                                              int stripeStart, int stripeRows,
                                              int clipLeftLogicalX,
                                              int clipRightLogicalX) {
  if (text.length() == 0) return;
  // Whole-string early-out — map the entire run's logicalX extent into the
  // native-Y axis (orientation aware), skip if it doesn't touch the stripe.
  const bool rotated = uiRotated_;
  const int textWidth = measureTinyTextWidth(text, scale);
  const int textLeftLX = logicalX;
  const int textRightLX = logicalX + textWidth;
  const int aY = rotated ? textLeftLX : ((kDisplayWidth - 1) - textRightLX + 1);
  const int bY = rotated ? textRightLX : ((kDisplayWidth - 1) - textLeftLX + 1);
  const int textNyStart = std::min(aY, bY);
  const int textNyEnd = std::max(aY, bY);
  if (textNyEnd <= stripeStart || textNyStart >= stripeStart + stripeRows) return;

  // Per-glyph: rely on drawTinyGlyphNativeStripe's per-pixel clip so this works
  // for both orientations and for marquee-style negative cursor offsets.
  const int charPitch = (kTinyGlyphWidth + kTinyGlyphSpacing) * scale;
  int cursorX = logicalX;
  for (size_t i = 0; i < text.length(); ++i) {
    drawTinyGlyphNativeStripe(cursorX, logicalY, text[i], scale, panelEncoded,
                              stripeStart, stripeRows, clipLeftLogicalX,
                              clipRightLogicalX);
    cursorX += charPitch;
  }
}

namespace {

// 565 alpha blend — alpha=0 → a, alpha=255 → b. Used to build per-bar gradient
// stops and the soft halo around peak markers.
inline uint16_t blend565(uint16_t a, uint16_t b, uint8_t alpha) {
  const uint16_t inv = static_cast<uint16_t>(255 - alpha);
  const uint16_t ar = (a >> 11) & 0x1F, ag = (a >> 5) & 0x3F, ab = a & 0x1F;
  const uint16_t br = (b >> 11) & 0x1F, bg = (b >> 5) & 0x3F, bb = b & 0x1F;
  const uint16_t r = static_cast<uint16_t>((ar * inv + br * alpha) >> 8) & 0x1F;
  const uint16_t g = static_cast<uint16_t>((ag * inv + bg * alpha) >> 8) & 0x3F;
  const uint16_t bbo = static_cast<uint16_t>((ab * inv + bb * alpha) >> 8) & 0x1F;
  return static_cast<uint16_t>((r << 11) | (g << 5) | bbo);
}

}  // namespace

void DisplayManager::renderModulePlayerFrame(const char *fileName,
                                             const char *title,
                                             const char *format,
                                             int bpm, int speed,
                                             int pos, int row, int numRows,
                                             const uint8_t *barLevels,
                                             const uint8_t *peakLevels,
                                             int barCount) {
  if (!initialized_) return;
  // Animation forces a full repaint each frame.
  lastRenderKey_ = "";

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  const uint32_t t0 = micros();
  uint32_t composeUs = 0;
  uint32_t spiUs = 0;
#endif

  // ---- 1) Build display strings. ----
  const String fileText = fileName ? String(fileName) : String("");
  String headline = title ? String(title) : String("");
  headline.trim();
  if (headline.isEmpty()) headline = fileText;

  String meta;
  if (!fileText.isEmpty() && fileText != headline) meta += fileText;
  if (format && format[0]) {
    if (meta.length() > 0) meta += "  ";
    meta += format;
  }
  if (bpm > 0) {
    if (meta.length() > 0) meta += "  ";
    meta += "BPM "; meta += String(bpm);
  }
  if (speed > 0) {
    meta += "  SPD "; meta += String(speed);
  }
  if (numRows > 0) {
    if (meta.length() > 0) meta += "  ";
    meta += "POS "; meta += String(pos);
    meta += "  ROW "; meta += String(row);
    meta += "/"; meta += String(numRows);
  }

  // ---- 2) Logical-coord layout. ----
  constexpr int kHeadlineLogicalY = 4;
  constexpr int kHeadlineLeftLogicalX = 22;   // leaves room for the play pulse glyph
  constexpr int kHeadlineRightLogicalX = kDisplayWidth - kFooterMarginX - 12;
  constexpr int kMetaLogicalY = kHeadlineLogicalY + (kTinyGlyphHeight * kTinyScale) + 6;
  constexpr int kBarsTopLogicalY = 50;
  constexpr int kBarsBottomLogicalY = kDisplayHeight - 6;          // 166
  constexpr int kBarsLeftLogicalX = 12;
  constexpr int kBarsRightLogicalX = kDisplayWidth - kFooterMarginX - 12;
  constexpr int kBarsHeight = kBarsBottomLogicalY - kBarsTopLogicalY;  // 116
  constexpr int kBarsLogicalWidth = kBarsRightLogicalX - kBarsLeftLogicalX;
  constexpr int kProgressLogicalY = 1;          // pattern-row progress hairline
  constexpr int kPlayPulseLogicalX = 10;        // left of the headline
  constexpr int kPlayPulseLogicalY = kHeadlineLogicalY + (kTinyGlyphHeight * kTinyScale) / 2;

  // ---- 3) Orientation-aware native coord helpers. ----
  // Mirrors flushScaledFrame's slow path so the module player renders right
  // side up regardless of the user's display-flip preference.
  const bool rotated = uiRotated_;
  auto lxToNy = [rotated](int lx) -> int {
    return rotated ? lx : ((kDisplayWidth - 1) - lx);
  };
  auto lyToNx = [rotated](int ly) -> int {
    return rotated ? ((kDisplayHeight - 1) - ly) : ly;
  };

  // ---- 4) Bar geometry, projected once into native space. ----
  if (barCount < 1) barCount = 4;
  if (barCount > 32) barCount = 32;
  constexpr int kMinBarPitchPx = 6;
  int effectiveBars = barCount;
  while (effectiveBars > 1 && kBarsLogicalWidth / effectiveBars < kMinBarPitchPx) {
    effectiveBars /= 2;
  }
  const int pitch = kBarsLogicalWidth / effectiveBars;
  const int barBodyW = std::max(2, pitch - 4);

  // Pico-8-ish rainbow so adjacent channels read as distinct columns.
  static const uint16_t kBarPalette[] = {
      0xF810, 0xFD60, 0xFFE0, 0x07E6, 0x2B7F, 0xC618, 0xFFFB, 0xFE36,
  };
  constexpr int kBarPaletteSize = sizeof(kBarPalette) / sizeof(kBarPalette[0]);

  // 32 channels × 116 px gradient column cache. Function-scope static = 7.4 KB
  // in BSS, recomputed once per frame for the live bars only. Saves ~10 ms of
  // re-blending across 14 stripes vs. computing inline per-pixel.
  static uint16_t sBarFillCache[32 * kBarsHeight];

  struct BarGeom {
    int16_t nyStart;        // native-row range that covers the bar's width
    int16_t nyEnd;
    int16_t nxBaseline;     // native column of the bar's baseline pixel
    int16_t h;              // filled pixel count (0..kBarsHeight)
    int16_t peakNxCore0;    // peak marker — first of two bright cells
    int16_t peakNxCore1;    // second bright cell (or same as core0 if peakH==1)
    int16_t peakNxHalo;     // 1-px halo above the marker (-1 if disabled)
    uint16_t peakHaloColor;
  };
  BarGeom bars[32];
  const int nxDir = rotated ? 1 : -1;  // direction "up the bar" in native X
  const int railA = lyToNx(kBarsTopLogicalY);
  const int railB = lyToNx(kBarsBottomLogicalY - 1);
  const int kRailNxStart = std::min(railA, railB);
  const int kRailNxEnd = std::max(railA, railB) + 1;
  // 25 %, 50 %, 75 % grid ticks in the rail give the meter a sense of scale.
  int gridNx[3];
  gridNx[0] = lyToNx(kBarsBottomLogicalY - (kBarsHeight * 1) / 4);
  gridNx[1] = lyToNx(kBarsBottomLogicalY - (kBarsHeight * 2) / 4);
  gridNx[2] = lyToNx(kBarsBottomLogicalY - (kBarsHeight * 3) / 4);
  const uint16_t railColor = panelColor(blend565(0x0000, dimColor(), 200));   // very dim
  const uint16_t gridColor = panelColor(blend565(0x0000, dimColor(), 120));   // even dimmer ghost ticks
  const uint16_t peakCoreColor = panelColor(0xFFFF);

  const uint16_t white565 = 0xFFFF;
  for (int i = 0; i < effectiveBars; ++i) {
    BarGeom &g = bars[i];
    const int lx = kBarsLeftLogicalX + i * pitch + (pitch - barBodyW) / 2;
    const int ny0 = lxToNy(lx);
    const int ny1 = lxToNy(lx + barBodyW - 1);
    g.nyStart = static_cast<int16_t>(std::min(ny0, ny1));
    g.nyEnd = static_cast<int16_t>(std::max(ny0, ny1) + 1);
    g.nxBaseline = static_cast<int16_t>(lyToNx(kBarsBottomLogicalY - 1));

    int v = (i < barCount && barLevels) ? barLevels[i] : 0;
    if (v > 64) v = 64;
    const int h = (v * kBarsHeight) / 64;
    g.h = static_cast<int16_t>(h);

    // 3-stop gradient: darkened base at the baseline, full base in the body,
    // hard white in the top 12 % so the tip pops like a hot VU LED.
    const uint16_t base565 = kBarPalette[i % kBarPaletteSize];
    const uint16_t baseDim565 = blend565(0x0000, base565, 165);  // ~65 % base
    uint16_t *fill = sBarFillCache + i * kBarsHeight;
    const int hh = std::max(1, h);
    for (int step = 0; step < h; ++step) {
      const int t256 = (step * 255) / std::max(1, hh - 1);
      uint16_t mid565;
      if (t256 >= 224) {
        const int hotFrac = std::min(255, (t256 - 224) * 8);
        mid565 = blend565(base565, white565, static_cast<uint8_t>(hotFrac));
      } else {
        const int riseFrac = (t256 * 255) / 224;
        mid565 = blend565(baseDim565, base565, static_cast<uint8_t>(riseFrac));
      }
      // Force the top 2 px to pure white so even short bars get a visible cap.
      if (step >= h - 2) mid565 = white565;
      fill[step] = panelColor(mid565);
    }

    // Peak-hold marker: 2-px white core + 1-px palette-tinted halo above.
    int p = (i < barCount && peakLevels) ? peakLevels[i] : 0;
    if (p > 64) p = 64;
    const int peakH = (p * kBarsHeight) / 64;
    if (peakH > h + 1 && peakH > 0) {
      const int core0Step = std::max(0, peakH - 1);
      const int core1Step = std::max(0, peakH - 2);
      g.peakNxCore0 = static_cast<int16_t>(g.nxBaseline + core0Step * nxDir);
      g.peakNxCore1 = static_cast<int16_t>(g.nxBaseline + core1Step * nxDir);
      if (peakH < kBarsHeight) {
        g.peakNxHalo = static_cast<int16_t>(g.nxBaseline + peakH * nxDir);
        g.peakHaloColor = panelColor(blend565(base565, white565, 180));
      } else {
        g.peakNxHalo = -1;
        g.peakHaloColor = 0;
      }
    } else {
      g.peakNxCore0 = -1;
      g.peakNxCore1 = -1;
      g.peakNxHalo = -1;
      g.peakHaloColor = 0;
    }
  }

  // ---- 5) Marquee offset for the headline if it overflows. ----
  const int headlineBandWidth = kHeadlineRightLogicalX - kHeadlineLeftLogicalX;
  const int headlineTextWidth = measureTinyTextWidth(headline, kTinyScale);
  int headlineCursorX = kHeadlineLeftLogicalX;
  if (headlineTextWidth > headlineBandWidth) {
    headlineCursorX -= marqueePingPongOffset(headlineTextWidth - headlineBandWidth);
  }
  const uint16_t headlineColor = panelColor(focusColor());
  const uint16_t metaColor = panelColor(dimColor());

  // ---- 6) Play pulse — a soft heartbeat behind the title. ----
  // 1.2 s sine-ish cycle; level in [0,255] drives a circle's brightness.
  const uint32_t pulseMs = millis() % 1200;
  const int pulseLevel = (pulseMs < 600)
      ? static_cast<int>((pulseMs * 255) / 600)
      : static_cast<int>(((1200 - pulseMs) * 255) / 600);
  const uint16_t pulseColor = panelColor(
      blend565(dimColor(), focusColor(), static_cast<uint8_t>(pulseLevel)));
  const int pulseNxC = lyToNx(kPlayPulseLogicalY);
  const int pulseNyC = lxToNy(kPlayPulseLogicalX);

  // ---- 7) Pattern-row progress hairline native bounds. ----
  const int progressNx = lyToNx(kProgressLogicalY);
  const int progLeftLX = kHeadlineLeftLogicalX;
  const int progRightLX = kHeadlineRightLogicalX;
  const int progressWidth = progRightLX - progLeftLX;
  // Use row/numRows when available; fall back to pos/256 for headers that
  // skip per-pattern info.
  const float progressFrac = (numRows > 0)
      ? std::min(1.0f, std::max(0.0f, static_cast<float>(row) / static_cast<float>(numRows)))
      : 0.0f;
  const int progFilledLX = progLeftLX + static_cast<int>(progressFrac * progressWidth);
  auto rangeNy = [&](int lxLeft, int lxRight, int &nyStart, int &nyEnd) {
    const int a = lxToNy(lxLeft);
    const int b = lxToNy(lxRight - 1);
    nyStart = std::min(a, b);
    nyEnd = std::max(a, b) + 1;
  };
  int progBgNyStart, progBgNyEnd, progFillNyStart, progFillNyEnd;
  rangeNy(progLeftLX, progRightLX, progBgNyStart, progBgNyEnd);
  if (progFilledLX > progLeftLX) {
    rangeNy(progLeftLX, progFilledLX, progFillNyStart, progFillNyEnd);
  } else {
    progFillNyStart = progFillNyEnd = 0;
  }
  const uint16_t progBgColor = panelColor(blend565(0x0000, dimColor(), 110));
  const uint16_t progFillColor = panelColor(focusColor());

  // ---- 8) Stripe loop. Native-orientation compose, no virtualFrame_. ----
  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    const int stripeEnd = stripeStart + rows;
    const size_t bytes = static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t);

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    const uint32_t cBegin = micros();
#endif
    std::memset(txBuffer_, 0, bytes);

    // -- Bars -------------------------------------------------------------
    for (int i = 0; i < effectiveBars; ++i) {
      const BarGeom &g = bars[i];
      if (g.nyEnd <= stripeStart || g.nyStart >= stripeEnd) continue;
      const int rStart = std::max<int>(0, g.nyStart - stripeStart);
      const int rEnd = std::min<int>(rows, g.nyEnd - stripeStart);

      // Rail + grid ticks (rail spans the full bar height; grid pokes 3 px).
      for (int r = rStart; r < rEnd; ++r) {
        uint16_t *base = txBuffer_ + r * kPanelNativeWidth;
        for (int c = kRailNxStart; c < kRailNxEnd; ++c) base[c] = railColor;
        for (int gi = 0; gi < 3; ++gi) {
          const int gnx = gridNx[gi];
          if (gnx >= kRailNxStart && gnx < kRailNxEnd) base[gnx] = gridColor;
        }
      }

      // Filled body — one gradient column per step, written across every
      // row in the bar's native-Y span.
      const uint16_t *fill = sBarFillCache + i * kBarsHeight;
      for (int step = 0; step < g.h; ++step) {
        const int nx = g.nxBaseline + step * nxDir;
        const uint16_t col = fill[step];
        for (int r = rStart; r < rEnd; ++r) {
          txBuffer_[r * kPanelNativeWidth + nx] = col;
        }
      }

      // Peak-hold marker (core + halo).
      if (g.peakNxCore0 >= 0) {
        for (int r = rStart; r < rEnd; ++r) {
          uint16_t *base = txBuffer_ + r * kPanelNativeWidth;
          base[g.peakNxCore0] = peakCoreColor;
          base[g.peakNxCore1] = peakCoreColor;
          if (g.peakNxHalo >= 0 && g.peakNxHalo < kPanelNativeWidth) {
            base[g.peakNxHalo] = g.peakHaloColor;
          }
        }
      }
    }

    // -- Pattern progress hairline ---------------------------------------
    if (progressNx >= 0 && progressNx < kPanelNativeWidth) {
      const int rBgStart = std::max(0, progBgNyStart - stripeStart);
      const int rBgEnd = std::min(rows, progBgNyEnd - stripeStart);
      for (int r = rBgStart; r < rBgEnd; ++r) {
        txBuffer_[r * kPanelNativeWidth + progressNx] = progBgColor;
      }
      if (progFillNyEnd > progFillNyStart) {
        const int rFStart = std::max(0, progFillNyStart - stripeStart);
        const int rFEnd = std::min(rows, progFillNyEnd - stripeStart);
        for (int r = rFStart; r < rFEnd; ++r) {
          txBuffer_[r * kPanelNativeWidth + progressNx] = progFillColor;
        }
      }
    }

    // -- Play pulse: a 3×3 cross with a brighter center (cheap "♥" glyph) -
    if (pulseNxC >= 0 && pulseNxC < kPanelNativeWidth) {
      const int rCenter = pulseNyC - stripeStart;
      for (int dy = -1; dy <= 1; ++dy) {
        const int r = rCenter + dy;
        if (r < 0 || r >= rows) continue;
        uint16_t *base = txBuffer_ + r * kPanelNativeWidth;
        for (int dx = -1; dx <= 1; ++dx) {
          if (dy != 0 && dx != 0) continue;  // plus shape
          const int nx = pulseNxC + dx;
          if (nx >= 0 && nx < kPanelNativeWidth) base[nx] = pulseColor;
        }
      }
    }

    // -- Text -------------------------------------------------------------
    drawTinyTextNativeStripe(headline, headlineCursorX, kHeadlineLogicalY,
                             kTinyScale, headlineColor, stripeStart, rows,
                             kHeadlineLeftLogicalX, kHeadlineRightLogicalX);
    if (meta.length() > 0) {
      drawTinyTextNativeStripe(meta, kHeadlineLeftLogicalX, kMetaLogicalY, 1,
                               metaColor, stripeStart, rows,
                               kHeadlineLeftLogicalX, kHeadlineRightLogicalX);
    }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    composeUs += micros() - cBegin;
    const uint32_t sBegin = micros();
#endif
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeEnd, txBuffer_)) {
      return;
    }
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    spiUs += micros() - sBegin;
#endif
  }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  static uint32_t sFrameLogMs = 0;
  static uint32_t sFrames = 0;
  ++sFrames;
  if (millis() - sFrameLogMs >= 1000) {
    Serial.printf("[modplayer] compose=%lu spi=%lu total=%lu us fps=%lu rot=%d\n",
                  static_cast<unsigned long>(composeUs),
                  static_cast<unsigned long>(spiUs),
                  static_cast<unsigned long>(micros() - t0),
                  static_cast<unsigned long>(sFrames),
                  rotated ? 1 : 0);
    sFrameLogMs = millis();
    sFrames = 0;
  }
#endif
}

namespace {

inline void formatNote(char *out, uint8_t note) {
  // libxmp: 0 = empty, 1..96 = C-0..B-7, 254 = key-off, 255 = release.
  static const char *kKeys[] = {"C-", "C#", "D-", "D#", "E-", "F-",
                                "F#", "G-", "G#", "A-", "A#", "B-"};
  if (note == 0) { out[0] = '.'; out[1] = '.'; out[2] = '.'; return; }
  if (note == 254) { out[0] = '='; out[1] = '='; out[2] = '='; return; }
  if (note == 255) { out[0] = '^'; out[1] = '^'; out[2] = '^'; return; }
  if (note >= 1 && note <= 96) {
    const int oct = (note - 1) / 12;
    const int key = (note - 1) % 12;
    out[0] = kKeys[key][0];
    out[1] = kKeys[key][1];
    out[2] = static_cast<char>('0' + oct);
    return;
  }
  out[0] = '?'; out[1] = '?'; out[2] = '?';
}

inline void formatHex2(char *out, uint8_t v) {
  static const char kHex[] = "0123456789ABCDEF";
  out[0] = kHex[(v >> 4) & 0xF];
  out[1] = kHex[v & 0xF];
}

inline void formatFx(char *out, uint8_t fxt, uint8_t fxp) {
  // FastTracker-style letter + 2 hex digits. Empty cell shows "...".
  static const char kHex[] = "0123456789ABCDEF";
  if (fxt == 0 && fxp == 0) { out[0] = '.'; out[1] = '.'; out[2] = '.'; return; }
  if (fxt < 10) {
    out[0] = static_cast<char>('0' + fxt);
  } else if (fxt < 36) {
    out[0] = static_cast<char>('A' + (fxt - 10));
  } else {
    out[0] = '?';
  }
  out[1] = kHex[(fxp >> 4) & 0xF];
  out[2] = kHex[fxp & 0xF];
}

}  // namespace

void DisplayManager::renderModulePlayerPatternFrame(
    const char *fileName, const char *title, const char *format,
    int pattern, int patternCount, int pos, int orderCount,
    int currentRow, int patternRows, int bpm, int speed,
    const ModulePatternCell *cells, int visibleRowCount, int chanStride,
    int firstRow, int totalChannels, int populatedChannels) {
  if (!initialized_) return;
  lastRenderKey_ = "";

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  const uint32_t t0 = micros();
  uint32_t composeUs = 0;
  uint32_t spiUs = 0;
#endif

  // ---- Layout (logical 640×172) ---------------------------------------
  constexpr int kHeaderLogicalY = 2;
  constexpr int kHeaderRowH = (kTinyGlyphHeight * 1) + 4;  // 11 px
  constexpr int kGridTopLogicalY = kHeaderLogicalY + kHeaderRowH + 2;  // 15
  constexpr int kRowHeight = (kTinyGlyphHeight * 1) + 1;   // 8 px → 19 rows fit
  constexpr int kRowLabelLeftLX = 2;
  constexpr int kRowLabelRightLX = 22;          // 2 hex chars (12 px) + pad
  constexpr int kCellsStartLX = 26;
  constexpr int kCellWidthLX = 64;
  constexpr int kCellNoteOffsetLX = 0;
  constexpr int kCellInsOffsetLX = 4 * 6;       // 3-char note + 1-char gap
  constexpr int kCellFxOffsetLX = 7 * 6;        // + 2-char ins + 1-char gap
  constexpr int kMaxVisibleChans = 8;

  // ---- Orientation-aware coord helpers (same as the bars renderer). ---
  const bool rotated = uiRotated_;
  auto lxToNy = [rotated](int lx) -> int {
    return rotated ? lx : ((kDisplayWidth - 1) - lx);
  };
  auto lyToNx = [rotated](int ly) -> int {
    return rotated ? ((kDisplayHeight - 1) - ly) : ly;
  };

  // ---- Header line --------------------------------------------------
  String headline;
  // "PAT 03 / 12  POS 05/A8  ROW 11/40  BPM 084 SPD 03"
  headline.reserve(64);
  headline += "PAT ";
  headline += String(pattern);
  if (patternCount > 0) { headline += '/'; headline += String(patternCount); }
  headline += "  POS ";
  headline += String(pos);
  if (orderCount > 0) { headline += '/'; headline += String(orderCount); }
  headline += "  ROW ";
  headline += String(currentRow);
  if (patternRows > 0) { headline += '/'; headline += String(patternRows); }
  if (bpm > 0) { headline += "  BPM "; headline += String(bpm); }
  if (speed > 0) { headline += "  SPD "; headline += String(speed); }

  const String fileText = fileName ? String(fileName) : String("");
  String trackLabel = title ? String(title) : String("");
  trackLabel.trim();
  if (trackLabel.isEmpty()) trackLabel = fileText;
  if (format && format[0]) {
    trackLabel += "  ";
    trackLabel += format;
  }

  // ---- Color palette (MilkyTracker-ish) -----------------------------
  const uint16_t bgColor = panelColor(0x0841);        // very dark navy
  const uint16_t headerColor = panelColor(0xC638);    // soft white
  const uint16_t labelDimColor = panelColor(0x4A69);  // dim row label
  const uint16_t labelMarkColor = panelColor(0x9CD3); // bright every 4 rows
  const uint16_t separatorColor = panelColor(0x18C3); // dark column separator
  const uint16_t noteColor = panelColor(0xE71C);      // light cyan
  const uint16_t noteMarkColor = panelColor(0xFFFF);  // pure white on marker row
  const uint16_t insColor = panelColor(0xFE60);       // amber/yellow
  const uint16_t fxColor = panelColor(0xFC00);        // orange
  const uint16_t emptyColor = panelColor(0x2104);     // very dim "..."
  const uint16_t currentRowBg = panelColor(0x9013);   // dark wine — current row band
  const uint16_t currentRowBgEdge = panelColor(0x4007);
  const uint16_t hintColor = panelColor(0x9CD3);

  // ---- Current-row band native bounds (a horizontal logical band). ----
  const int highlightRowIdx = currentRow - firstRow;
  int highlightLyLo = -1, highlightLyHi = -1;
  if (highlightRowIdx >= 0 && highlightRowIdx < visibleRowCount) {
    highlightLyLo = kGridTopLogicalY + highlightRowIdx * kRowHeight;
    highlightLyHi = highlightLyLo + kRowHeight;
  }
  int highlightNxStart = 0, highlightNxEnd = 0;
  if (highlightLyLo >= 0) {
    const int a = lyToNx(highlightLyLo);
    const int b = lyToNx(highlightLyHi - 1);
    highlightNxStart = std::min(a, b);
    highlightNxEnd = std::max(a, b) + 1;
  }

  // Vertical column separator: 1-px line in logical X between each cell.
  // In native: 1 native row per separator, all panel-wide.
  int separatorNy[kMaxVisibleChans + 1];
  int separatorCount = 0;
  for (int c = 0; c <= kMaxVisibleChans && c <= populatedChannels; ++c) {
    const int sepLx = kCellsStartLX + c * kCellWidthLX - 2;
    if (sepLx < 0 || sepLx >= kDisplayWidth) continue;
    separatorNy[separatorCount++] = lxToNy(sepLx);
  }

  // Header text colors get panel-encoded once; cells call drawTinyTextNativeStripe.
  // "+N more" hint when modules have more channels than fit on screen.
  String moreHint;
  if (totalChannels > kMaxVisibleChans) {
    moreHint = "+";
    moreHint += String(totalChannels - kMaxVisibleChans);
    moreHint += " more";
  }

  // ---- Stripe loop ---------------------------------------------------
  char noteBuf[3], insBuf[2], fxBuf[3];
  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    const int stripeEnd = stripeStart + rows;
    const size_t pixels = static_cast<size_t>(rows) * kPanelNativeWidth;

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    const uint32_t cBegin = micros();
#endif
    // Background — solid dark navy fill (memset-fast loop).
    for (size_t p = 0; p < pixels; ++p) txBuffer_[p] = bgColor;

    // Current-row highlight band — a gradient (edge → centre → edge) reads
    // like a hardware tracker's playing row.
    if (highlightLyLo >= 0) {
      for (int c = highlightNxStart; c < highlightNxEnd; ++c) {
        if (c < 0 || c >= kPanelNativeWidth) continue;
        // Top + bottom rows of the band slightly darker → subtle "lit row" look.
        const bool edge = (c == highlightNxStart) || (c == highlightNxEnd - 1);
        const uint16_t col = edge ? currentRowBgEdge : currentRowBg;
        for (int r = 0; r < rows; ++r) {
          txBuffer_[r * kPanelNativeWidth + c] = col;
        }
      }
    }

    // Column separators (1-px vertical logical lines).
    for (int s = 0; s < separatorCount; ++s) {
      const int ny = separatorNy[s];
      if (ny < stripeStart || ny >= stripeEnd) continue;
      const int r = ny - stripeStart;
      uint16_t *base = txBuffer_ + r * kPanelNativeWidth;
      const int lyLo = kGridTopLogicalY;
      const int lyHi = kGridTopLogicalY + visibleRowCount * kRowHeight;
      const int a = lyToNx(lyLo);
      const int b = lyToNx(lyHi - 1);
      const int nxLo = std::max(0, std::min(a, b));
      const int nxHi = std::min(kPanelNativeWidth - 1, std::max(a, b));
      for (int c = nxLo; c <= nxHi; ++c) base[c] = separatorColor;
    }

    // Header line.
    drawTinyTextNativeStripe(headline, kRowLabelLeftLX, kHeaderLogicalY, 1,
                             headerColor, stripeStart, rows, 0, kDisplayWidth);
    if (!trackLabel.isEmpty()) {
      const int trackWidth = measureTinyTextWidth(trackLabel, 1);
      const int trackX = std::max(kRowLabelLeftLX,
                                  kDisplayWidth - kFooterMarginX - trackWidth);
      drawTinyTextNativeStripe(trackLabel, trackX, kHeaderLogicalY, 1,
                               hintColor, stripeStart, rows, trackX,
                               kDisplayWidth - kFooterMarginX);
    }

    // Hint about hidden channels.
    if (!moreHint.isEmpty()) {
      const int hintWidth = measureTinyTextWidth(moreHint, 1);
      const int hintX = kDisplayWidth - kFooterMarginX - hintWidth;
      const int hintY = kGridTopLogicalY + visibleRowCount * kRowHeight - kTinyGlyphHeight;
      drawTinyTextNativeStripe(moreHint, hintX, hintY, 1, hintColor, stripeStart,
                               rows, hintX, kDisplayWidth - kFooterMarginX);
    }

    // Rows.
    char rowLabel[3] = {0, 0, 0};
    for (int r = 0; r < visibleRowCount; ++r) {
      const int rowIdx = firstRow + r;
      if (rowIdx < 0 || (patternRows > 0 && rowIdx >= patternRows)) continue;
      const int ly = kGridTopLogicalY + r * kRowHeight;
      const bool markerRow = (rowIdx % 4) == 0;
      const bool isCurrentRow = (rowIdx == currentRow);
      const uint16_t rowLabelColor = markerRow ? labelMarkColor : labelDimColor;
      const uint16_t noteCol = (markerRow || isCurrentRow) ? noteMarkColor : noteColor;

      formatHex2(rowLabel, static_cast<uint8_t>(rowIdx & 0xFF));
      String rowLabelStr(rowLabel);
      drawTinyTextNativeStripe(rowLabelStr, kRowLabelLeftLX, ly, 1, rowLabelColor,
                               stripeStart, rows, kRowLabelLeftLX, kRowLabelRightLX);

      const int chans = std::min(populatedChannels, kMaxVisibleChans);
      for (int c = 0; c < chans; ++c) {
        const ModulePatternCell &cell = cells[r * chanStride + c];
        const int cellLx = kCellsStartLX + c * kCellWidthLX;

        formatNote(noteBuf, cell.note);
        formatHex2(insBuf, cell.ins);
        formatFx(fxBuf, cell.fxt, cell.fxp);

        const bool noteEmpty = (cell.note == 0);
        const bool insEmpty = (cell.ins == 0);
        const bool fxEmpty = (cell.fxt == 0 && cell.fxp == 0);
        // Force "  " (spaces) for empty instr so we don't print "00".
        if (insEmpty) { insBuf[0] = '.'; insBuf[1] = '.'; }

        String noteStr; noteStr.concat(noteBuf, 3);
        String insStr; insStr.concat(insBuf, 2);
        String fxStr; fxStr.concat(fxBuf, 3);

        drawTinyTextNativeStripe(noteStr, cellLx + kCellNoteOffsetLX, ly, 1,
                                 noteEmpty ? emptyColor : noteCol, stripeStart,
                                 rows, cellLx, cellLx + kCellWidthLX);
        drawTinyTextNativeStripe(insStr, cellLx + kCellInsOffsetLX, ly, 1,
                                 insEmpty ? emptyColor : insColor, stripeStart,
                                 rows, cellLx, cellLx + kCellWidthLX);
        drawTinyTextNativeStripe(fxStr, cellLx + kCellFxOffsetLX, ly, 1,
                                 fxEmpty ? emptyColor : fxColor, stripeStart,
                                 rows, cellLx, cellLx + kCellWidthLX);
      }
    }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    composeUs += micros() - cBegin;
    const uint32_t sBegin = micros();
#endif
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeEnd, txBuffer_)) {
      return;
    }
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    spiUs += micros() - sBegin;
#endif
  }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  static uint32_t sFrameLogMs = 0;
  static uint32_t sFrames = 0;
  ++sFrames;
  if (millis() - sFrameLogMs >= 1000) {
    Serial.printf("[modpat] compose=%lu spi=%lu total=%lu us fps=%lu rot=%d\n",
                  static_cast<unsigned long>(composeUs),
                  static_cast<unsigned long>(spiUs),
                  static_cast<unsigned long>(micros() - t0),
                  static_cast<unsigned long>(sFrames),
                  rotated ? 1 : 0);
    sFrameLogMs = millis();
    sFrames = 0;
  }
#endif
}

int DisplayManager::libraryLetterAtY(const std::vector<char> &letterAnchors, int y) {
  if (letterAnchors.empty()) {
    return -1;
  }
  const int n = static_cast<int>(letterAnchors.size());
  const int slot = (n * y) / std::max(1, kDisplayHeight);
  return std::max(0, std::min(n - 1, slot));
}

int DisplayManager::libraryScrubLetterAtY(const std::vector<char> &letterAnchors, int y,
                                          int focusIdx) {
  if (letterAnchors.empty()) {
    return -1;
  }
  const int n = static_cast<int>(letterAnchors.size());
  const int visible = std::min(n, kLibraryScrubVisibleLetters);
  const int stripTopPad = kScrubFocusPadY;
  const int usableHeight = std::max(1, kDisplayHeight - stripTopPad * 2);
  const int slotHeight = std::max(1, usableHeight / visible);
  const int slot =
      std::max(0, std::min(visible - 1, (y - stripTopPad) / slotHeight));
  int windowStart = focusIdx - visible / 2;
  if (windowStart < 0) windowStart = 0;
  if (windowStart + visible > n) windowStart = n - visible;
  return std::max(0, std::min(n - 1, windowStart + slot));
}

void DisplayManager::renderLibrary(const std::vector<LibraryItem> &items, size_t selectedIndex,
                                   const std::vector<char> &letterAnchors,
                                   int focusedLetterIdx,
                                   const std::vector<char> &scrubLetters) {
  if (items.empty()) {
    renderCenteredWord("LIBRARY");
    return;
  }

  if (selectedIndex >= items.size()) {
    selectedIndex = items.size() - 1;
  }

  String renderKey = "library|";
  renderKey += String(selectedIndex);
  renderKey += "|b:";
  renderKey += batteryLabel_;
  renderKey += "|d:";
  renderKey += String(darkMode_ ? 1 : 0);
  renderKey += "|n:";
  renderKey += String(nightMode_ ? 1 : 0);
  for (const LibraryItem &item : items) {
    renderKey += "|";
    renderKey += item.title;
    renderKey += "~";
    renderKey += item.subtitle;
    renderKey += "~w";
    renderKey += String(item.wordCount);
    renderKey += "~c";
    renderKey += String(item.chapterCount);
    renderKey += "~p";
    renderKey += String(item.progressPercent);
    for (const String &badge : item.badges) {
      renderKey += "~b";
      renderKey += badge;
    }
  }
  if (!letterAnchors.empty()) {
    renderKey += "|L";
    for (char c : letterAnchors) {
      renderKey += c;
    }
    if (focusedLetterIdx >= 0) {
      renderKey += "|F";
      renderKey += String(focusedLetterIdx);
    }
  }

  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const size_t itemCount = items.size();
  const int usableHeight = std::max(kLibraryRowHeight, virtualHeight - (2 * kLibraryScreenPaddingY));
  const size_t visibleCount =
      std::min(itemCount, static_cast<size_t>(std::max(1, usableHeight / kLibraryRowHeight)));
  size_t firstVisible = 0;
  if (selectedIndex >= visibleCount / 2) {
    firstVisible = selectedIndex - visibleCount / 2;
  }
  if (firstVisible + visibleCount > itemCount) {
    firstVisible = itemCount - visibleCount;
  }

  const int totalHeight = kLibraryRowHeight * static_cast<int>(visibleCount);
  int y = std::max(kLibraryScreenPaddingY, (virtualHeight - totalHeight) / 2);

  clearVirtualBuffer(virtualWidth, virtualHeight);

  const int chipH = kTinyGlyphHeight * kTinyScale + kLibraryChipPadY * 2;

  // Pre-pass: compute max chip-text width per column so chip widths align across rows
  // (numbers in chips line up vertically because text is left-aligned within a fixed-width chip).
  std::vector<int> chipColumnTextWidths;
  auto recordChipWidth = [&](size_t colIdx, const String &label) {
    const int textW = measureTinyTextWidth(label, kTinyScale);
    if (chipColumnTextWidths.size() <= colIdx) {
      chipColumnTextWidths.resize(colIdx + 1, 0);
    }
    if (textW > chipColumnTextWidths[colIdx]) {
      chipColumnTextWidths[colIdx] = textW;
    }
  };
  for (const LibraryItem &item : items) {
    size_t col = 0;
    if (item.progressPercent >= 0) {
      recordChipWidth(col++, String(item.progressPercent) + "%");
    }
    if (item.chapterCount > 0) {
      recordChipWidth(col++, String(item.chapterCount) + "ch");
    }
    for (const String &badge : item.badges) {
      if (badge.isEmpty()) continue;
      recordChipWidth(col++, badge);
    }
  }

  for (size_t row = 0; row < visibleCount; ++row) {
    const size_t itemIndex = firstVisible + row;
    const LibraryItem &item = items[itemIndex];
    const bool selected = itemIndex == selectedIndex;
    const uint16_t titleColor = selected ? focusColor() : wordColor();
    const uint16_t subtitleColor = blendOverBackground(titleColor, kLibrarySubtitleAlpha);
    const uint16_t chipBg = blendOverBackground(titleColor, kLibraryChipBgAlpha);
    const int rowY = y + static_cast<int>(row) * kLibraryRowHeight;

    if (selected) {
      fillVirtualRect(10, rowY + 3, 5, kLibraryRowHeight - 6, selectedBarColor());
    }

    std::vector<String> chipLabels;
    chipLabels.reserve(3 + item.badges.size());
    if (item.progressPercent >= 0) {
      chipLabels.push_back(String(item.progressPercent) + "%");
    }
    if (item.chapterCount > 0) {
      chipLabels.push_back(String(item.chapterCount) + "ch");
    }
    for (const String &badge : item.badges) {
      if (!badge.isEmpty()) {
        chipLabels.push_back(badge);
      }
    }

    const int rightContentEdge =
        letterAnchors.empty() ? virtualWidth : (virtualWidth - kLibraryLetterStripWidth);
    int chipsLeftEdge = rightContentEdge - kLibraryChipsRightMargin;
    const int chipY = rowY + (kLibraryRowHeight - chipH) / 2;
    int rightCursor = chipsLeftEdge;
    const int chipRadius = std::min(6, chipH / 2);
    for (size_t ci = 0; ci < chipLabels.size(); ++ci) {
      const String &label = chipLabels[ci];
      const int columnTextW = ci < chipColumnTextWidths.size()
                                  ? chipColumnTextWidths[ci]
                                  : measureTinyTextWidth(label, kTinyScale);
      const int chipW = columnTextW + kLibraryChipPadX * 2;
      const int chipX = rightCursor - chipW;
      fillRoundedRect(chipX, chipY, chipW, chipH, chipRadius, chipBg);
      drawTinyTextAt(label, chipX + kLibraryChipPadX, chipY + kLibraryChipPadY, titleColor,
                     kTinyScale);
      rightCursor = chipX - kLibraryChipGap;
    }
    if (!chipLabels.empty()) {
      chipsLeftEdge = rightCursor;
    }

    const int maxWidth = std::max(40, chipsLeftEdge - kLibraryInsetX - 8);
    const String title = fitTinyText(item.title, maxWidth, kTinyScale);
    if (item.subtitle.isEmpty()) {
      drawTinyTextAt(title, kLibraryInsetX, rowY + 12, titleColor, kTinyScale);
      continue;
    }

    drawTinyTextAt(title, kLibraryInsetX, rowY + kLibraryTitleYOffset, titleColor, kTinyScale);
    drawTinyTextAt(fitTinyText(item.subtitle, maxWidth, kTinyScale), kLibraryInsetX,
                   rowY + kLibrarySubtitleYOffset, subtitleColor, kTinyScale);
  }

  if (!letterAnchors.empty()) {
    const bool scrubbing = focusedLetterIdx >= 0;
    const std::vector<char> &activeLetters =
        (scrubbing && !scrubLetters.empty()) ? scrubLetters : letterAnchors;
    const int stripWidth = scrubbing ? kLibraryLetterScrubWidth : kLibraryLetterStripWidth;
    const int stripX = virtualWidth - stripWidth;
    const int n = static_cast<int>(activeLetters.size());
    const uint16_t letterColor = blendOverBackground(wordColor(), 200);
    const uint16_t focusColor = wordColor();

    if (scrubbing) {
      const uint16_t panelBg = blendOverBackground(wordColor(), 40);
      fillVirtualRect(stripX, 0, stripWidth, virtualHeight, panelBg);
      const int visible = std::min(n, kLibraryScrubVisibleLetters);
      const int stripTopPad = kScrubFocusPadY;
      const int usableHeight = std::max(1, virtualHeight - stripTopPad * 2);
      const int slotHeight = std::max(1, usableHeight / visible);
      int windowStart = focusedLetterIdx - visible / 2;
      if (windowStart < 0) windowStart = 0;
      if (windowStart + visible > n) windowStart = n - visible;
      const int letterScale = kTinyScale;
      const int innerX = stripX + 12;
      for (int slot = 0; slot < visible; ++slot) {
        const int letterIdx = windowStart + slot;
        if (letterIdx < 0 || letterIdx >= n) continue;
        const int slotY =
            stripTopPad + slotHeight * slot + (slotHeight - kTinyGlyphHeight * letterScale) / 2;
        const String label = String(activeLetters[letterIdx]);
        const bool isFocus = letterIdx == focusedLetterIdx;
        if (isFocus) {
          const int textW = measureTinyTextWidth(label, letterScale);
          const int boxW = textW + kScrubFocusPadX * 2;
          const int boxH = kTinyGlyphHeight * letterScale + kScrubFocusPadY * 2;
          const int boxX = innerX - kScrubFocusPadX;
          const int boxY = slotY - kScrubFocusPadY;
          fillRoundedRect(boxX, boxY, boxW, boxH,
                          std::min(kScrubFocusMaxRadius, boxH / 2),
                          blendOverBackground(focusColor, kScrubFocusBgAlpha));
          drawTinyTextAt(label, innerX, slotY, focusColor, letterScale);
        } else {
          drawTinyTextAt(label, innerX, slotY, letterColor, letterScale);
        }
      }
    } else {
      const int slotHeight = std::max(1, virtualHeight / std::max(1, n));
      const int innerX = stripX + 6;
      for (int i = 0; i < n; ++i) {
        const int slotY = (slotHeight * i) + (slotHeight - kTinyGlyphHeight) / 2;
        const String label = String(letterAnchors[i]);
        drawTinyTextAt(label, innerX, slotY, letterColor, 1);
      }
    }
  }

  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderStatus(const String &title, const String &line1, const String &line2) {
  const String renderKey = "status|" + title + "|" + line1 + "|" + line2 + "|b:" +
                           batteryLabel_ + "|d:" + String(darkMode_ ? 1 : 0) + "|n:" +
                           String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int titleY = std::max(0, (virtualHeight - kBaseGlyphHeight) / 2 - 26);
  const int line1Y = std::min(virtualHeight - kTinyGlyphHeight * kTinyScale,
                              titleY + kBaseGlyphHeight + 22);
  const int line2Y = std::min(virtualHeight - kTinyGlyphHeight * kTinyScale,
                              line1Y + kTinyGlyphHeight * kTinyScale + 10);

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawWordLine(title, titleY, wordColor());
  const int textMaxWidth = virtualWidth - 24;
  auto fittedScale = [&](const String &text) -> int {
    if (measureTinyTextWidth(text, kTinyScale) <= textMaxWidth) return kTinyScale;
    return 1;
  };
  if (!line1.isEmpty()) {
    const int s = fittedScale(line1);
    drawTinyTextCentered(fitTinyText(line1, textMaxWidth, s), line1Y, dimColor(), s);
  }
  if (!line2.isEmpty()) {
    const int s = fittedScale(line2);
    drawTinyTextCentered(fitTinyText(line2, textMaxWidth, s), line2Y, focusColor(), s);
  }
  drawBatteryBadge();

  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

void DisplayManager::renderProgress(const String &title, const String &line1, const String &line2,
                                    int progressPercent) {
  progressPercent = std::max(-1, std::min(100, progressPercent));
  const String renderKey =
      "progress|" + title + "|" + line1 + "|" + line2 + "|" + String(progressPercent) +
      "|b:" + batteryLabel_ + "|d:" + String(darkMode_ ? 1 : 0) + "|n:" +
      String(nightMode_ ? 1 : 0);
  if (!initialized_ || renderKey == lastRenderKey_) {
    return;
  }

  lastRenderKey_ = renderKey;

  const int scale = 1;
  const int virtualWidth = kDisplayWidth;
  const int virtualHeight = kDisplayHeight;
  const int titleY = std::max(0, (virtualHeight - kBaseGlyphHeight) / 2 - 34);
  const int line1Y = std::min(virtualHeight - kTinyGlyphHeight * kTinyScale,
                              titleY + kBaseGlyphHeight + 18);
  const int line2Y = std::min(virtualHeight - kTinyGlyphHeight * kTinyScale,
                              line1Y + kTinyGlyphHeight * kTinyScale + 10);
  const int barWidth = std::min(300, virtualWidth - 48);
  const int barHeight = 8;
  const int barX = std::max(0, (virtualWidth - barWidth) / 2);
  const int barY = std::min(virtualHeight - barHeight - 8,
                            line2Y + kTinyGlyphHeight * kTinyScale + 14);

  clearVirtualBuffer(virtualWidth, virtualHeight);
  drawWordLine(title, titleY, wordColor());
  const int textMaxWidth = virtualWidth - 24;
  auto fittedScale = [&](const String &text) -> int {
    if (measureTinyTextWidth(text, kTinyScale) <= textMaxWidth) return kTinyScale;
    return 1;
  };
  if (!line1.isEmpty()) {
    const int s = fittedScale(line1);
    drawTinyTextCentered(fitTinyText(line1, textMaxWidth, s), line1Y, dimColor(), s);
  }
  if (!line2.isEmpty()) {
    const int s = fittedScale(line2);
    drawTinyTextCentered(fitTinyText(line2, textMaxWidth, s), line2Y, focusColor(), s);
  }

  if (progressPercent >= 0) {
    fillVirtualRect(barX, barY, barWidth, barHeight, dimColor());
    fillVirtualRect(barX + 1, barY + 1, barWidth - 2, barHeight - 2, backgroundColor());
    const int fillWidth = std::max(1, ((barWidth - 2) * progressPercent) / 100);
    fillVirtualRect(barX + 1, barY + 1, fillWidth, barHeight - 2, focusColor());
  }

  drawBatteryBadge();
  flushScaledFrame(scale, virtualWidth, virtualHeight);
}

namespace {

inline void putPixel(uint16_t *buf, int bufW, int bufH, int x, int y, uint16_t color) {
  if (x < 0 || x >= bufW || y < 0 || y >= bufH) return;
  buf[y * bufW + x] = color;
}

// Horizontal span at row y from xStart to xStart+len-1. Tight inner loop —
// no per-pixel branch, compiler vectorises the store.
inline IRAM_ATTR void hLine(uint16_t *buf, int bufW, int bufH, int xStart, int y, int len,
                            uint16_t color) {
  if (y < 0 || y >= bufH || len <= 0) return;
  int x1 = xStart;
  int x2 = xStart + len - 1;
  if (x1 < 0) x1 = 0;
  if (x2 >= bufW) x2 = bufW - 1;
  if (x1 > x2) return;
  uint16_t *row = buf + y * bufW + x1;
  const int n = x2 - x1 + 1;
  for (int i = 0; i < n; ++i) row[i] = color;
}

// Filled circle via Bresenham — emits 4 horizontal spans per octant step.
// Replaces the prior O(r²) per-pixel containment loop with O(r) span ops.
void fillCircleSolid(uint16_t *buf, int bufW, int bufH, int cx, int cy, int r,
                     uint16_t color) {
  if (r <= 0) {
    putPixel(buf, bufW, bufH, cx, cy, color);
    return;
  }
  int x = r;
  int y = 0;
  int err = 1 - r;
  while (x >= y) {
    hLine(buf, bufW, bufH, cx - x, cy + y, 2 * x + 1, color);
    hLine(buf, bufW, bufH, cx - x, cy - y, 2 * x + 1, color);
    hLine(buf, bufW, bufH, cx - y, cy + x, 2 * y + 1, color);
    hLine(buf, bufW, bufH, cx - y, cy - x, 2 * y + 1, color);
    ++y;
    if (err < 0) {
      err += 2 * y + 1;
    } else {
      --x;
      err += 2 * (y - x) + 1;
    }
  }
}

}  // namespace

// Native-stripe screensaver renderer. Bypasses virtualFrame_ and the rotated
// per-pixel transpose entirely; composes each panel-native stripe directly
// into txBuffer_ (DMA-capable internal RAM) and pushes it.
//
// Coordinate mapping — single source of truth:
//   The generic UI path renders into virtualFrame_ in logical 640×172 space,
//   then flushScaledFrame rotates into the panel's 172×640 native memory:
//       virtualFrame_[(kDisplayHeight - 1 - nativeX) * VW + nativeY]
//   Inverse (logical → native) is therefore:
//       nativeY = logicalX
//       nativeX = (kDisplayHeight - 1) - logicalY     // = 171 - logicalY
//   We project the screensaver in logical space (sx in [0,640), sy in [0,172))
//   so we apply that inverse once per primitive, then never transpose again.
void DisplayManager::renderScreensaverFrame(Screensaver &saver) {
  if (!initialized_) return;
  // Animation forces a full repaint each frame.
  lastRenderKey_ = "";
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  const uint32_t t0 = micros();
#endif
  saver.tick();
  saver.sortPoints();
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  const uint32_t t1 = micros();
  uint32_t composeUs = 0;
  uint32_t spiUs = 0;
#endif

  // Project once into native space; the stripe loop only does bbox-cull + draw.
  // 165 sprites × 12 bytes = ~2 KB on stack.
  struct Sprite {
    int16_t cnX;        // native X (0..171) — center of dim outer rim
    int16_t cnY;        // native Y (0..639)
    int16_t radius;     // 0 for stars, 1..kRadiusMax for dots
    int16_t coreShift;  // bright-core offset toward "light" (radius / 4)
    int16_t hiOffset;   // 0 unless dot is large enough for a specular speck
    uint16_t color;     // bright/lit color, already panelColor()-encoded
    uint16_t dimColor;  // ~50% of color — outer rim, reads as the shadow side
    uint16_t hiColor;   // white specular if hiOffset > 0
  };
  Sprite sprites[Screensaver::kStarCount + Screensaver::kPointCount];
  int spriteCount = 0;

  constexpr int kVirtualWidth = kDisplayWidth;     // 640
  constexpr int kVirtualHeight = kDisplayHeight;   // 172

  // Stars first → drawn first inside each stripe → behind dots.
  // Stars now compute their own screen coords + brightness in
  // Screensaver::tick() so we can support multiple star modes (3D-forward,
  // parallax-horizontal) without branching here.
  const auto *stars = saver.stars();
  for (size_t i = 0; i < saver.starCount(); ++i) {
    const auto &s = stars[i];
    if (s.brightness == 0) continue;  // off-screen / culled
    if (s.sx < 0 || s.sx >= kVirtualWidth || s.sy < 0 || s.sy >= kVirtualHeight) continue;
    const uint16_t c5 = static_cast<uint16_t>(s.brightness) >> 3;
    const uint16_t c6 = static_cast<uint16_t>(s.brightness) >> 2;
    Sprite &sp = sprites[spriteCount++];
    sp.cnX = static_cast<int16_t>((kVirtualHeight - 1) - s.sy);
    sp.cnY = static_cast<int16_t>(s.sx);
    sp.radius = 0;
    sp.coreShift = 0;
    sp.hiOffset = 0;
    sp.color = panelColor(static_cast<uint16_t>((c5 << 11) | (c6 << 5) | c5));
    sp.dimColor = 0;
    sp.hiColor = 0;
  }

  // Dots back-to-front (drawOrder() is sorted descending by cz).
  const uint16_t *palette = Screensaver::palette();
  const uint16_t *order = saver.drawOrder();
  const auto *allPoints = saver.points();
  // Tuned for the 640×172 landscape panel. Bumped from 110 → 180 so the
  // cluster fills more of the panel; combined with kCameraZ=3.0 in
  // Screensaver::tick(), nearest dots project to ~10 px radius.
  constexpr float kFocal = 180.0f;
  // Per-dot radius scaler. Cap=13 lets near dots feel substantial; the
  // 6³ grid spacing (0.6 model units) projects to ≥36 px between adjacent
  // dots at typical depth, so even max-radius dots don't overlap.
  constexpr float kRadiusScale = 11.0f;
  constexpr int kRadiusMax = 13;
  for (size_t i = 0; i < saver.pointCount(); ++i) {
    const auto &p = allPoints[order[i]];
    if (p.cz <= 0.1f) continue;
    const float invCz = 1.0f / p.cz;
    const int sx = kVirtualWidth / 2 + static_cast<int>(p.cx * kFocal * invCz);
    const int sy = kVirtualHeight / 2 + static_cast<int>(p.cy * kFocal * invCz);
    int radius = static_cast<int>(kRadiusScale * invCz);
    if (radius < 1) radius = 1;
    if (radius > kRadiusMax) radius = kRadiusMax;
    // Z-brightness depth cue — multiply each RGB565 channel by p.brightness
    // (0..255). Pre-shifted in fixed-point to avoid a divide per dot.
    const uint16_t base = palette[p.colorIndex % Screensaver::kPaletteSize];
    const uint16_t scale = p.brightness;
    const uint16_t r5 = static_cast<uint16_t>((((base >> 11) & 0x1F) * scale) >> 8);
    const uint16_t g6 = static_cast<uint16_t>((((base >>  5) & 0x3F) * scale) >> 8);
    const uint16_t b5 = static_cast<uint16_t>(( (base        & 0x1F) * scale) >> 8);
    const uint16_t shaded = static_cast<uint16_t>((r5 << 11) | (g6 << 5) | b5);
    // dimColor = shaded × 0.5 via the RGB565 halve trick: mask the LSB of
    // each channel so the right-shift doesn't bleed across channels.
    const uint16_t dim = static_cast<uint16_t>((shaded & 0xF7DEu) >> 1);
    Sprite &sp = sprites[spriteCount++];
    sp.cnX = static_cast<int16_t>((kVirtualHeight - 1) - sy);
    sp.cnY = static_cast<int16_t>(sx);
    sp.radius = static_cast<int16_t>(radius);
    // Bright core shifted toward the "light" (same axis as the specular).
    // Below radius 3 the shift can't show a crescent, so we skip it.
    sp.coreShift = (radius >= 3) ? static_cast<int16_t>(radius / 4) : 0;
    // Specular highlight at +hiOffset in native coords. Threshold lowered
    // from 6 → 4 so most non-tiny dots get the glint that sells the sphere.
    sp.hiOffset = (radius >= 4) ? static_cast<int16_t>(radius / 3) : 0;
    sp.color = panelColor(shaded);
    sp.dimColor = panelColor(dim);
    sp.hiColor = (radius >= 4) ? panelColor(0xFFFF) : 0;
  }

  // Stripe loop. txBuffer_ is the DMA-capable internal-RAM scratch already
  // sized for kPanelNativeWidth × kMaxChunkPhysicalRows (172 × ~47 = ~16 KB).
  // The existing putPixel/fillCircleSolid/hLine helpers take (buf,bufW,bufH)
  // and clip Y on every span, so calling them with bufH=stripeRows naturally
  // discards any out-of-stripe rows — no extra per-primitive math needed.
  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    const size_t bytes = static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t);

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    const uint32_t cBegin = micros();
#endif
    std::memset(txBuffer_, 0, bytes);

    const int stripeEnd = stripeStart + rows;
    for (int s = 0; s < spriteCount; ++s) {
      const Sprite &sp = sprites[s];
      // Bbox cull on native Y (= stripe axis). Use radius+coreShift since
      // the bright core may extend slightly past the rim on the lit side.
      const int extentY = sp.radius + sp.coreShift;
      if (sp.cnY + extentY < stripeStart || sp.cnY - extentY >= stripeEnd) continue;
      const int localCy = sp.cnY - stripeStart;
      if (sp.radius == 0) {
        putPixel(txBuffer_, kPanelNativeWidth, rows, sp.cnX, localCy, sp.color);
        continue;
      }
      // 3-tone shaded sphere: dim outer rim → bright inner core shifted
      // toward the light → small white specular at the lit pole. The
      // shadow side of the dim disc remains visible as a crescent because
      // the bright core is offset by coreShift along (+cnX, +cnY).
      fillCircleSolid(txBuffer_, kPanelNativeWidth, rows, sp.cnX, localCy, sp.radius,
                      sp.dimColor);
      const int innerR = sp.radius - 1;
      if (innerR >= 0) {
        fillCircleSolid(txBuffer_, kPanelNativeWidth, rows,
                        sp.cnX + sp.coreShift, localCy + sp.coreShift,
                        innerR, sp.color);
      }
      if (sp.hiOffset > 0) {
        fillCircleSolid(txBuffer_, kPanelNativeWidth, rows,
                        sp.cnX + sp.hiOffset, localCy + sp.hiOffset,
                        sp.hiOffset, sp.hiColor);
      }
    }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    composeUs += micros() - cBegin;
    const uint32_t sBegin = micros();
#endif
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
    spiUs += micros() - sBegin;
#endif
  }

#if defined(SCREENSAVER_PROFILING) && SCREENSAVER_PROFILING
  const uint32_t tEnd = micros();
  static uint32_t sFrameLogMs = 0;
  static uint32_t sFrames = 0;
  static uint32_t sMinHeap = UINT32_MAX;
  const uint32_t freeNow = ESP.getFreeHeap();
  if (freeNow < sMinHeap) sMinHeap = freeNow;
  ++sFrames;
  if (millis() - sFrameLogMs >= 1000) {
    Serial.printf("[saver] tick=%lu compose=%lu spi=%lu total=%lu us fps=%lu free=%u min=%u\n",
                  static_cast<unsigned long>(t1 - t0),
                  static_cast<unsigned long>(composeUs),
                  static_cast<unsigned long>(spiUs),
                  static_cast<unsigned long>(tEnd - t0),
                  static_cast<unsigned long>(sFrames),
                  static_cast<unsigned int>(freeNow),
                  static_cast<unsigned int>(sMinHeap));
    sFrameLogMs = millis();
    sFrames = 0;
  }
#endif
}

// ============================================================================
// Demoscene-style demos. All follow the native-stripe pattern: bypass
// virtualFrame_, compose directly into txBuffer_ in panel-native orientation,
// push each stripe via drawBitmap. Coordinate mapping is the same as the
// screensaver: nativeY = logicalX, nativeX = (kDisplayHeight − 1) − logicalY.
// ============================================================================

namespace {

inline IRAM_ATTR void fillStripeRowSpan(uint16_t *dst, int len, uint16_t color) {
  for (int i = 0; i < len; ++i) dst[i] = color;
}

// Memcpy the same 172-pixel row into every native row of the chunk. Hot path
// for column-uniform effects (rasterbars, copper).
inline void blastColumnsAcrossStripe(uint16_t *stripeBuf, const uint16_t *cols,
                                     int rows) {
  for (int r = 0; r < rows; ++r) {
    std::memcpy(stripeBuf + r * kPanelNativeWidth, cols,
                kPanelNativeWidth * sizeof(uint16_t));
  }
}

}  // namespace

void DisplayManager::renderRasterbarsFrame(const Rasterbars &rb) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // Pre-compute the 172-entry native-column color array for this frame.
  // Logical bars (constant logical Y) become vertical native columns
  // (constant nativeX) thanks to the rotation. Background = animated copper
  // gradient sampled per native column.
  uint16_t cols[kPanelNativeWidth];
  for (int nx = 0; nx < kPanelNativeWidth; ++nx) {
    // Copper: smooth rainbow scrolling through the 172-px height. Use a
    // fast triangle-wave-ish hue derived from nativeX + phase.
    const float u = (static_cast<float>(nx) / kPanelNativeWidth) + rb.copperPhase();
    const float ru = u - floorf(u);  // [0,1)
    // 3-stop ramp magenta → cyan → yellow then back, gives Amiga copper feel.
    uint8_t r, g, b;
    if (ru < 0.333f) {
      const float t = ru * 3.0f;
      r = static_cast<uint8_t>(255.0f * (1.0f - t));
      g = 0;
      b = static_cast<uint8_t>(255.0f * t);
    } else if (ru < 0.666f) {
      const float t = (ru - 0.333f) * 3.0f;
      r = 0;
      g = static_cast<uint8_t>(255.0f * t);
      b = static_cast<uint8_t>(255.0f * (1.0f - t * 0.5f));
    } else {
      const float t = (ru - 0.666f) * 3.0f;
      r = static_cast<uint8_t>(255.0f * t);
      g = static_cast<uint8_t>(255.0f * (1.0f - t * 0.5f));
      b = 0;
    }
    // Dim the background so bars pop on top.
    r >>= 2; g >>= 2; b >>= 2;
    cols[nx] = panelColor(static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)));
  }

  // Now stamp each bar into its native-column range.
  for (int i = 0; i < Rasterbars::kBarCount; ++i) {
    const int centerLogicalY = rb.barCenterY(i);
    const int half = rb.barHalfThickness(i);
    // logical Y → native X = 171 − logicalY. Symmetric range stays the same.
    const int centerNativeX = (kDisplayHeight - 1) - centerLogicalY;
    const int x0 = std::max(0, centerNativeX - half);
    const int x1 = std::min(kPanelNativeWidth - 1, centerNativeX + half);
    // Pre-compute full + half-bright variants once per bar.
    const uint16_t baseRaw = rb.barColor(i);
    const uint16_t baseR = (baseRaw >> 11) & 0x1F;
    const uint16_t baseG = (baseRaw >> 5) & 0x3F;
    const uint16_t baseB = baseRaw & 0x1F;
    const uint16_t dimRaw = static_cast<uint16_t>(((baseR >> 1) << 11) |
                                                  ((baseG >> 1) << 5) | (baseB >> 1));
    const uint16_t baseColor = panelColor(baseRaw);
    const uint16_t dimColor = panelColor(dimRaw);
    for (int x = x0; x <= x1; ++x) {
      const int distFromCenter = std::abs(x - centerNativeX);
      cols[x] = (distFromCenter >= half - 1) ? dimColor : baseColor;
    }
  }

  // Stripe loop — every row of every stripe is the same column array.
  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    blastColumnsAcrossStripe(txBuffer_, cols, rows);
    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderStarfieldFrame(const Starfield &sf) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // Project star positions into native space once per frame.
  struct NativeStar {
    int16_t cnX;
    int16_t cnY;
    uint8_t brightness;
    uint8_t layer;
  };
  NativeStar projected[Starfield::kStarCount];
  const auto *stars = sf.stars();
  for (size_t i = 0; i < sf.starCount(); ++i) {
    const auto &s = stars[i];
    NativeStar &n = projected[i];
    const int sx = static_cast<int>(s.x);
    n.cnX = static_cast<int16_t>((kDisplayHeight - 1) - s.y);
    n.cnY = static_cast<int16_t>(sx);
    n.brightness = s.brightness;
    n.layer = s.layer;
  }

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    const size_t bytes = static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t);
    std::memset(txBuffer_, 0, bytes);

    const int stripeEnd = stripeStart + rows;
    for (int i = 0; i < Starfield::kStarCount; ++i) {
      const NativeStar &n = projected[i];
      if (n.cnY < stripeStart || n.cnY >= stripeEnd) continue;
      const int localY = n.cnY - stripeStart;
      const uint8_t b = n.brightness;
      const uint16_t c5 = static_cast<uint16_t>(b) >> 3;
      const uint16_t c6 = static_cast<uint16_t>(b) >> 2;
      const uint16_t color =
          panelColor(static_cast<uint16_t>((c5 << 11) | (c6 << 5) | c5));
      putPixel(txBuffer_, kPanelNativeWidth, rows, n.cnX, localY, color);
      // Closer layers get a 2-pixel horizontal streak so they read as
      // motion against the dim background.
      if (n.layer >= 2 && n.cnY + 1 < stripeEnd) {
        putPixel(txBuffer_, kPanelNativeWidth, rows, n.cnX, localY + 1, color);
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderSineScrollerFrame(const SineScroller &ss) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // Scaled tiny font: 5×7 source × 4 → 20×28 logical glyph.
  constexpr int kScale = 4;
  constexpr int kGlyphW = kTinyGlyphWidth * kScale;     // 20
  constexpr int kGlyphH = kTinyGlyphHeight * kScale;    // 28
  constexpr int kCharPitch = (kTinyGlyphWidth + kTinyGlyphSpacing) * kScale;  // 24
  constexpr int kBaselineY = (kDisplayHeight - kGlyphH) / 2;  // ~72 → text vertical center

  const char *msg = ss.message();
  if (msg == nullptr || msg[0] == '\0') return;
  const int msgLen = static_cast<int>(strlen(msg));
  const int totalPx = msgLen * kCharPitch;

  // Wrap scroll so the message loops cleanly. scrollPx grows monotonically;
  // headPx is the X (logical) of the LEFT edge of the first glyph this frame.
  const float scrollPx = ss.scrollPx();
  const float wrap = static_cast<float>(totalPx + kDisplayWidth);
  float wrapped = scrollPx;
  while (wrapped >= wrap) wrapped -= wrap;
  // headPx starts at +kDisplayWidth (off-screen right) and decreases.
  const int headPx = kDisplayWidth - static_cast<int>(wrapped);

  // Per-glyph wave amplitude + color cycle. Use the demo's wave phase
  // directly so the tick rate matches.
  const float wave = ss.wavePhase();

  // Orientation-aware mapping — same lambdas as the module player so the
  // demo renders right-way-up + non-mirrored regardless of which way the
  // display-flip preference resolves. Previous version hardcoded the
  // uiRotated_=false formula and produced a horizontal mirror on the
  // default 180-rotated device ("NANO RSVP" → "PVSR ONAN").
  const bool rotated = uiRotated_;
  auto lxToNy = [rotated](int lx) -> int {
    return rotated ? lx : ((kDisplayWidth - 1) - lx);
  };
  auto lyToNx = [rotated](int ly) -> int {
    return rotated ? ((kDisplayHeight - 1) - ly) : ly;
  };

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
    const size_t bytes = static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t);
    std::memset(txBuffer_, 0, bytes);

    const int stripeEnd = stripeStart + rows;
    for (int gi = 0; gi < msgLen; ++gi) {
      const int glyphLeftLogicalX = headPx + gi * kCharPitch;
      // Skip glyphs entirely off-screen on the right (logical X >= 640) or
      // past the left edge (logicalX + width <= 0).
      if (glyphLeftLogicalX >= kDisplayWidth) continue;
      if (glyphLeftLogicalX + kGlyphW <= 0) continue;
      // Per-glyph vertical sine bounce. Amplitude ~30 px, smooth phase.
      const float yOff = sinf(wave + static_cast<float>(gi) * 0.4f) * 30.0f;
      const int glyphTopLogicalY = kBaselineY + static_cast<int>(yOff);

      // Native-Y range this glyph occupies (orientation-aware). Take
      // min/max of the two endpoint mappings so the iteration order is
      // always ascending native-Y.
      const int nyA = lxToNy(glyphLeftLogicalX);
      const int nyB = lxToNy(glyphLeftLogicalX + kGlyphW - 1);
      const int glyphNyStart = std::min(nyA, nyB);
      const int glyphNyEnd = std::max(nyA, nyB) + 1;
      const int nyStart = std::max(stripeStart, glyphNyStart);
      const int nyEnd = std::min(stripeEnd, glyphNyEnd);
      if (nyStart >= nyEnd) continue;

      const uint8_t *rows5x7 = tinyRowsFor(msg[gi]);
      // Cycle a punchy color per glyph index + slow phase shift.
      const float colorWave = wave * 0.5f + static_cast<float>(gi) * 0.2f;
      const uint8_t r = static_cast<uint8_t>(128 + 127 * sinf(colorWave));
      const uint8_t g = static_cast<uint8_t>(128 + 127 * sinf(colorWave + 2.094f));
      const uint8_t b = static_cast<uint8_t>(128 + 127 * sinf(colorWave + 4.188f));
      const uint16_t color =
          panelColor(static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)));

      for (int ny = nyStart; ny < nyEnd; ++ny) {
        // Recover the logical-X for this native row, then derive the
        // glyph column. With the orientation-aware mapping this is the
        // straight ratio — no inversions needed.
        const int lx = rotated ? ny : ((kDisplayWidth - 1) - ny);
        const int glyphCol = (lx - glyphLeftLogicalX) / kScale;
        if (glyphCol < 0 || glyphCol >= kTinyGlyphWidth) continue;
        const uint8_t bitMask = static_cast<uint8_t>(0x10 >> glyphCol);  // bit4 = col0
        const int localStripeRow = ny - stripeStart;
        // Each set source bit fills a kScale × kScale block in the panel.
        for (int gr = 0; gr < kTinyGlyphHeight; ++gr) {
          if ((rows5x7[gr] & bitMask) == 0) continue;
          const int logicalY0 = glyphTopLogicalY + gr * kScale;
          const int logicalY1 = logicalY0 + kScale;  // exclusive
          // Map the kScale-tall row span into a native-X span.
          const int nxA = lyToNx(logicalY0);
          const int nxB = lyToNx(logicalY1 - 1);
          const int xLo = std::max(0, std::min(nxA, nxB));
          const int xHi = std::min(kPanelNativeWidth - 1, std::max(nxA, nxB));
          if (xLo > xHi) continue;
          uint16_t *dst = txBuffer_ + localStripeRow * kPanelNativeWidth + xLo;
          for (int x = xLo; x <= xHi; ++x) *dst++ = color;
        }
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderPlasmaFrame(const Plasma &pl) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // Per-pixel: combine 4 sin sources at sample point (logicalX, logicalY),
  // map to palette index, look up RGB565. Hot loop runs ~110 k times per
  // frame; we hoist the palette + LUT pointers and compute LUT indices with
  // wrap-around bitmasks.
  const int8_t *sinLut = Plasma::sinLutPtr();
  const uint16_t *palette = pl.palette();
  // Convert phase floats into sin-LUT integer offsets (0..255 wraps).
  const int paOff = static_cast<int>(pl.pa() * 40.7f) & 0xFF;
  const int pbOff = static_cast<int>(pl.pb() * 35.3f) & 0xFF;
  const int pcOff = static_cast<int>(pl.pc() * 28.1f) & 0xFF;
  const int pdOff = static_cast<int>(pl.pd() * 51.2f) & 0xFF;
  const uint8_t paletteShift = pl.paletteShift();

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    // Pre-cache the panelColor()-applied palette once per stripe so the
    // inner loop does only LUT lookup + store.
    static uint16_t cachedPalette[Plasma::kPaletteSize];
    static const uint16_t *lastPaletteSrc = nullptr;
    static uint8_t lastShift = 0xFF;
    if (palette != lastPaletteSrc || paletteShift != lastShift) {
      for (int i = 0; i < Plasma::kPaletteSize; ++i) {
        cachedPalette[i] = panelColor(palette[(i + paletteShift) & 0xFF]);
      }
      lastPaletteSrc = palette;
      lastShift = paletteShift;
    }

    for (int ny = 0; ny < rows; ++ny) {
      const int nativeY = stripeStart + ny;          // = logicalX (after rotation)
      // Pre-compute the two sine contributions that depend only on nativeY.
      const int8_t sA = sinLut[(nativeY + paOff) & 0xFF];
      const int8_t sC = sinLut[((nativeY * 2) + pcOff) & 0xFF];
      uint16_t *row = txBuffer_ + ny * kPanelNativeWidth;
      for (int nx = 0; nx < kPanelNativeWidth; ++nx) {
        // logicalY = (kDisplayHeight - 1) - nativeX (we don't even need
        // logicalY explicitly; just use nx with the mirror baked in).
        const int8_t sB = sinLut[(nx + pbOff) & 0xFF];
        const int8_t sD = sinLut[(((nx + nativeY) >> 1) + pdOff) & 0xFF];
        // Sum is in [-508, 508]; map to [0, 255] as palette index.
        const int sum = static_cast<int>(sA) + sB + sC + sD;
        const uint8_t idx = static_cast<uint8_t>((sum + 512) >> 2);  // /4 → 0..255
        row[nx] = cachedPalette[idx];
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderShadeBobsFrame(const ShadeBobs &sb) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  const uint8_t *canvas = sb.canvas();
  if (canvas == nullptr) {
    // Canvas allocation failed (PSRAM full?) — render solid black so we
    // don't crash and so the user can see something happened.
    for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
         stripeStart += kMaxChunkPhysicalRows) {
      const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
      std::memset(txBuffer_, 0, static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t));
      if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) return;
    }
    return;
  }

  // Pre-bake the panelColor()-applied palette so the per-pixel inner loop
  // does only LUT[index] and a store. 256 × 2 B = 512 B static.
  static uint16_t cachedPalette[ShadeBobs::kPaletteSize];
  static const uint16_t *lastPaletteSrc = nullptr;
  if (sb.palette() != lastPaletteSrc) {
    for (int i = 0; i < ShadeBobs::kPaletteSize; ++i) {
      cachedPalette[i] = panelColor(sb.palette()[i]);
    }
    lastPaletteSrc = sb.palette();
  }

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    // Sequential PSRAM read of canvas slice → palette lookup → DRAM tx
    // buffer write. Both axes hit memory linearly so the PSRAM cache stays
    // hot.
    const uint8_t *src = canvas + stripeStart * kPanelNativeWidth;
    uint16_t *dst = txBuffer_;
    const int total = rows * kPanelNativeWidth;
    for (int i = 0; i < total; ++i) {
      dst[i] = cachedPalette[src[i]];
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderVectorballFrame(Vectorball &vb) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // The Vectorball class owns a 300×280 palette-indexed framebuffer; it
  // already cleared and stamped its sprites in vb.paintFramebuffer(). Our
  // job here is to letterbox-scale that frame onto the 640×172 logical
  // panel and look up RGB565 from the 136-entry palette.
  //
  // Letterbox math (preserves the 300×280 aspect, fills the 172-tall
  // dimension): scale = 172/280 = 0.6143; visible width = 300 * 0.6143
  // = 184 px; left margin = (640 - 184) / 2 = 228; right margin = same.
  // Source mapping inverse: srcX = (logicalX - 228) * 300 / 184,
  //                         srcY = logicalY * 280 / 172.
  const uint8_t *frame = vb.framebuffer();
  if (frame == nullptr) {
    // Framebuffer alloc failed (PSRAM full) — paint solid black.
    for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
         stripeStart += kMaxChunkPhysicalRows) {
      const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
      std::memset(txBuffer_, 0, static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t));
      if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) return;
    }
    return;
  }

  // Pre-bake the palette: panelColor()-swap each of the 136 entries once
  // per frame. Cheap (272 B work) and removes a swap from the hot loop.
  uint16_t pal[136];
  for (int i = 0; i < 136; ++i) {
    const uint8_t r = kVectorballPalette[i][0];
    const uint8_t g = kVectorballPalette[i][1];
    const uint8_t b = kVectorballPalette[i][2];
    const uint16_t rgb565 = static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    pal[i] = panelColor(rgb565);
  }
  // The original palette uses slot 0 as a pure-white transparency marker
  // for its sprite-clear pass; sprites only ever write indices 1..63 into
  // the framebuffer. Our renderer clears the framebuffer to 0, so without
  // this override the empty background would render as white. Map slot 0
  // to black instead.
  pal[0] = panelColor(0x0000);
  const uint16_t kBlack = panelColor(0x0000);

  // Source-coordinate lookup tables — eliminates per-pixel multiplies.
  // srcXForLogicalX[lx-228] = (lx-228) * 300 / 184 for lx in [228, 412).
  // srcYForLogicalY[ly]     = ly * 280 / 172 for ly in [0, 172).
  // Built once per render call (frame); ~360 B of stack.
  constexpr int kVisibleW = 184;
  constexpr int kLeftMargin = 228;  // (640 - 184) / 2
  constexpr int kRightEdge = kLeftMargin + kVisibleW;  // 412
  uint16_t srcXForCol[kVisibleW];
  for (int c = 0; c < kVisibleW; ++c) {
    int sx = (c * Vectorball::kFrameWidth) / kVisibleW;
    if (sx < 0) sx = 0;
    if (sx >= Vectorball::kFrameWidth) sx = Vectorball::kFrameWidth - 1;
    srcXForCol[c] = static_cast<uint16_t>(sx);
  }
  uint16_t srcYForLogicalY[kDisplayHeight];
  for (int y = 0; y < kDisplayHeight; ++y) {
    int sy = (y * Vectorball::kFrameHeight) / kDisplayHeight;
    if (sy < 0) sy = 0;
    if (sy >= Vectorball::kFrameHeight) sy = Vectorball::kFrameHeight - 1;
    srcYForLogicalY[y] = static_cast<uint16_t>(sy);
  }

  // Stripe loop — process panel in horizontal slabs (in native orientation
  // that's nativeY = stripeStart..stripeStart+rows). For each native column
  // (= one logical Y row, post-mirror), reads from the framebuffer are
  // along a single row → sequential PSRAM access, cache-friendly.
  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
      // Logical Y for this native column (mirror): ly = 171 - nativeX.
      const int ly = (kDisplayHeight - 1) - nativeX;
      const uint8_t *srcRow = frame + srcYForLogicalY[ly] * Vectorball::kFrameWidth;
      for (int localY = 0; localY < rows; ++localY) {
        const int lx = stripeStart + localY;  // logical X for this stripe row
        uint16_t color;
        if (lx < kLeftMargin || lx >= kRightEdge) {
          color = kBlack;
        } else {
          const uint8_t palIdx = srcRow[srcXForCol[lx - kLeftMargin]];
          color = pal[palIdx];
        }
        txBuffer_[localY * kPanelNativeWidth + nativeX] = color;
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderUnlimitedBobsFrame(const UnlimitedBobs &ub) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  // Letterbox the 320×200 framebuffer onto the 640×172 panel preserving
  // aspect: scale = 172/200 = 0.86 → visible 275×172, centered horizontally
  // with ~183 px black bars on each side.
  const uint8_t *frame = ub.framebuffer();
  if (frame == nullptr) {
    for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
         stripeStart += kMaxChunkPhysicalRows) {
      const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);
      std::memset(txBuffer_, 0, static_cast<size_t>(rows) * kPanelNativeWidth * sizeof(uint16_t));
      if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) return;
    }
    return;
  }

  // Per-pixel shade is just (50, 50, shadeIndex) — bake the full 256-entry
  // palette to panel byte order once per frame. 512 B of stack.
  uint16_t pal[256];
  for (int i = 0; i < 256; ++i) {
    const uint16_t rgb565 =
        static_cast<uint16_t>(((50 & 0xF8) << 8) | ((50 & 0xFC) << 3) | ((i & 0xFF) >> 3));
    pal[i] = panelColor(rgb565);
  }
  const uint16_t kBlack = panelColor(0x0000);

  // Letterbox geometry + source-coordinate lookup tables.
  constexpr int kVisibleW = 275;                                 // 320 * 172 / 200
  constexpr int kLeftMargin = (kDisplayWidth - kVisibleW) / 2;   // 182
  constexpr int kRightEdge = kLeftMargin + kVisibleW;            // 457
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

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
      const int ly = (kDisplayHeight - 1) - nativeX;
      const uint8_t *srcRow = frame + srcYForLogicalY[ly] * UnlimitedBobs::kFrameWidth;
      for (int localY = 0; localY < rows; ++localY) {
        const int lx = stripeStart + localY;
        uint16_t color;
        if (lx < kLeftMargin || lx >= kRightEdge) {
          color = kBlack;
        } else {
          const uint8_t shade = srcRow[srcXForCol[lx - kLeftMargin]];
          color = pal[shade];
        }
        txBuffer_[localY * kPanelNativeWidth + nativeX] = color;
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}

void DisplayManager::renderCameraRgb565Frame(const uint16_t *frame, int sourceWidth, int sourceHeight) {
  if (!initialized_) return;
  lastRenderKey_ = "";

  if (frame == nullptr || sourceWidth <= 0 || sourceHeight <= 0) {
    fillScreen(kTrueBlack);
    return;
  }

  // Native-stripe camera blit. This mirrors the screensaver/demo architecture:
  // compose directly into the panel's 172×640 orientation, then DMA each large
  // stripe. For a 320×240 webcam frame, aspect-fit gives ~229×172 centered.
  const int fitWByHeight = (sourceWidth * kDisplayHeight) / sourceHeight;
  const int fitHByWidth = (sourceHeight * kDisplayWidth) / sourceWidth;
  int visibleW = kDisplayWidth;
  int visibleH = fitHByWidth;
  if (fitWByHeight <= kDisplayWidth) {
    visibleW = std::max(1, fitWByHeight);
    visibleH = kDisplayHeight;
  }
  const int leftMargin = (kDisplayWidth - visibleW) / 2;
  const int topMargin = (kDisplayHeight - visibleH) / 2;
  const int rightEdge = leftMargin + visibleW;
  const int bottomEdge = topMargin + visibleH;
  const uint16_t black = panelColor(0x0000);

  uint16_t srcXForLogicalX[kDisplayWidth];
  for (int lx = 0; lx < kDisplayWidth; ++lx) {
    int sx = 0;
    if (lx >= leftMargin && lx < rightEdge) {
      sx = ((lx - leftMargin) * sourceWidth) / visibleW;
      if (sx >= sourceWidth) sx = sourceWidth - 1;
      sx = (sourceWidth - 1) - sx;
    }
    srcXForLogicalX[lx] = static_cast<uint16_t>(sx);
  }

  uint16_t srcYForLogicalY[kDisplayHeight];
  for (int ly = 0; ly < kDisplayHeight; ++ly) {
    int sy = 0;
    if (ly >= topMargin && ly < bottomEdge) {
      sy = ((ly - topMargin) * sourceHeight) / visibleH;
      if (sy >= sourceHeight) sy = sourceHeight - 1;
      sy = (sourceHeight - 1) - sy;
    }
    srcYForLogicalY[ly] = static_cast<uint16_t>(sy);
  }

  for (int stripeStart = 0; stripeStart < kPanelNativeHeight;
       stripeStart += kMaxChunkPhysicalRows) {
    const int rows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - stripeStart);

    for (int nativeX = 0; nativeX < kPanelNativeWidth; ++nativeX) {
      const int ly = (kDisplayHeight - 1) - nativeX;
      const bool yInside = (ly >= topMargin && ly < bottomEdge);
      const uint16_t *srcRow = yInside
                                   ? frame + srcYForLogicalY[ly] * sourceWidth
                                   : nullptr;
      for (int localY = 0; localY < rows; ++localY) {
        const int lx = stripeStart + localY;
        uint16_t color = black;
        if (srcRow != nullptr && lx >= leftMargin && lx < rightEdge) {
          color = panelColor(srcRow[srcXForLogicalX[lx]]);
        }
        txBuffer_[localY * kPanelNativeWidth + nativeX] = color;
      }
    }

    if (!drawBitmap(0, stripeStart, kPanelNativeWidth, stripeStart + rows, txBuffer_)) {
      return;
    }
  }
}
