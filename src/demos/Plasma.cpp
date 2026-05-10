#include "demos/Plasma.h"

#include <math.h>
#include <string.h>

namespace {

uint32_t splitmix32(uint32_t &state) {
  uint32_t z = (state += 0x9E3779B9u);
  z = (z ^ (z >> 16)) * 0x85EBCA6Bu;
  z = (z ^ (z >> 13)) * 0xC2B2AE35u;
  return z ^ (z >> 16);
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

uint8_t lerp8(uint8_t a, uint8_t b, uint8_t t) {
  return static_cast<uint8_t>(a + ((static_cast<int>(b) - a) * t) / 255);
}

// File-static sin LUT, populated on first call to ensureSinLut(). Q.7 fixed
// point: sinLut[i] = round(sin(2*pi*i/256) * 127), int8 in [-127, 127].
int8_t gSinLut[Plasma::kSinLutSize];
bool gSinLutReady = false;

void ensureSinLut() {
  if (gSinLutReady) return;
  for (int i = 0; i < Plasma::kSinLutSize; ++i) {
    const float x = (static_cast<float>(i) / Plasma::kSinLutSize) * 2.0f * 3.14159265f;
    const float v = sinf(x) * 127.0f;
    gSinLut[i] = static_cast<int8_t>(v < 0 ? v - 0.5f : v + 0.5f);
  }
  gSinLutReady = true;
}

}  // namespace

const int8_t *Plasma::sinLutPtr() { return gSinLut; }

void Plasma::buildPalette(uint32_t seedMs) {
  // Pick three random saturated colors and ramp between them in palette_.
  // Uses thirds of the table so the cycle wraps smoothly stop[2]→stop[0].
  uint32_t s = (seedMs != 0) ? seedMs : 0xC0FFEE42u;
  uint8_t stops[3][3];
  for (int k = 0; k < 3; ++k) {
    const uint32_t r = splitmix32(s);
    stops[k][0] = static_cast<uint8_t>((r >>  0) & 0xFF);
    stops[k][1] = static_cast<uint8_t>((r >>  8) & 0xFF);
    stops[k][2] = static_cast<uint8_t>((r >> 16) & 0xFF);
    // Push the strongest channel to 255 so each stop has visual punch.
    int strongest = 0;
    for (int c = 1; c < 3; ++c) if (stops[k][c] > stops[k][strongest]) strongest = c;
    stops[k][strongest] = 255;
  }
  for (int i = 0; i < kPaletteSize; ++i) {
    const int seg = (i * 3) / kPaletteSize;             // 0,1,2
    const int segPos = (i * 3) - (seg * kPaletteSize);  // 0..(kPaletteSize-1)
    const uint8_t t = static_cast<uint8_t>((segPos * 255) / (kPaletteSize - 1));
    const uint8_t *a = stops[seg];
    const uint8_t *b = stops[(seg + 1) % 3];
    palette_[i] = rgb565(lerp8(a[0], b[0], t), lerp8(a[1], b[1], t), lerp8(a[2], b[2], t));
  }
}

void Plasma::begin(uint32_t seedMs) {
  ensureSinLut();
  buildPalette(seedMs);
  pa_ = 0.0f;
  pb_ = 0.0f;
  pc_ = 0.0f;
  pd_ = 0.0f;
  paletteShift_ = 0;
  lastMs_ = 0;
  lastShiftMs_ = 0;
}

void Plasma::tick(uint32_t nowMs) {
  const float dt = (lastMs_ == 0) ? 1.0f / 40.0f
                                  : static_cast<float>(nowMs - lastMs_) / 1000.0f;
  lastMs_ = nowMs;
  // Independent angular speeds keep the interference pattern non-periodic
  // for a long time.
  pa_ += dt * 1.7f;
  pb_ += dt * 1.1f;
  pc_ += dt * 2.3f;
  pd_ += dt * 0.6f;
  // Slow palette rotation — ~30 entries/sec.
  if (lastShiftMs_ == 0 || nowMs - lastShiftMs_ >= 33) {
    lastShiftMs_ = nowMs;
    ++paletteShift_;
  }
}
