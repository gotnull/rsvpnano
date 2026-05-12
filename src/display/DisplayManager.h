#pragma once

#include <Arduino.h>
#include <vector>

#include "audio/ModulePatternCell.h"

class Screensaver;
class Rasterbars;
class Starfield;
class SineScroller;
class Plasma;
class ShadeBobs;
class Vectorball;
class UnlimitedBobs;
class Pupul;

class DisplayManager {
 public:
  struct TypographyConfig {
    int8_t trackingPx = 0;
    uint8_t anchorPercent = 35;
    uint8_t guideHalfWidth = 20;
    uint8_t guideGap = 4;
  };

  struct ContextWord {
    String text;
    bool paragraphStart = false;
    bool current = false;
  };

  struct LibraryItem {
    String title;
    String subtitle;
    int32_t wordCount = -1;
    int32_t chapterCount = -1;
    int8_t progressPercent = -1;
    std::vector<String> badges;
  };

  ~DisplayManager();

  bool begin();
  void setBatteryLabel(const String &label);
  void setCurrentWpm(uint16_t wpm);
  void setWpmHighlightUntil(uint32_t deadlineMs);
  void setBrightnessPercent(uint8_t percent);
  void setDarkMode(bool darkMode);
  void setNightMode(bool nightMode);
  void setTypographyConfig(const TypographyConfig &config);
  TypographyConfig typographyConfig() const;
  bool darkMode() const;
  bool nightMode() const;
  void setUiRotated(bool rotated);
  bool uiRotated() const;
  void setChapterFractions(const std::vector<float> &fractions);
  void prepareForSleep();
  bool wakeFromSleep();
  void renderCenteredWord(const String &word, uint16_t color = 0xFFFF);
  void renderRsvpWord(const String &word, const String &chapterLabel = "",
                      uint8_t progressPercent = 0, bool showFooter = true);
  void renderRsvpWordWithWpm(const String &word, uint16_t wpm, const String &chapterLabel = "",
                             uint8_t progressPercent = 0, bool showFooter = true);
  void renderPhantomRsvpWord(const String &beforeText, const String &word, const String &afterText,
                             uint8_t fontSizeLevel, const String &chapterLabel = "",
                             uint8_t progressPercent = 0, bool showFooter = true);
  void renderPhantomRsvpWordWithWpm(const String &beforeText, const String &word,
                                    const String &afterText, uint8_t fontSizeLevel, uint16_t wpm,
                                    const String &chapterLabel = "",
                                    uint8_t progressPercent = 0, bool showFooter = true);
  void renderTypographyPreview(const String &beforeText, const String &word, const String &afterText,
                               uint8_t fontSizeLevel, const String &title,
                               const String &line1 = "", const String &line2 = "");
  void renderContextView(const std::vector<ContextWord> &words, const String &chapterLabel = "",
                         uint8_t progressPercent = 0);
  void renderMenu(const char *const *items, size_t itemCount, size_t selectedIndex);
  void renderMenu(const std::vector<String> &items, size_t selectedIndex);
  void renderMenu(const std::vector<String> &items, size_t selectedIndex,
                  const std::vector<bool> &chevronRows);
  // Tabbed-picker renderer. Reserves a top tab band, centres each tab label
  // in its slot, and draws a Material-style underline at (underlineXPx,
  // underlineWPx) so callers can animate the underline by sliding those
  // values between calls. `chevronRows` follows the existing per-row
  // convention (must be present for every drill-in row).
  void renderMenuWithTabs(const std::vector<String> &items,
                          size_t selectedIndex,
                          const std::vector<bool> &chevronRows,
                          const std::vector<String> &tabLabels,
                          int activeTabIdx,
                          int underlineXPx,
                          int underlineWPx);
  // Partial 60 fps overlay used during a tab-slide animation. Repaints ONLY
  // the 2-row underline strip (a thin native-X slice) via drawBitmap, so the
  // rest of the menu rendered by renderMenuWithTabs is preserved on the
  // panel without paying for a full virtualFrame_ flush per frame.
  void renderTabUnderlineStrip(int underlineXPx, int underlineWPx);
  // Geometry helpers — return the X coordinate and width an active tab's
  // underline should occupy. Computed from the same divider-margin constants
  // used by renderMenuWithTabs / renderTabUnderlineStrip so the underline
  // perfectly overlaps its slice of the gray hairline.
  static int tabUnderlineXForTab(int tabIdx, int tabCount);
  static int tabUnderlineWidth(int tabCount);
  void renderMenuWithAccent(const char *const *items, size_t itemCount, size_t selectedIndex,
                            size_t accentRow, const String &accentText,
                            const std::vector<String> &accentChips = std::vector<String>(),
                            const std::vector<bool> &chevronRows = std::vector<bool>());
  void renderLibrary(const std::vector<LibraryItem> &items, size_t selectedIndex,
                     const std::vector<char> &letterAnchors = std::vector<char>(),
                     int focusedLetterIdx = -1,
                     const std::vector<char> &scrubLetters = std::vector<char>(),
                     const std::vector<String> &tabLabels = std::vector<String>(),
                     int activeTabIdx = -1,
                     int underlineXPx = -1,
                     int underlineWPx = -1);

  static constexpr int kLibraryLetterStripWidth = 22;
  static constexpr int kLibraryLetterScrubWidth = 140;
  static constexpr int kLibraryScrubVisibleLetters = 9;
  static int libraryLetterAtY(const std::vector<char> &letterAnchors, int y);
  static int libraryScrubLetterAtY(const std::vector<char> &letterAnchors, int y, int focusIdx);
  void renderStatus(const String &title, const String &line1 = "", const String &line2 = "");
  void renderProgress(const String &title, const String &line1 = "", const String &line2 = "",
                      int progressPercent = -1);
  // Renders a single frame of the dots/stars screensaver. Call once per frame
  // after Screensaver::tick() + sortPoints().
  void renderScreensaverFrame(Screensaver &saver);

  // Demoscene-style demos. All use the native-stripe path (no virtualFrame_,
  // no transpose) for sustained frame rates. Each takes a state object whose
  // tick() must already have been called for the current frame.
  void renderRasterbarsFrame(const Rasterbars &rb);
  void renderStarfieldFrame(const Starfield &sf);
  void renderSineScrollerFrame(const SineScroller &ss);
  void renderPlasmaFrame(const Plasma &pl);
  void renderShadeBobsFrame(const ShadeBobs &sb);
  void renderVectorballFrame(Vectorball &vb);
  void renderUnlimitedBobsFrame(const UnlimitedBobs &ub);
  // Pupul (Equinox 1989 Atari ST intro port). Reads the demo's internal
  // 320×228 RGB565 canvas via Pupul::framebuffer() and maps it to the
  // 640×172 panel: 2× horizontal scale (logicalX/2 → srcX) and 0.75×
  // vertical squash (logicalY * 228 / 172 → srcY).
  void renderPupulFrame(const Pupul &p);
  // Renders an RGB565 source frame using the same native-stripe path as the
  // screensaver/demos. Source pixels are normal RGB565 in logical orientation.
  void renderCameraRgb565Frame(const uint16_t *frame, int sourceWidth, int sourceHeight);
  // Fullscreen tracker player. Native-stripe renderer (no virtualFrame_, no
  // transpose) so the level meters animate at panel-rate. Shows file name
  // (marquee if it overflows), module title + format, BPM/speed, position/row
  // counters, and a row of channel volume bars with peak-hold markers
  // (levels 0..64 per libxmp's xmp_channel_info.volume).
  void renderModulePlayerFrame(const char *fileName,
                               const char *title,
                               const char *format,
                               int bpm, int speed,
                               int pos, int row, int numRows,
                               const uint8_t *barLevels,
                               const uint8_t *peakLevels,
                               int barCount);

  // Alternate now-playing view — MilkyTracker-style pattern grid. Cells
  // come in row-major layout sized [visibleRowCount × chanStride]; the
  // caller passes only the rows it wants visible (centred around current
  // row by sliding `firstRow`). Channel cells beyond `populatedChannels`
  // are rendered as empty.
  void renderModulePlayerPatternFrame(const char *fileName,
                                      const char *title,
                                      const char *format,
                                      int pattern, int patternCount,
                                      int pos, int orderCount,
                                      int currentRow, int patternRows,
                                      int bpm, int speed,
                                      const ModulePatternCell *cells,
                                      int visibleRowCount, int chanStride,
                                      int firstRow, int totalChannels,
                                      int populatedChannels);

 private:
  bool initPanel();
  bool allocateBuffers();
  bool drawBitmap(int xStart, int yStart, int xEnd, int yEnd, const void *colorData);
  void fillScreen(uint16_t color);
  void clearVirtualBuffer(int width, int height);
  uint16_t backgroundColor() const;
  uint16_t wordColor() const;
  uint16_t focusColor() const;
  uint16_t dimColor() const;
  uint16_t footerColor() const;
  uint16_t selectedBarColor() const;
  uint16_t blendOverBackground(uint16_t rgb565, uint8_t alpha) const;
  int chooseTextScale(const String &word) const;
  int measureTextWidth(const String &word) const;
  int measureSerifTextWidth(const String &text, int divisor) const;
  int measureSerif70TextWidth(const String &text) const;
  int measureSerifTextWidthScaled(const String &text, uint8_t scalePercent) const;
  int measureTinyTextWidth(const String &text, int scale) const;
  String fitSerifText(const String &text, int maxWidth, int divisor) const;
  String fitTinyText(const String &text, int maxWidth, int scale) const;
  void drawGlyph(int x, int y, char c, uint16_t color);
  void drawSerifGlyphScaled(int x, int y, char c, uint16_t color, int divisor);
  void drawSerif70Glyph(int x, int y, char c, uint16_t color);
  void drawSerifGlyphScaledPercent(int x, int y, char c, uint16_t color, uint8_t scalePercent);
  void fillVirtualRect(int x, int y, int width, int height, uint16_t color);
  void fillRoundedRect(int x, int y, int width, int height, int radius, uint16_t color);
  int drawChipText(const String &text, int leftX, int textY, uint16_t textColor, uint16_t bgColor,
                   bool rightAlign = false, int rightX = 0);
  int drawScrollingChipText(const String &text, int leftX, int textY, int maxChipWidth,
                            uint16_t textColor, uint16_t bgColor);
  void drawSerifTextAt(const String &text, int x, int y, uint16_t color, int divisor);
  void drawSerif70TextAt(const String &text, int x, int y, uint16_t color);
  void drawSerifTextScaledAt(const String &text, int x, int y, uint16_t color,
                             uint8_t scalePercent);
  void drawTinyGlyph(int x, int y, char c, uint16_t color, int scale);
  void drawTinyGlyphClipped(int x, int y, char c, uint16_t color, int scale, int clipLeftX,
                            int clipRightX);
  // Like drawTinyGlyphClipped, but blends each pixel toward fadeColor along
  // the leftmost/rightmost `fadeWidth` columns of the clip window. Useful to
  // give marquee text a soft fade at chip edges instead of hard clipping.
  void drawTinyGlyphFaded(int x, int y, char c, uint16_t color, int scale, int clipLeftX,
                          int clipRightX, int fadeWidth, uint16_t fadeColor);
  void drawTinyTextAt(const String &text, int x, int y, uint16_t color, int scale);
  // Renders `text` at tiny scale inside [leftX, rightX). When the text is wider
  // than that band, slides it ping-pong with a soft edge fade toward fadeColor.
  // Single source of truth — every overflowing tiny-text label should call this
  // instead of re-implementing the offset+per-glyph loop.
  void drawTinyMarquee(const String &text, int leftX, int rightX, int textY,
                       uint16_t color, uint16_t fadeColor);
  // Native-stripe text helpers. Same logical (x, y) input as drawTinyTextAt,
  // but rasterise directly into txBuffer_ (panel-native orientation) for the
  // current stripe. panelEncoded must already be panelColor()-applied. Clips
  // logical X to [clipLeftLogicalX, clipRightLogicalX) so marquee-style
  // negative offsets don't bleed out of the intended band.
  void drawTinyGlyphNativeStripe(int logicalX, int logicalY, char c, int scale,
                                 uint16_t panelEncoded, int stripeStart,
                                 int stripeRows, int clipLeftLogicalX,
                                 int clipRightLogicalX);
  void drawTinyTextNativeStripe(const String &text, int logicalX, int logicalY,
                                int scale, uint16_t panelEncoded,
                                int stripeStart, int stripeRows,
                                int clipLeftLogicalX, int clipRightLogicalX);
  void drawTinyTextCentered(const String &text, int y, uint16_t color, int scale);
  void drawBatteryBadge(bool leftAlign = false);
  // Shared top tab band — drawn at the top of any tabbed picker
  // (renderMenuWithTabs, renderLibrary, ...). Honours the shared
  // tabUnderlineXForTab/Width geometry so labels and underlines always
  // share the same slice and never overlap the right-side battery chip.
  void drawTabBand(const std::vector<String> &tabLabels, int activeTabIdx,
                   int underlineXPx, int underlineWPx);
  void drawFooter(const String &chapterLabel, uint8_t progressPercent);
  void drawRsvpAnchorGuide(int anchorX, int textY, int textHeight);
  void drawWordAt(const String &word, int x, int y, uint16_t color);
  void drawRsvpWordAt(const String &word, int x, int y, int focusIndex);
  void drawRsvp70WordAt(const String &word, int x, int y, int focusIndex);
  void drawRsvpWordScaledAt(const String &word, int x, int y, int focusIndex, int divisor);
  void drawRsvpWordScaledPercentAt(const String &word, int x, int y, int focusIndex,
                                   uint8_t scalePercent);
  void drawWordLine(const String &word, int y, uint16_t color);
  void drawMenuItem(const String &item, int y, bool selected);
  void applyBrightness();
  void flushScaledFrame(int scale, int virtualWidth, int virtualHeight);

  uint16_t *virtualFrame_ = nullptr;
  uint16_t *txBuffer_ = nullptr;
  // Second DMA-capable stripe buffer. Used by flushScaledFrame to compose the
  // next stripe while the previous one is being DMA'd to the panel. Same
  // size as txBuffer_; both live in internal RAM so the SPI peripheral can
  // DMA from them directly.
  uint16_t *txBufferAlt_ = nullptr;
  size_t txBufferBytes_ = 0;
  bool initialized_ = false;
  uint8_t brightnessPercent_ = 100;
  bool darkMode_ = true;
  bool nightMode_ = false;
  bool uiRotated_ = true;
  String lastRenderKey_;
  String batteryLabel_;
  uint16_t currentWpm_ = 0;
  uint32_t wpmHighlightUntilMs_ = 0;
  std::vector<float> chapterFractions_;
};
