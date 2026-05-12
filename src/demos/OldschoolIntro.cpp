#include "demos/OldschoolIntro.h"

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <algorithm>
#include <cmath>
#include <cstring>

#include "demos/oldschoolintro_assets.h"

namespace {

constexpr int kStrips      = 22;   // big scroller — vertical strip count
constexpr int kStripRows   = 8;    // per-strip row count
constexpr int kStripStride = 320;  // canvas width
constexpr int kQRowStride  = 8400; // q[] inter-row stride (Java)
constexpr int kQSize       = 134400;
constexpr int kSSize       = 24000;
constexpr int kAySize      = OldschoolIntro::kCanvasW * OldschoolIntro::kCanvasH;
constexpr int kDSize       = 3300;
constexpr int kBigScrollerSrcRow = 80;  // h() writes starting at (f+80)*7 cols
constexpr int kBigScrollerCharLimit = 850;  // h() iterations
constexpr int kFontGlyphCount = 32;     // big-font glyphs in o[1792]
constexpr int kSmallGlyphCount = 31;    // small-font glyphs in ac[7905]
constexpr int kSmallGlyphStride = 255;  // bytes per glyph in ac[]
constexpr int kSmallGlyphW = 15;
constexpr int kSmallGlyphH = 17;

// Hardcoded ring-dot sprite (Scrolls88.a(byte[], int)). Pairs of
// (offset, palette-index) stamped into ay at base+offset.
struct DotPixel { int16_t offset; uint8_t color; };
constexpr DotPixel kRingDot[] = {
    {   5, 121},
    { 323,  71}, { 324,  71}, { 325,  89}, { 326,  90}, { 327, 111},
    { 642,  65}, { 643,  65}, { 644,  89}, { 645,  96}, { 646,  96}, { 647,  92},
    { 961,  67}, { 962,  65}, { 963,  67}, { 964,  92}, { 965,  94}, { 966,  90}, { 967,  94}, { 968, 121},
    {1280,  86}, {1281,  65}, {1282,  67}, {1283,  77}, {1284,  96}, {1285,  87}, {1286,  89}, {1287,  83}, {1288,  96},
    {1600, 102}, {1601,  75}, {1602,  81}, {1603,  90}, {1604,  84}, {1605,  94}, {1606,  90}, {1607,  84}, {1608, 121},
    {1920, 121}, {1921,  87}, {1922,  91}, {1923,  96}, {1924,  84}, {1925,  89}, {1926,  84}, {1927,  84},
    {2241, 109}, {2242,  94}, {2243,  94}, {2244,  89}, {2245,  75}, {2246,  77}, {2247, 109},
    {2563, 102}, {2564,  94}, {2565, 102},
};
constexpr int kRingDotCount = sizeof(kRingDot) / sizeof(kRingDot[0]);

inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

inline uint8_t clampByte(int v) {
  return v < 0 ? 0 : (v > 255 ? 255 : static_cast<uint8_t>(v));
}

void *psramAlloc(size_t bytes) {
  void *p = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!p) p = heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
  return p;
}

}  // namespace

int OldschoolIntro::substituteChar(char c, bool bigFont) {
  // Java applies these substitutions before the `-65` mapping. The big
  // scroller uses one set of substitutions; the small scrollers another.
  if (bigFont) {
    if (c == ' ') return 91;
    if (c == '.') return 92;
    if (c == '!') return 93;
    if (c == ':') return 94;
    if (c == ',') return 95;
    if (c == '-') return 96;
  } else {
    if (c == ' ')  return 91;
    if (c == '.')  return 92;
    if (c == '!')  return 93;
    if (c == '\'') return 94;
    if (c == ',')  return 95;
  }
  return static_cast<int>(static_cast<uint8_t>(c));
}

bool OldschoolIntro::begin(uint32_t nowMs) {
  (void)nowMs;
  if (initialised_) return true;

  if (!ay_) ay_ = static_cast<uint8_t *>(psramAlloc(kAySize));
  if (!ay_) {
    Serial.println("[oldschool] FATAL: ay_ alloc failed");
    return false;
  }
  if (!q_)  q_  = static_cast<uint8_t *>(psramAlloc(kQSize));
  if (!q_) {
    Serial.println("[oldschool] FATAL: q_ alloc failed");
    return false;
  }
  if (!s_)  s_  = static_cast<int16_t *>(psramAlloc(kSSize * sizeof(int16_t)));
  if (!s_) {
    Serial.println("[oldschool] FATAL: s_ alloc failed");
    return false;
  }
  if (!d_)  d_  = static_cast<uint8_t *>(psramAlloc(kDSize));
  if (!d_) {
    Serial.println("[oldschool] FATAL: d_ alloc failed");
    return false;
  }

  std::memset(ay_, 0, kAySize);
  std::memset(q_, 0, kQSize);

  buildPalette();
  buildSineLut();
  buildAdLut();
  buildAfAgLuts();
  unpackBannerMask();
  bakeBigScrollerStrip();

  t_ = 0;
  ab_ = aa_ = zPhase_ = vPhase_ = 0.0f;
  smallE_ = 0;
  smallF_ = 0;
  smallAh_ = 0;
  smallAi_ = 0;
  adCursor_ = 0;
  frameCounter_ = 0;
  initialised_ = true;
  return true;
}

void OldschoolIntro::end() {
  if (ay_) { heap_caps_free(ay_); ay_ = nullptr; }
  if (q_)  { heap_caps_free(q_);  q_  = nullptr; }
  if (s_)  { heap_caps_free(s_);  s_  = nullptr; }
  if (d_)  { heap_caps_free(d_);  d_  = nullptr; }
  initialised_ = false;
}

void OldschoolIntro::buildPalette() {
  // Mirrors Scrolls88.init() palette construction for a7==1.
  int n[256], m[256], l[256];

  // Initial 4×8 RGB block (aj=0..31).
  for (int aj = 0; aj < 8; ++aj) {
    n[aj] = 255; m[aj] = 255; l[aj] = aj * 32;
  }
  for (int aj = 8; aj < 16; ++aj) {
    n[aj] = (15 - aj) * 32; m[aj] = 255; l[aj] = 255;
  }
  for (int aj = 16; aj < 24; ++aj) {
    n[aj] = (aj - 16) * 32; m[aj] = (23 - aj) * 32; l[aj] = 255;
  }
  for (int aj = 24; aj < 32; ++aj) {
    n[aj] = 255; m[aj] = (aj - 24) * 32; l[aj] = (31 - aj) * 32;
  }

  // Fade copies (32-block × 6) — each successive block fades by 20.
  constexpr int fade = 20;
  for (int v = 1; v < 7; ++v) {
    for (int aj = 0; aj < 32; ++aj) {
      const int prev = aj + 32 * (v - 1);
      const int cur  = aj + 32 * v;
      n[cur] = n[prev] > fade ? n[prev] - fade : 0;
      m[cur] = m[prev] > fade ? m[prev] - fade : 0;
      l[cur] = l[prev] > fade ? l[prev] - fade : 0;
    }
  }

  // Boost passes (lines 204-225).
  for (int aj = 0; aj < 256; ++aj) {
    n[aj] = n[aj] < 235 ? n[aj] + 20 : 255;
    m[aj] = m[aj] < 250 ? m[aj] +  5 : 255;
    l[aj] = l[aj] >  20 ? l[aj] - 20 : 0;
  }

  // Force palette 0 to black (transparent).
  n[0] = 0; m[0] = 0; l[0] = 0;

  // Big-scroller per-row colour ramp into palette 1..8.
  for (int aj = 0; aj < 8; ++aj) {
    n[aj + 1] = 48;
    m[aj + 1] = 255 - aj * 16 - 48 + 16;
    l[aj + 1] = 255 - aj * 12;
  }

  // Small-scroller ramps at 128..152.
  n[128] = 0; m[128] = 0; l[128] = 0;
  for (int aj = 129; aj < 137; ++aj) {
    n[aj]      = 0;
    m[aj]      = (aj - 129) * 31;
    l[aj]      = (aj - 129) * 31;
    n[aj + 8]  = (aj - 129) * 31;
    m[aj + 8]  = (aj - 129) * 31;
    l[aj + 8]  = 0;
    n[aj + 16] = (aj - 129) * 31;
    m[aj + 16] = 0;
    l[aj + 16] = (aj - 129) * 31;
  }

  // Ring-dot ramp at 64..123 fed from the embedded a[186] table.
  int cur = 0;
  for (int aj = 64; aj < 124 && cur + 2 < 186; ++aj) {
    n[aj] = oldschool::kPaletteRamp[cur++] / 2 + 128;
    m[aj] = oldschool::kPaletteRamp[cur++] * 6 / 7;
    l[aj] = oldschool::kPaletteRamp[cur++] * 6 / 7;
  }

  // White-hot accents — lines 272-294.
  m[64] = 255; l[64] = 255;
  for (int v = 10; v < 20; ++v) {
    n[v] = (v - 10) * 27;
    m[v] = 255;
    l[v] = (v - 10) * 27;
  }
  for (int aj = 130; aj < 140; ++aj) {
    n[aj]      = (aj - 130) * 25;
    m[aj]      = 255;
    l[aj]      = 255;
    n[aj + 10] = 255;
    m[aj + 10] = 255 - (aj - 130) * 12;
    l[aj + 10] = 255;
    n[aj + 20] = 255;
    m[aj + 20] = 128;
    l[aj + 20] = 255 - (aj - 130) * 12;
  }

  for (int i = 0; i < 256; ++i) {
    palette565_[i] = rgb565(clampByte(n[i]), clampByte(m[i]), clampByte(l[i]));
  }
}

void OldschoolIntro::buildSineLut() {
  for (int i = 0; i < kSSize; ++i) {
    const int wrap = (i / 2) % 12200;
    const int wob  = static_cast<int>(std::sin(i / 30.0) * 30.0);
    s_[i] = static_cast<int16_t>(30 + wrap + wob);
  }
}

void OldschoolIntro::buildAdLut() {
  for (int aj = 0; aj < 1000; ++aj) {
    ad_[aj] = static_cast<int16_t>(std::sin(0.012566f * aj * 5.0f) * 40.0 + 100.0 - 8.0);
  }
}

void OldschoolIntro::buildAfAgLuts() {
  std::memset(af_, 0, sizeof(af_));
  for (int aj = 0; aj < 96; ++aj) {
    af_[aj + 52] = static_cast<uint8_t>(std::sin(3.1415f * aj / 96.0f) * 5.0 + 15.0);
  }
  for (int aj = 0; aj < 200; ++aj) {
    ag_[aj] = static_cast<uint8_t>(aj / 8 + 25);
  }
}

void OldschoolIntro::unpackBannerMask() {
  // Bit-unpack c[413] → d[3300]: each bit becomes 0xFF or 0x00.
  for (int n = 0; n < kDSize; ++n) {
    const uint8_t bit = (oldschool::kBannerPacked[n / 8] >> (7 - n % 8)) & 1U;
    d_[n] = bit ? 0xFFu : 0u;
  }
  // Stretch lit pixels right by 1 (Scrolls88 init lines 333-343), then
  // remap "any lit" → palette index 64.
  for (int n = 3298; n >= 0; --n) {
    if (d_[n] != 0) {
      d_[n + 1] = 64;
      d_[n]     = 64;
    }
  }
}

void OldschoolIntro::bakeBigScrollerStrip() {
  // Mirrors Scrolls88.h(): pre-render the big scroller into q[].
  const char *bt = oldschool::kTextBig;
  const int btLen = static_cast<int>(strlen(bt));

  int f = 0;
  for (int n = 0; n < kBigScrollerCharLimit; ++n) {
    if (f >= btLen) f = 0;
    int ch = substituteChar(bt[f++], /*bigFont=*/true);
    const int glyph = ch - 65;
    if (glyph < 0 || glyph >= kFontGlyphCount) continue;
    for (int n3 = 0; n3 < 8; ++n3) {
      const int dstBase = (n + kBigScrollerSrcRow) * 7 + n3 * kQRowStride;
      const int srcBase = glyph * 56 + n3 * 7;
      for (int n4 = 0; n4 < 7; ++n4) {
        if (dstBase + n4 < kQSize) {
          q_[dstBase + n4] = oldschool::kBigFont[srcBase + n4];
        }
      }
    }
  }
}

void OldschoolIntro::drawBigScrollerStrip() {
  // ay_ row 0 is the separator row at the top (zero / dark).
  std::memset(ay_, 0, kStripStride);

  for (int strip = 0; strip < kStrips; ++strip) {
    for (int row = 0; row < kStripRows; ++row) {
      const int sIdx = (t_++) % kSSize;
      int srcOff = row * kQRowStride + s_[sIdx];
      if (srcOff < 0) srcOff = 0;
      const int dstOff = kStripStride * (row + 9 * strip + 1);
      if (srcOff + kStripStride <= kQSize) {
        std::memcpy(ay_ + dstOff, q_ + srcOff, kStripStride);
      } else {
        std::memset(ay_ + dstOff, 0, kStripStride);
      }
    }
    const int sepDst = kStripStride * (kStripRows + 9 * strip + 1);
    if (sepDst + kStripStride <= kAySize) {
      std::memset(ay_ + sepDst, 0, kStripStride);
    }
  }

  // Java's `t -= 174` after building. Net advance per frame = +2.
  t_ -= 174;
  if (t_ > 12000) t_ = 0;
}

void OldschoolIntro::blitRingDot(int center) {
  for (int i = 0; i < kRingDotCount; ++i) {
    const int off = center + kRingDot[i].offset;
    if (off >= 0 && off < kAySize) {
      ay_[off] = kRingDot[i].color;
    }
  }
}

void OldschoolIntro::drawDotRing() {
  for (int n2 = 0; n2 < 60; ++n2) {
    const float phase = n2 / 60.0f;
    const int x = static_cast<int>(
        std::cos(ab_ + phase * 12.566370964050293) * std::sin(vPhase_) * 20.0
        + std::cos((2.0f * aa_) / 2.0 - phase * 6.28) * 40.0
        - std::sin((aa_ / 2.0f + zPhase_) + phase * 6.283185005187988) * 60.0)
        + 160 - 4;
    const int y = static_cast<int>(
        std::cos((aa_ * 1.5f) + phase * 6.283185005187988) * std::sin(zPhase_) * 20.0
        - std::sin((4.0f * ab_) / 3.0 + phase * 6.28 * 2.0) * 40.0
        + std::sin((ab_ / 2.0f + vPhase_) + phase * 6.283185005187988) * 20.0)
        + 100;
    blitRingDot(x + y * 320);
  }
  ab_     += 0.02f;
  aa_     += 0.0146f;
  zPhase_ += 0.0182f;
  vPhase_ += 0.0206f;
}

void OldschoolIntro::blitVerticalGlyph(int slot, int xShift, int glyphOffset) {
  // Mirrors Scrolls88.e(): stamp a 15x16 glyph at column 16 of canvas,
  // mirrored to column 16+272=288, with vertical sub-pixel shift `ah`.
  // slot range is 0..11; intra-glyph shift xShift is in [-15, 0].
  const uint8_t *mask = oldschool::kSmallFont;
  if (slot == 0 && xShift > -15) {
    int agIdx = 6;
    int dst = 1936;
    int srcIdx = glyphOffset - xShift * 15 + 15;
    for (int n7 = -xShift; n7 < 16; ++n7) {
      const uint8_t color = ag_[agIdx++];
      for (int col = 0; col < 15; ++col) {
        if (srcIdx >= 0 && srcIdx < 7905 && mask[srcIdx] != 0) {
          if (dst < kAySize)         ay_[dst] = color;
          if (dst + 272 < kAySize)   ay_[dst + 272] = color;
        }
        ++srcIdx;
        ++dst;
      }
      dst += 305;
    }
    return;
  }
  if (slot != 0 && slot < 11) {
    int agIdx = 6 + slot * 17 + xShift;
    int dst = 1936 + (slot * 17 + xShift) * 320;
    int srcIdx = glyphOffset;
    for (int n12 = 0; n12 < 16; ++n12) {
      const uint8_t color = ag_[agIdx >= 0 && agIdx < 200 ? agIdx : 0];
      ++agIdx;
      for (int col = 0; col < 15; ++col) {
        if (srcIdx >= 0 && srcIdx < 7905 && mask[srcIdx] != 0) {
          if (dst >= 0 && dst < kAySize)         ay_[dst] = color;
          if (dst + 272 >= 0 && dst + 272 < kAySize) ay_[dst + 272] = color;
        }
        ++srcIdx;
        ++dst;
      }
      dst += 305;
    }
    return;
  }
  if (slot >= 11) {
    int agIdx = 6 + slot * 17 + xShift;
    int dst = 1936 + (slot * 17 + xShift) * 320;
    int srcIdx = glyphOffset + 15;
    for (int n17 = 0; n17 < -xShift; ++n17) {
      const uint8_t color = ag_[agIdx >= 0 && agIdx < 200 ? agIdx : 0];
      ++agIdx;
      for (int col = 0; col < 15; ++col) {
        if (srcIdx >= 0 && srcIdx < 7905 && mask[srcIdx] != 0) {
          if (dst >= 0 && dst < kAySize)         ay_[dst] = color;
          if (dst + 272 >= 0 && dst + 272 < kAySize) ay_[dst + 272] = color;
        }
        ++srcIdx;
        ++dst;
      }
      dst += 305;
    }
  }
}

void OldschoolIntro::drawVerticalScroller() {
  // i() — 12 glyphs vertically stacked (mirrored at +272 px in ay).
  if (smallAi_ < 0) smallAi_ = 0;
  smallAh_ -= 2;
  if (smallAh_ <= -16) {
    smallAh_ = 0;
    ++smallAi_;
  }
  const char *bs = oldschool::kTextBsVert;
  const int bsLen = static_cast<int>(strlen(bs));
  int cursor = smallAi_;
  int consumed = 0;
  for (int n2 = 0; n2 < 12; ++n2) {
    if (cursor >= bsLen) {
      cursor = 0;
      consumed = 0;
    }
    int ch = substituteChar(bs[cursor++], /*bigFont=*/false);
    ++consumed;
    const int glyph = ch - 65;
    if (glyph >= 0 && glyph < kSmallGlyphCount) {
      blitVerticalGlyph(n2, smallAh_, glyph * kSmallGlyphStride + 15);
    }
  }
  smallAi_ -= consumed;
  if (smallAi_ < 0) smallAi_ = 0;
}

void OldschoolIntro::blitHorizontalGlyph(int slot, int xShift, int glyphOffset) {
  // Mirrors Scrolls88.g(): one 15-wide × 16-row glyph at column slot*16+e,
  // vertical baseline ad[adCursor_++].
  const int yBase = ad_[adCursor_ % 1000];
  ++adCursor_;
  const int baseCol = slot * 16 + xShift;
  const uint8_t *mask = oldschool::kSmallFont;
  if (slot == 0) {
    int dstBase = (yBase) * 320 + baseCol;
    int srcStart = glyphOffset - xShift;
    for (int n8 = 0; n8 < 17; ++n8) {
      const uint8_t color = af_[(yBase + n8) >= 0 && (yBase + n8) < 200 ? (yBase + n8) : 0];
      const int dst = dstBase + n8 * 320;
      for (int col = -xShift; col < 15; ++col) {
        if (srcStart + col >= 0 && srcStart + col < 7905 && mask[srcStart + col] != 0) {
          const int off = dst + col;
          if (off >= 0 && off < kAySize) ay_[off] = color;
        }
      }
      srcStart += 15;
    }
    return;
  }
  if (slot != 0 && slot != 20) {
    int srcIdx = glyphOffset;
    for (int n12 = 0; n12 < 17; ++n12) {
      const uint8_t color = af_[(yBase + n12) >= 0 && (yBase + n12) < 200 ? (yBase + n12) : 0];
      const int dst = (yBase + n12) * 320 + baseCol;
      for (int col = 0; col < 15; ++col) {
        if (srcIdx >= 0 && srcIdx < 7905 && mask[srcIdx] != 0) {
          const int off = dst + col;
          if (off >= 0 && off < kAySize) ay_[off] = color;
        }
        ++srcIdx;
      }
    }
    return;
  }
  if (slot == 20 && xShift != 0) {
    int srcStart = glyphOffset;
    for (int n14 = 0; n14 < 17; ++n14) {
      const uint8_t color = af_[(yBase + n14) >= 0 && (yBase + n14) < 200 ? (yBase + n14) : 0];
      const int dst = (yBase + n14) * 320 + baseCol;
      for (int col = 0; col < -xShift; ++col) {
        if (srcStart + col >= 0 && srcStart + col < 7905 && mask[srcStart + col] != 0) {
          const int off = dst + col;
          if (off >= 0 && off < kAySize) ay_[off] = color;
        }
      }
      srcStart += 15;
    }
  }
}

void OldschoolIntro::drawHorizontalScroller() {
  // k() — 21 glyphs of bu waving across the canvas.
  if (smallF_ < 0) smallF_ = 0;
  smallE_ -= 2;
  if (smallE_ <= -15) {
    smallE_ = 0;
    ++smallF_;
    ++adCursor_;
  }
  const char *bu = oldschool::kTextBuLarge;
  const int buLen = static_cast<int>(strlen(bu));
  int cursor = smallF_;
  int consumed = 0;
  for (int n2 = 0; n2 < 21; ++n2) {
    if (cursor >= buLen) {
      cursor = 0;
      consumed = 0;
    }
    int ch = substituteChar(bu[cursor++], /*bigFont=*/false);
    ++consumed;
    const int glyph = ch - 65;
    if (glyph >= 0 && glyph < kSmallGlyphCount) {
      blitHorizontalGlyph(n2, smallE_, glyph * kSmallGlyphStride);
    } else {
      ++adCursor_;
    }
  }
  adCursor_ -= 20;
  if (adCursor_ >= 500) adCursor_ -= 500;
  if (adCursor_ < 0) adCursor_ = 0;

  smallF_ -= consumed;
  if (smallF_ < 0) smallF_ = 0;
}

void OldschoolIntro::drawBannerOverlay() {
  // q() — write d[] onto ay starting at offset 322, in 11 outer iters
  // of 296 bytes-with-conditional-writes followed by +20 skip.
  int n  = 322;
  int n2 = 0;
  for (int row = 0; row < 11; ++row) {
    for (int i = 0; i < 74; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (n2 < kDSize && n < kAySize) {
          const uint8_t by = d_[n2++];
          if (by != 0) ay_[n] = by;
        }
        ++n;
      }
    }
    n += 20;
  }
}

void OldschoolIntro::tick(uint32_t nowMs) {
  (void)nowMs;
  if (!initialised_) return;

  drawBigScrollerStrip();
  drawDotRing();
  drawVerticalScroller();
  drawHorizontalScroller();
  drawBannerOverlay();

  ++frameCounter_;
}
