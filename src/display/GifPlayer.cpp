#include "display/GifPlayer.h"

#include <esp_heap_caps.h>

namespace {

// Single-instance dispatch. App owns exactly one GifPlayer; the AnimatedGIF
// callbacks are C-style and need a way to reach our methods. A static
// pointer is simpler (and safer here) than wrapping the void *pUser hop.
GifPlayer *sActive = nullptr;

// Cap framebuffer dimensions so a malformed GIF can't allocate hundreds of
// MB. Real-world GIFs we want to show are well under this.
constexpr int kMaxCanvasW = 480;
constexpr int kMaxCanvasH = 480;

}  // namespace

GifPlayer::GifPlayer() = default;

GifPlayer::~GifPlayer() { end(); }

void GifPlayer::setError(const char *msg) {
  if (msg == nullptr) {
    errorMsg_[0] = '\0';
    return;
  }
  strncpy(errorMsg_, msg, sizeof(errorMsg_) - 1);
  errorMsg_[sizeof(errorMsg_) - 1] = '\0';
}

bool GifPlayer::begin(const String &path) {
  end();

  path_ = path;
  file_ = SD_MMC.open(path.c_str(), FILE_READ);
  if (!file_) {
    setError("open failed");
    return false;
  }

  sActive = this;
  gif_.begin(GIF_PALETTE_RGB565_LE);
  const int ok = gif_.open(path.c_str(), &GifPlayer::openCb, &GifPlayer::closeCb,
                           &GifPlayer::readCb, &GifPlayer::seekCb, &GifPlayer::drawCb);
  if (!ok) {
    Serial.printf("[gif] open failed err=%d path=%s\n", gif_.getLastError(), path.c_str());
    setError("decode init failed");
    file_.close();
    sActive = nullptr;
    return false;
  }
  gifOpen_ = true;
  width_ = gif_.getCanvasWidth();
  height_ = gif_.getCanvasHeight();
  if (width_ <= 0 || height_ <= 0 || width_ > kMaxCanvasW || height_ > kMaxCanvasH) {
    Serial.printf("[gif] reject dims %dx%d (max %dx%d)\n", width_, height_, kMaxCanvasW,
                  kMaxCanvasH);
    setError("dimensions too large");
    gif_.close();
    gifOpen_ = false;
    file_.close();
    sActive = nullptr;
    return false;
  }

  const size_t needed = static_cast<size_t>(width_) * static_cast<size_t>(height_) *
                        sizeof(uint16_t);
  if (needed != frameBytes_) {
    if (frame_) heap_caps_free(frame_);
    frame_ = static_cast<uint16_t *>(heap_caps_malloc(needed, MALLOC_CAP_SPIRAM));
    if (!frame_) {
      setError("psram alloc failed");
      gif_.close();
      gifOpen_ = false;
      file_.close();
      sActive = nullptr;
      frameBytes_ = 0;
      return false;
    }
    frameBytes_ = needed;
  }
  // Black initial frame so any transparent regions before first decode show
  // a known background instead of PSRAM garbage.
  memset(frame_, 0, frameBytes_);

  playing_ = true;
  nextFrameAtMs_ = millis();
  setError(nullptr);
  Serial.printf("[gif] opened %s canvas=%dx%d\n", path.c_str(), width_, height_);
  return true;
}

void GifPlayer::end() {
  if (gifOpen_) {
    gif_.close();
    gifOpen_ = false;
  }
  if (file_) file_.close();
  if (frame_) {
    heap_caps_free(frame_);
    frame_ = nullptr;
    frameBytes_ = 0;
  }
  width_ = 0;
  height_ = 0;
  playing_ = false;
  if (sActive == this) sActive = nullptr;
}

bool GifPlayer::reopen() {
  if (!gifOpen_) return false;
  gif_.close();
  gifOpen_ = false;
  if (file_) file_.close();
  file_ = SD_MMC.open(path_.c_str(), FILE_READ);
  if (!file_) {
    setError("reopen failed");
    return false;
  }
  const int ok = gif_.open(path_.c_str(), &GifPlayer::openCb, &GifPlayer::closeCb,
                           &GifPlayer::readCb, &GifPlayer::seekCb, &GifPlayer::drawCb);
  if (!ok) {
    setError("reopen decode failed");
    file_.close();
    return false;
  }
  gifOpen_ = true;
  return true;
}

bool GifPlayer::tick(uint32_t nowMs) {
  if (!playing_ || !gifOpen_) return false;
  if (static_cast<int32_t>(nowMs - nextFrameAtMs_) < 0) return false;

  int delayMs = 0;
  // bSync=false → playFrame does not block on the GIF's per-frame delay;
  // we own the pacing via nextFrameAtMs_.
  const int more = gif_.playFrame(false, &delayMs);
  // GIF per-frame delay floors at ~16 ms to keep the display loop sane on
  // fast GIFs that claim 0/10 ms. AnimatedGIF returns the GIF's authored
  // delay in milliseconds.
  const int paced = delayMs < 16 ? 16 : delayMs;
  nextFrameAtMs_ = nowMs + static_cast<uint32_t>(paced);

  if (more <= 0) {
    if (looping_) {
      if (!reopen()) {
        playing_ = false;
        return false;
      }
      nextFrameAtMs_ = nowMs;
      // Reset canvas so frame 1 of the new loop doesn't inherit leftover
      // pixels from the last loop's final frame.
      if (frame_ && frameBytes_) memset(frame_, 0, frameBytes_);
      return true;
    }
    playing_ = false;
    return false;
  }
  return true;
}

void GifPlayer::plotScanline(int x, int y, const uint16_t *src, int len,
                             bool hasTransparency, uint8_t transparentIndex,
                             const uint16_t *palette, const uint8_t *indexedPixels) {
  (void)src;
  if (!frame_ || y < 0 || y >= height_) return;
  uint16_t *row = frame_ + static_cast<ptrdiff_t>(y) * width_;
  const int xEnd = x + len;
  const int clampedEnd = (xEnd > width_) ? width_ : xEnd;
  for (int i = 0, dx = x; dx < clampedEnd; ++i, ++dx) {
    if (dx < 0) continue;
    const uint8_t idx = indexedPixels[i];
    if (hasTransparency && idx == transparentIndex) continue;
    row[dx] = palette[idx];
  }
}

// ---- AnimatedGIF static callbacks ----

void *GifPlayer::openCb(const char *fname, int32_t *psize) {
  (void)fname;  // single-instance — we already opened the file in begin().
  if (!sActive || !sActive->file_) {
    if (psize) *psize = 0;
    return nullptr;
  }
  sActive->file_.seek(0);
  if (psize) *psize = static_cast<int32_t>(sActive->file_.size());
  return &sActive->file_;
}

void GifPlayer::closeCb(void *handle) {
  (void)handle;
  // No-op: file lifetime is managed by GifPlayer::end().
}

int32_t GifPlayer::readCb(GIFFILE *file, uint8_t *buf, int32_t len) {
  File *f = static_cast<File *>(file->fHandle);
  if (!f || !*f) return 0;
  const int32_t got = static_cast<int32_t>(f->read(buf, static_cast<size_t>(len)));
  file->iPos = static_cast<int32_t>(f->position());
  return got;
}

int32_t GifPlayer::seekCb(GIFFILE *file, int32_t pos) {
  File *f = static_cast<File *>(file->fHandle);
  if (!f || !*f) return 0;
  f->seek(static_cast<uint32_t>(pos));
  file->iPos = static_cast<int32_t>(f->position());
  return file->iPos;
}

void GifPlayer::drawCb(GIFDRAW *pDraw) {
  if (!sActive || pDraw == nullptr) return;
  // pDraw->iY is the frame's top-left Y within the canvas; pDraw->y is the
  // current scanline within this frame's area. Absolute canvas Y = iY + y.
  const int absY = pDraw->iY + pDraw->y;
  // pDraw->pPixels is indexed pixels (8-bit) for this scanline. Palette is
  // RGB565 in the endianness we requested (little-endian → native uint16_t).
  sActive->plotScanline(pDraw->iX, absY, nullptr, pDraw->iWidth,
                        pDraw->ucHasTransparency != 0, pDraw->ucTransparent,
                        reinterpret_cast<const uint16_t *>(pDraw->pPalette),
                        pDraw->pPixels);
}
