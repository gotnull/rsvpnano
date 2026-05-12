#pragma once

#include <stdint.h>

// OldschoolIntro — port of Equinox Scrolls88 applet (eqx=1 / oldschoolintro,
// Keops 2000). 320×200 internal canvas containing:
//   - a 22×8-strip x-distorted big scroller (text bt)
//   - a 21-glyph horizontally waving small scroller (text bu)
//   - a 12-glyph vertically stacked (mirrored) small scroller (text bs)
//   - a 60-dot Lissajous ring (4-phase)
//   - a 1-bit banner overlay decoded from the embedded c[413] mask
//
// See tools/demoref/equinox/oldschoolintro/PORT_NOTES.md for the full spec.
class OldschoolIntro {
 public:
  static constexpr int kCanvasW = 320;
  static constexpr int kCanvasH = 200;

  OldschoolIntro() = default;

  bool begin(uint32_t nowMs);
  void end();
  void tick(uint32_t nowMs);

  // Indexed framebuffer (palette index 0..255), 320*200 bytes.
  const uint8_t *framebuffer() const { return ay_; }
  // 256-entry RGB565 palette LUT.
  const uint16_t *palette565() const { return palette565_; }

  // For the profiling line.
  uint32_t frameCounter() const { return frameCounter_; }

 private:
  // Lifecycle helpers.
  void buildPalette();
  void buildSineLut();          // s[24000]
  void buildAdLut();            // ad[1002]
  void buildAfAgLuts();         // af[200], ag[200]
  void unpackBannerMask();      // c[413] -> d[3300], stretch right
  void bakeBigScrollerStrip();  // bt -> q[134400]

  // Per-frame helpers (mirror Scrolls88.v()).
  void drawBigScrollerStrip();   // 22×8 + separator rows
  void drawDotRing();            // 60 dots Lissajous
  void drawVerticalScroller();   // i() — bs glyphs, left+right mirrored
  void drawHorizontalScroller(); // k() — bu glyphs across screen
  void drawBannerOverlay();      // q(ay, d)

  // Glyph blitters.
  void blitVerticalGlyph(int slot, int xShift, int glyphOffset);
  void blitHorizontalGlyph(int slot, int xShift, int glyphOffset);
  void blitRingDot(int center);

  // Substitute non-alpha chars to glyph indices (`-65` + remap).
  static int substituteChar(char c, bool bigFont);

  // Buffers (PSRAM).
  uint8_t *ay_ = nullptr;   // 64000 — output framebuffer (indexed)
  uint8_t *q_  = nullptr;   // 134400 — big-scroller pre-rendered strip
  int16_t *s_  = nullptr;   // 24000 — sine-modulated horizontal offsets
  uint8_t *d_  = nullptr;   // 3300 — unpacked banner mask

  // Palette LUT (RGB565).
  uint16_t palette565_[256] = {0};

  // Bobbing/colour LUTs (small enough to live in-class).
  int16_t ad_[1002] = {0};
  uint8_t af_[200]  = {0};
  uint8_t ag_[200]  = {0};

  // Scroller / ring state.
  uint32_t t_       = 0;     // index into s_[]
  float ab_         = 0.0f;
  float aa_         = 0.0f;
  float zPhase_     = 0.0f;
  float vPhase_     = 0.0f;
  int   smallE_     = 0;     // horizontal scroller (bu) intra-glyph shift
  int   smallF_     = 0;     // horizontal scroller text cursor
  int   smallAh_    = 0;     // vertical scroller (bs) intra-glyph shift
  int   smallAi_    = 0;     // vertical scroller text cursor
  int   adCursor_   = 0;     // shared ad[] cursor for both small scrollers

  uint32_t frameCounter_ = 0;
  bool initialised_      = false;
};
