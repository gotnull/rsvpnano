#include "demos/Pupul.h"

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <algorithm>
#include <cmath>
#include <cstring>

#include "demos/pupul_assets.h"

// Mirrors the Java applet's scrolltext verbatim. The leading/trailing
// spaces matter — they're how the message enters/exits the screen
// without abrupt pop-in.
static const char *const kScrollText =
    "       HI KIDS! THIS IS THE GREAT PUPUL INTRO COMING BACK FROM AGES!!!"
    "  THIS JAVA VERSION IS NEARLY THE SAME AS OUR ORIGINAL ATARI INTRO"
    " CODED TEN YEARS AGO...  TIME TO WRAP!      ";

namespace {

// Map an ASCII char into its top-left pixel index into fonte.gif's 320×78
// indexed buffer. Matches the magic constants in Pupul.b()/c(). Out-of-
// range characters return -1 → caller paints a transparent cell.
int fontOffsetFor(char ch) {
  if (ch == '!') return 16832;
  if (ch == '.') return 16864;
  if (ch == '?') return 16896;
  if (ch < 'A') return -1;          // blank / digits / lowercase
  const int c = ch - 'A';
  if (c >= 26) return -1;
  if (c >= 20) return (c - 20) * 32 + 16640;  // row 2: U..Z
  if (c >= 10) return (c - 10) * 32 + 8320;   // row 1: K..T
  return c * 32;                              // row 0: A..J
}

}  // namespace

bool Pupul::begin(uint32_t nowMs) {
  (void)nowMs;
  if (initialised_) return true;
  if (frame_ == nullptr) {
    const size_t frameBytes = kCanvasW * kCanvasH * sizeof(uint16_t);
    frame_ = static_cast<uint16_t *>(
        heap_caps_malloc(frameBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (frame_ == nullptr) {
      frame_ = static_cast<uint16_t *>(heap_caps_malloc(frameBytes, MALLOC_CAP_8BIT));
    }
    if (frame_ == nullptr) {
      Serial.println("[pupul] FATAL: frame_ alloc failed");
      return false;
    }
  }
  if (ac_ == nullptr) {
    const size_t bgBytes = kCanvasW * kCanvasBgH * sizeof(uint16_t);
    ac_ = static_cast<uint16_t *>(
        heap_caps_malloc(bgBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (ac_ == nullptr) {
      ac_ = static_cast<uint16_t *>(heap_caps_malloc(bgBytes, MALLOC_CAP_8BIT));
    }
    if (ac_ == nullptr) {
      Serial.println("[pupul] FATAL: ac_ alloc failed");
      return false;
    }
  }

  std::memset(frame_, 0, kCanvasW * kCanvasH * sizeof(uint16_t));
  blitBackgroundIntoFrame();
  // Snapshot the pristine background so the raster-bar XOR/restore pattern
  // can undo its inversions cleanly each frame.
  std::memcpy(ac_, frame_, kCanvasW * kCanvasBgH * sizeof(uint16_t));

  precomputePath();
  initRasterBars();
  resetScrollerSlots();

  an_ = 0;
  aj_ = 1;
  am_ = 1;
  ar_ = 0;
  ap_ = 0;
  ao_ = 0;
  g_ = 0;
  q_ = 0;
  initialised_ = true;
  return true;
}

void Pupul::end() {
  if (frame_) {
    heap_caps_free(frame_);
    frame_ = nullptr;
  }
  if (ac_) {
    heap_caps_free(ac_);
    ac_ = nullptr;
  }
  initialised_ = false;
}

void Pupul::precomputePath() {
  // Five segments with different angular deltas — see PORT_NOTES.md.
  // Position formula (az=32, aw=26):
  //   u[i] = 160 - 16 + (128 - 16 - 5) * cos(α)   = 144 + 107*cos(α)
  //   t[i] = 98  - 13 + 30 + (65.333 - 13 - 10) * sin(β) = 115 + 42.333*sin(β)
  double a = 0.0;  // for u (cos)
  double b = 0.0;  // for t (sin)
  auto write = [&](int idx) {
    if (idx >= 0 && idx < 951) {
      u_[idx] = static_cast<int16_t>(144.0 + 107.0 * cos(a));
      t_[idx] = static_cast<int16_t>(115.0 + 42.3333 * sin(b));
    }
  };
  for (int i = 0; i < 125; ++i) {
    write(i);
    write(i + 940);
    a += 0.05;
    b += 0.055;
  }
  for (int i = 125; i < 220; ++i) {
    write(i);
    write(i + 940);
    a += 0.03;
    b += 0.035;
  }
  for (int i = 220; i < 480; ++i) {
    write(i);
    write(i + 940);
    a += 0.06;
    b += 0.03;
  }
  for (int i = 480; i < 630; ++i) {
    write(i);
    write(i + 940);
    a += 0.045;
    b += 0.035;
  }
  for (int i = 630; i < 940; ++i) {
    write(i);
    write(i + 940);
    a += 0.02;
    b += 0.045;
  }
  // Tail-end fill so the wrap doesn't see uninitialised entries.
  for (int i = 940; i < 951; ++i) {
    if (u_[i] == 0) u_[i] = u_[i - 1];
    if (t_[i] == 0) t_[i] = t_[i - 1];
  }
}

void Pupul::initRasterBars() {
  // Java: `v[i] = 5*i + 4` for i in 0..20, then z[i] = 296 / v[i] + 110.
  for (int i = 0; i < 20; ++i) {
    v_[i] = 5 * i + 4;
    z_[i] = 296 / v_[i] + 110;
  }
}

void Pupul::blitBackgroundIntoFrame() {
  // fondpupul2 fills rows 0..195 of the canvas. Convert palette indices
  // to RGB565 once here; the per-frame raster logic only touches small
  // bar regions afterwards.
  const auto &bg = pupul_assets::kFondMain;
  for (int y = 0; y < bg.height && y < kCanvasBgH; ++y) {
    uint16_t *dstRow = frame_ + y * kCanvasW;
    const uint8_t *srcRow = bg.pixels + y * bg.width;
    const int w = bg.width < kCanvasW ? bg.width : kCanvasW;
    for (int x = 0; x < w; ++x) {
      dstRow[x] = bg.palette[srcRow[x]];
    }
  }
}

void Pupul::resetScrollerSlots() {
  for (int n = 0; n < kScrollerCells; ++n) {
    af_[n] = n * 32;
    // Pre-seed every cell with the next character from the script so the
    // marquee enters with real glyphs, not blanks.
    rebuildScrollerCell(n);
  }
}

void Pupul::rebuildScrollerCell(int n) {
  if (n < 0 || n >= kScrollerCells) return;
  const size_t textLen = strlen(kScrollText);
  if (textLen == 0) {
    std::memset(cellRgb_[n], 0, sizeof(cellRgb_[n]));
    return;
  }
  const char ch = kScrollText[q_];
  q_ = (q_ + 1) % static_cast<int>(textLen);
  const int srcOffset = fontOffsetFor(ch);
  if (srcOffset < 0) {
    std::memset(cellRgb_[n], 0, sizeof(cellRgb_[n]));
    return;
  }
  const auto &font = pupul_assets::kFont;
  // Cell is 25 rows of 32 pixels each, sampled from fonte.gif (320-wide).
  for (int row = 0; row < kCellH; ++row) {
    const int srcLinear = srcOffset + row * font.width;
    const uint8_t *srcRow = font.pixels + srcLinear;
    uint16_t *dstRow = cellRgb_[n] + row * kCellW;
    for (int col = 0; col < kCellW; ++col) {
      const uint8_t idx = srcRow[col];
      // Treat the GIF's transparent index as bg-coloured pixels in the
      // pre-rendered cell. The renderer paints these flat over the
      // scroller strip background, so plain 0x0000 reads as black.
      dstRow[col] =
          (font.transparentIdx != 0xFF && idx == font.transparentIdx)
              ? 0x0000
              : font.palette[idx];
    }
  }
}

void Pupul::tick(uint32_t nowMs) {
  (void)nowMs;
  if (!initialised_) return;
  ++ar_;
  // Restore the entire pristine background each frame. The Java applet's
  // equivalent is `drawImage(a_[0], 0, 73, 320, 200, ...)` after the bar
  // restore — it re-blits the bg buffer wholesale BEFORE dragons are
  // drawn, so dragon stamps never leave trails. Earlier ports only
  // restored bar rows and got the "dragons paint a trail" bug.
  std::memcpy(frame_, ac_, kCanvasW * kCanvasBgH * sizeof(uint16_t));
  advanceBarsAndPaintStrip();
  paintBottomBorder();
  advanceDragonFsm();
  stampDragons();
  advanceScroller();
  // Composite the scroller strip into the framebuffer so the renderer can
  // read uniformly. The scroller area in the original is y=200..224 plus
  // the gap rows 196..199 between bg and scroller — both painted black
  // each frame so the marquee doesn't leave trails as cells slide left.
  const size_t scrollerClearRows = kCanvasH - kCanvasBgH;  // 32 rows below bg
  const size_t scrollerClearBytes =
      scrollerClearRows * kCanvasW * sizeof(uint16_t);
  std::memset(frame_ + kCanvasBgH * kCanvasW, 0, scrollerClearBytes);
  for (int n = 0; n < kScrollerCells; ++n) {
    const int xStart = af_[n];
    if (xStart >= kCanvasW) continue;
    if (xStart + kCellW <= 0) continue;
    for (int row = 0; row < kCellH; ++row) {
      const int dstY = kScrollerY + row;
      if (dstY < 0 || dstY >= kCanvasH) continue;
      uint16_t *dstRow = frame_ + dstY * kCanvasW;
      const uint16_t *srcRow = cellRgb_[n] + row * kCellW;
      const int colStart = std::max(0, -xStart);
      const int colEnd = std::min(kCellW, kCanvasW - xStart);
      for (int col = colStart; col < colEnd; ++col) {
        dstRow[xStart + col] = srcRow[col];
      }
    }
  }
}

void Pupul::advanceBarsAndPaintStrip() {
  // Step phase: each pair of v[] ticks down; if either pair fully drains,
  // reset to (95, 100). z[] derives from v[] each frame.
  for (int i = 0; i < 10; ++i) {
    z_[i * 2 + 1] = 296 / v_[i * 2 + 1] + 110;
    z_[i * 2] = 296 / v_[i * 2] + 110;
    --v_[i * 2];
    --v_[i * 2 + 1];
    if (v_[i * 2 + 1] <= 0) {
      v_[i * 2] = 95;
      v_[i * 2 + 1] = 100;
    }
    if (v_[i * 2] <= 0) {
      v_[i * 2] = 1;
    }
  }
  // Paint fondpupul22 into the bar rows. Source row = barRow - 110 so the
  // strip image lines up with the bar's vertical position.
  const auto &strip = pupul_assets::kFondStrip;
  for (int i = 0; i < 10; ++i) {
    const int top = z_[i * 2 + 1];
    int bot = z_[i * 2];
    if (bot > kCanvasBgH) bot = kCanvasBgH;
    const int rowCount = bot - top;
    if (rowCount <= 0) continue;
    for (int r = 0; r < rowCount; ++r) {
      const int dstRow = top + r;
      const int srcRow = dstRow - 110;
      if (srcRow < 0 || srcRow >= strip.height) continue;
      uint16_t *dst = frame_ + dstRow * kCanvasW;
      const uint8_t *src = strip.pixels + srcRow * strip.width;
      for (int x = 0; x < kCanvasW && x < strip.width; ++x) {
        dst[x] = strip.palette[src[x]];
      }
    }
  }
}

void Pupul::paintBottomBorder() {
  // Java filled row 195 (the last row of the bg area) with -4482748. That
  // value is 0xFFBBBBC4 → RGB ≈ (0xBB, 0xBB, 0xC4). Convert once.
  static const uint16_t kBorderRgb565 =
      static_cast<uint16_t>(((0xBB & 0xF8) << 8) | ((0xBB & 0xFC) << 3) | (0xC4 >> 3));
  uint16_t *row = frame_ + (kCanvasBgH - 1) * kCanvasW;
  for (int x = 0; x < kCanvasW; ++x) row[x] = kBorderRgb565;
}

void Pupul::advanceDragonFsm() {
  switch (an_) {
    case 0:
      aj_ = 1;
      if (ar_ == 250) {
        an_ = 2;
        ar_ = 0;
        am_ = 1;
      }
      break;
    case 1:
      break;
    case 2:
      if ((ar_ % 2) == 0) {
        aj_ += am_;
      }
      if (aj_ == 8) am_ = -1;
      if (aj_ == 5) am_ = 1;
      if (ar_ == 400) {
        an_ = 3;
        ar_ = 0;
      }
      break;
    case 3:
      if ((ar_ % 2) == 0) {
        --aj_;
      }
      if (aj_ <= 1) {
        an_ = 0;
        ++g_;
        ar_ = 0;
      }
      break;
    default:
      break;
  }
  if (aj_ < 1) aj_ = 1;
  if (aj_ > 8) aj_ = 8;

  // Counters tick after FSM evaluation. ap_ wraps at 939, ao_ at 950.
  ++ap_;
  if (ap_ > 939) ap_ = 0;
  ++ao_;
  if (ao_ > 950) ao_ = 0;
}

void Pupul::stampDragons() {
  const int frameIdx = aj_ - 1;
  const auto *sprite = pupul_assets::kDragon[frameIdx];
  // Seven trailing dragons, each offset by n*16 along the precomputed
  // path; plus the main dragon at the current head positions.
  for (int n = 0; n < 7; ++n) {
    const int idxAp = (ap_ + n * 16) % 940;
    const int idxAo = (ao_ + n * 16) % 951;
    blitIndexedSpriteTransparent(u_[idxAp], t_[idxAo], sprite->pixels,
                                 sprite->palette, sprite->width,
                                 sprite->height, sprite->transparentIdx);
  }
  blitIndexedSpriteTransparent(u_[ap_], t_[ao_], sprite->pixels,
                               sprite->palette, sprite->width,
                               sprite->height, sprite->transparentIdx);
}

void Pupul::advanceScroller() {
  for (int n = 0; n < kScrollerCells; ++n) {
    af_[n] -= 6;
    if (af_[n] <= -32) {
      af_[n] += 352;
      rebuildScrollerCell(n);
    }
  }
}

void Pupul::blitIndexedSpriteOpaque(int dstX, int dstY, const uint8_t *pixels,
                                    const uint16_t *palette, int w, int h) {
  for (int row = 0; row < h; ++row) {
    const int y = dstY + row;
    if (y < 0 || y >= kCanvasBgH) continue;
    uint16_t *dstRow = frame_ + y * kCanvasW;
    const uint8_t *srcRow = pixels + row * w;
    for (int col = 0; col < w; ++col) {
      const int x = dstX + col;
      if (x < 0 || x >= kCanvasW) continue;
      dstRow[x] = palette[srcRow[col]];
    }
  }
}

void Pupul::blitIndexedSpriteTransparent(int dstX, int dstY,
                                         const uint8_t *pixels,
                                         const uint16_t *palette, int w, int h,
                                         uint8_t transparentIdx) {
  for (int row = 0; row < h; ++row) {
    const int y = dstY + row;
    if (y < 0 || y >= kCanvasBgH) continue;
    uint16_t *dstRow = frame_ + y * kCanvasW;
    const uint8_t *srcRow = pixels + row * w;
    for (int col = 0; col < w; ++col) {
      const uint8_t idx = srcRow[col];
      if (idx == transparentIdx) continue;
      const int x = dstX + col;
      if (x < 0 || x >= kCanvasW) continue;
      dstRow[x] = palette[idx];
    }
  }
}

void Pupul::copyRow(uint16_t *dst, const uint16_t *src, int count) {
  std::memcpy(dst, src, static_cast<size_t>(count) * sizeof(uint16_t));
}
