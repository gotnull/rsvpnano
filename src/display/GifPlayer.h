#pragma once

// SD-backed GIF decoder for rsvpnano. Wraps bitbank2/AnimatedGIF, holds a
// PSRAM frame buffer in RGB565 (panel-agnostic — DisplayManager handles the
// native-stripe fit + effects layer), and paces frames against the GIF's
// embedded per-frame delay.
//
// Lifecycle:
//   begin(path) → tick(nowMs) each render loop → end()
//
// Tick is non-blocking: it returns false unless the GIF's frame-delay window
// has elapsed since the previous decoded frame. App::update() is the sole
// owner; no FreeRTOS task. Reading from SD_MMC is synchronous but fast
// (<5 ms for typical 128x128 frames).

#include <AnimatedGIF.h>
#include <Arduino.h>
#include <SD_MMC.h>

class GifPlayer {
 public:
  GifPlayer();
  ~GifPlayer();

  // Opens `path` (full SD path, e.g. "/gifs/foo.gif"). Returns false on any
  // failure (file missing, parse error, OOM, dimensions too large). The
  // PSRAM framebuffer is reallocated on each begin() to fit the file.
  bool begin(const String &path);
  // Frees the framebuffer + closes the file. Safe to call on a stopped instance.
  void end();

  bool isPlaying() const { return playing_; }
  // Loop the GIF on end-of-stream (default true). Toggle before begin() or
  // mid-playback.
  void setLooping(bool on) { looping_ = on; }
  bool isLooping() const { return looping_; }

  // Advance the decoder if enough time has passed. Returns true when the
  // framebuffer now holds a newly-decoded frame the caller should ship to
  // the display. Returns false when no work was due, when end-of-stream
  // with looping=false was reached, or on a decode error.
  bool tick(uint32_t nowMs);

  // Active-frame view. nullptr until the first successful tick() with a
  // new frame; subsequent ticks may or may not produce a new frame, but
  // the buffer always reflects the latest decoded frame after the first.
  const uint16_t *frame() const { return frame_; }
  int width() const { return width_; }
  int height() const { return height_; }

  // Most recent error string for status display. Empty when none.
  const char *error() const { return errorMsg_; }

  // Per-pixel callback used internally. Exposed so the static AnimatedGIF
  // callback can dispatch into the active instance.
  void plotScanline(int x, int y, const uint16_t *src, int len, bool hasTransparency,
                    uint8_t transparentIndex, const uint16_t *palette,
                    const uint8_t *indexedPixels);

 private:
  static void *openCb(const char *fname, int32_t *psize);
  static void closeCb(void *handle);
  static int32_t readCb(GIFFILE *file, uint8_t *buf, int32_t len);
  static int32_t seekCb(GIFFILE *file, int32_t pos);
  static void drawCb(GIFDRAW *pDraw);

  bool reopen();
  void setError(const char *msg);

  AnimatedGIF gif_{};
  String path_;
  File file_{};
  uint16_t *frame_ = nullptr;
  size_t frameBytes_ = 0;
  int width_ = 0;
  int height_ = 0;
  bool gifOpen_ = false;
  bool playing_ = false;
  bool looping_ = true;
  uint32_t nextFrameAtMs_ = 0;
  char errorMsg_[80] = {0};
};
