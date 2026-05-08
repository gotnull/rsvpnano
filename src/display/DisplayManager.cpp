#include "display/DisplayManager.h"

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
constexpr int kTxBufferWidth = kDisplayWidth > kPanelNativeWidth ? kDisplayWidth : kPanelNativeWidth;
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
  for (int nativeYStart = 0; nativeYStart < kPanelNativeHeight;
       nativeYStart += kMaxChunkPhysicalRows) {
    const int nativeRows = std::min(kMaxChunkPhysicalRows, kPanelNativeHeight - nativeYStart);
    std::memset(txBuffer_, 0, txBufferBytes_);

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

    if (!drawBitmap(0, nativeYStart, kPanelNativeWidth, nativeYStart + nativeRows, txBuffer_)) {
      return;
    }
  }
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
  const int totalHeight = rowHeight * static_cast<int>(visibleCount);
  int y = std::max(0, (virtualHeight - totalHeight) / 2);

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
  const int totalHeight = rowHeight * static_cast<int>(visibleCount);
  int y = std::max(0, (virtualHeight - totalHeight) / 2);

  clearVirtualBuffer(virtualWidth, virtualHeight);

  const int chevronWidth = measureTinyTextWidth(">", kTinyScale);
  const int spaceWidth = (kTinyGlyphWidth + kTinyGlyphSpacing) * kTinyScale;

  // Chevron column sits two spaces past the widest item in this menu so every
  // chevron lines up regardless of which row it's on.
  int maxItemTextWidth = 0;
  for (size_t i = 0; i < itemCount; ++i) {
    const int w = measureTinyTextWidth(items[i], kTinyScale);
    if (w > maxItemTextWidth) maxItemTextWidth = w;
  }
  const int chevronColumnX = kCompactMenuX + maxItemTextWidth + spaceWidth * 2;

  for (size_t row = 0; row < visibleCount; ++row) {
    const size_t itemIndex = firstVisible + row;
    const bool selected = itemIndex == selectedIndex;
    const uint16_t color = selected ? focusColor() : dimColor();
    const bool hasChevron = itemIndex < chevronRows.size() && chevronRows[itemIndex];
    const int rightReserve = chevronColumnX - kCompactMenuX + chevronWidth + kFooterMarginX;
    const int maxWidth = virtualWidth - kCompactMenuX - rightReserve;
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
