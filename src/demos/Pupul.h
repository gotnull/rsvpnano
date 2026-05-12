#pragma once

#include <stdint.h>

// Pupul — port of the 1989 Atari ST Equinox intro (Java applet version,
// 2000). Renders into an internal 320×228 RGB565 canvas that
// DisplayManager::renderPupulFrame() then maps to the 640×172 panel with
// a 2× horizontal scale + 0.75× vertical squash.
//
// See tools/demoref/equinox/pupul/PORT_NOTES.md for the full spec.
class Pupul {
 public:
  static constexpr int kCanvasW = 320;
  static constexpr int kCanvasH = 228;
  // Background pixel buffer covers rows 0..195. Scroller cells live at
  // y=200..224 in the canvas (separate sprite array).
  static constexpr int kCanvasBgH = 196;
  static constexpr int kScrollerY = 200;
  static constexpr int kScrollerH = 25;
  static constexpr int kScrollerCells = 11;
  static constexpr int kCellW = 32;
  static constexpr int kCellH = 25;
  static constexpr int kDragonW = 32;
  static constexpr int kDragonH = 26;

  Pupul() = default;

  // Allocates the PSRAM buffers, snapshots the pristine background, builds
  // the precomputed dragon Lissajous path, primes the raster-bar state
  // machine, and seeds the scroller. Idempotent.
  bool begin(uint32_t nowMs);
  void end();

  // Advance state by one frame. Caller invokes this at panel rate; the
  // demo's per-frame logic mirrors Pupul.b() in the Java source.
  void tick(uint32_t nowMs);

  // Read-only accessors used by DisplayManager::renderPupulFrame.
  const uint16_t *framebuffer() const { return frame_; }
  const uint16_t *scrollerCellPixels(int cell) const {
    return cellRgb_[cell];
  }
  const int *scrollerCellXs() const { return af_; }

  // For the profiling line.
  uint32_t frameCounter() const { return ar_; }

 private:
  // Lifecycle / init helpers.
  void precomputePath();
  void resetScrollerSlots();
  void initRasterBars();
  void blitBackgroundIntoFrame();

  // Per-frame helpers (mirror Pupul.b() steps).
  void advanceBarsAndPaintStrip();
  void paintBottomBorder();
  void advanceDragonFsm();
  void stampDragons();
  void advanceScroller();
  void rebuildScrollerCell(int cell);

  // Blit helpers (write into frame_ at canvas coords).
  void blitIndexedSpriteOpaque(int dstX, int dstY, const uint8_t *pixels,
                               const uint16_t *palette, int w, int h);
  void blitIndexedSpriteTransparent(int dstX, int dstY, const uint8_t *pixels,
                                    const uint16_t *palette, int w, int h,
                                    uint8_t transparentIdx);
  // Copy raw RGB565 row from src into dst (`count` pixels).
  static void copyRow(uint16_t *dst, const uint16_t *src, int count) ;

  uint16_t *frame_ = nullptr;   // 320 × 228 RGB565 (PSRAM)
  uint16_t *ac_ = nullptr;      // 320 × 196 pristine bg snapshot (PSRAM)

  // 10 raster bars, paired Y-positions / speeds (mirrors Java z[], v[]).
  int v_[20];
  int z_[20];

  // Dragon path — 951 precomputed (u, t) positions sampled by ap_ / ao_.
  int16_t u_[951];
  int16_t t_[951];

  // Dragon FSM — animation state machine on `an_`. Frame index 1..8 in aj_.
  int an_ = 0;
  int aj_ = 1;
  int am_ = 1;
  uint32_t ar_ = 0;
  int ap_ = 0;
  int ao_ = 0;
  uint32_t g_ = 0;

  // Scroller — 11 cells of 32×25, each pre-rendered to RGB565. af_ holds
  // each cell's logical X on the canvas (advances left, wraps right).
  int af_[kScrollerCells];
  uint16_t cellRgb_[kScrollerCells][kCellW * kCellH];
  int q_ = 0;  // index into the scroller text

  bool initialised_ = false;
};
