#include "screensaver/StarfieldModes.h"

#include <math.h>

namespace screensaver {

uint32_t StarFieldRng::nextU32() {
  uint32_t x = state_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state_ = x;
  return x;
}

float StarFieldRng::nextFloat() {
  return static_cast<float>(nextU32()) / static_cast<float>(0xFFFFFFFFu);
}

namespace {

using Star = Screensaver::Star;

constexpr int16_t kSW = Screensaver::kScreenWidth;
constexpr int16_t kSH = Screensaver::kScreenHeight;

// ----------------------------------------------------------------------------
// Shared init — sets the tint and size for every star, then dispatches to the
// per-mode placement helper. Bias toward small stars; only ~20% get the big
// size to avoid the field looking like polka dots.
// ----------------------------------------------------------------------------
void seedCommonAppearance(Star& s, StarFieldRng& rng) {
  s.tint = rng.nextTint();
  const uint32_t sizeRoll = rng.nextU32() & 0xFFu;
  s.size = (sizeRoll < 200u) ? 0u : ((sizeRoll < 240u) ? 1u : 2u);
}

// ----------------------------------------------------------------------------
// Parallax tuning. Five layers, slowest → fastest (px/frame at 60 fps).
// More layers = busier feel + better depth illusion.
// ----------------------------------------------------------------------------
constexpr int kParallaxLayers = 5;
constexpr float kParallaxLayerSpeed[kParallaxLayers] = {0.5f, 1.1f, 1.8f, 2.7f, 4.0f};
constexpr uint8_t kParallaxLayerBright[kParallaxLayers] = {80, 130, 180, 220, 255};

// Vortex tuning. Inner stars whip fast, outer stars drift; recycle at
// kVortexMaxRadius keeps the field from extending past the panel forever.
constexpr float kVortexMaxRadius = 1.4f;
constexpr float kVortexRadialAccel = 0.012f;

// Rain tuning. Fixed slope so the streaks line up visually.
constexpr float kRainSlope = 0.45f;

// ----------------------------------------------------------------------------
// Forward3D — classic perspective starfield, stars stream toward viewer.
// ----------------------------------------------------------------------------
void initForward3D(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    seedCommonAppearance(s, rng);
    s.a = rng.nextFloat() * 2.0f - 1.0f;
    s.b = rng.nextFloat() * 2.0f - 1.0f;
    s.c = rng.nextFloat();  // z=1 far, 0 near
  }
}

void updateForward3D(Star* stars, int count, StarFieldRng& rng) {
  const int16_t halfW = kSW / 2;
  const int16_t halfH = kSH / 2;
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    s.c -= 0.02f;
    if (s.c <= 0.0f) {
      s.a = rng.nextFloat() * 2.0f - 1.0f;
      s.b = rng.nextFloat() * 2.0f - 1.0f;
      s.c = 1.0f;
      s.tint = rng.nextTint();
    }
    const float invZ = 1.0f / s.c;
    const int sx = halfW + static_cast<int>(s.a * halfW * invZ);
    const int sy = halfH + static_cast<int>(s.b * halfH * invZ);
    if (sx < 0 || sx >= kSW || sy < 0 || sy >= kSH) {
      s.brightness = 0;
      continue;
    }
    int b = static_cast<int>(255.0f * (1.0f - s.c));
    if (b < 0) b = 0;
    if (b > 255) b = 255;
    s.sx = static_cast<int16_t>(sx);
    s.sy = static_cast<int16_t>(sy);
    s.brightness = static_cast<uint8_t>(b);
  }
}

// ----------------------------------------------------------------------------
// Parallax — 5-layer horizontal scroll.
// ----------------------------------------------------------------------------
void initParallax(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    seedCommonAppearance(s, rng);
    s.a = rng.nextFloat() * static_cast<float>(kSW);
    s.b = rng.nextFloat() * static_cast<float>(kSH);
    const uint8_t layer = static_cast<uint8_t>(i % kParallaxLayers);
    s.c = layer / static_cast<float>(kParallaxLayers - 1);
  }
}

void updateParallax(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    int layer = static_cast<int>(s.c * (kParallaxLayers - 1) + 0.5f);
    if (layer < 0) layer = 0;
    if (layer >= kParallaxLayers) layer = kParallaxLayers - 1;
    s.a -= kParallaxLayerSpeed[layer];
    if (s.a < 0.0f) {
      s.a += static_cast<float>(kSW);
      s.b = rng.nextFloat() * static_cast<float>(kSH);
      s.tint = rng.nextTint();
    }
    s.sx = static_cast<int16_t>(s.a);
    s.sy = static_cast<int16_t>(s.b);
    s.brightness = kParallaxLayerBright[layer];
  }
}

// ----------------------------------------------------------------------------
// Vortex — stars spiral outward from centre with elliptical projection so
// the wide 640×172 panel fills end-to-end.
// ----------------------------------------------------------------------------
void initVortex(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    seedCommonAppearance(s, rng);
    s.a = rng.nextFloat() * 2.0f * static_cast<float>(M_PI);
    s.b = rng.nextFloat() * 0.3f;
    s.c = (rng.nextFloat() * 0.06f) + 0.02f;
  }
}

void updateVortex(Star* stars, int count, StarFieldRng& rng) {
  const int16_t halfW = kSW / 2;
  const int16_t halfH = kSH / 2;
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    s.b += kVortexRadialAccel * (0.3f + s.b);
    s.a += s.c;
    if (s.b > kVortexMaxRadius) {
      s.a = rng.nextFloat() * 2.0f * static_cast<float>(M_PI);
      s.b = rng.nextFloat() * 0.15f;
      s.c = (rng.nextFloat() * 0.06f) + 0.02f;
      s.tint = rng.nextTint();
    }
    const int sx = halfW + static_cast<int>(cosf(s.a) * s.b * halfW);
    const int sy = halfH + static_cast<int>(sinf(s.a) * s.b * halfH);
    if (sx < 0 || sx >= kSW || sy < 0 || sy >= kSH) {
      s.brightness = 0;
      continue;
    }
    float bf = s.b / kVortexMaxRadius;
    if (bf > 1.0f) bf = 1.0f;
    const int b = static_cast<int>(255.0f * (0.4f + 0.6f * bf));
    s.sx = static_cast<int16_t>(sx);
    s.sy = static_cast<int16_t>(sy);
    s.brightness = static_cast<uint8_t>(b);
  }
}

// ----------------------------------------------------------------------------
// Twinkle — random field of pulsing sparkles. Per-star rate varies so the
// field never beats in sync.
// ----------------------------------------------------------------------------
void initTwinkle(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    seedCommonAppearance(s, rng);
    s.a = rng.nextFloat() * static_cast<float>(kSW);
    s.b = rng.nextFloat() * static_cast<float>(kSH);
    s.c = rng.nextFloat();
  }
}

void updateTwinkle(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    s.c += 0.012f + static_cast<float>(i & 7) * 0.0008f;
    if (s.c >= 1.0f) {
      s.c -= 1.0f;
      s.a = rng.nextFloat() * static_cast<float>(kSW);
      s.b = rng.nextFloat() * static_cast<float>(kSH);
      s.tint = rng.nextTint();
    }
    s.sx = static_cast<int16_t>(s.a);
    s.sy = static_cast<int16_t>(s.b);
    const float pulse = sinf(s.c * static_cast<float>(M_PI));
    int b = static_cast<int>(255.0f * pulse);
    if (b < 0) b = 0;
    if (b > 255) b = 255;
    s.brightness = static_cast<uint8_t>(b);
  }
}

// ----------------------------------------------------------------------------
// Rain — diagonal streaks falling NE→SW. Faster drops are brighter.
// ----------------------------------------------------------------------------
void initRain(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    seedCommonAppearance(s, rng);
    s.a = rng.nextFloat() * static_cast<float>(kSW);
    s.b = rng.nextFloat() * static_cast<float>(kSH);
    s.c = 1.5f + rng.nextFloat() * 3.0f;
  }
}

void updateRain(Star* stars, int count, StarFieldRng& rng) {
  for (int i = 0; i < count; ++i) {
    Star& s = stars[i];
    s.a -= s.c;
    s.b += s.c * kRainSlope;
    if (s.a < 0.0f || s.b >= kSH) {
      if ((rng.nextU32() & 1u) == 0u) {
        s.a = static_cast<float>(kSW);
        s.b = rng.nextFloat() * static_cast<float>(kSH);
      } else {
        s.a = rng.nextFloat() * static_cast<float>(kSW);
        s.b = 0.0f;
      }
      s.c = 1.5f + rng.nextFloat() * 3.0f;
      s.tint = rng.nextTint();
    }
    s.sx = static_cast<int16_t>(s.a);
    s.sy = static_cast<int16_t>(s.b);
    const int b = 120 + static_cast<int>((s.c - 1.5f) / 3.0f * 135.0f);
    s.brightness = static_cast<uint8_t>((b > 255) ? 255 : b);
  }
}

// ----------------------------------------------------------------------------
// Registry. Order MUST match the integer values of Screensaver::StarMode.
// ----------------------------------------------------------------------------
constexpr StarfieldModeDef kModes[] = {
    {"forward3d", initForward3D, updateForward3D},
    {"parallax",  initParallax,  updateParallax},
    {"vortex",    initVortex,    updateVortex},
    {"twinkle",   initTwinkle,   updateTwinkle},
    {"rain",      initRain,      updateRain},
};

static_assert(sizeof(kModes) / sizeof(kModes[0]) ==
                  static_cast<size_t>(Screensaver::StarMode::kCount),
              "starfield registry must cover every StarMode enum value");

// Defensive fallback definition: keeps the orchestrator safe if it ever
// reads StarMode::kCount (which can't happen via the public API, but the
// reference return needs *some* concrete target if it ever did).
const StarfieldModeDef kFallback = kModes[0];

}  // namespace

const StarfieldModeDef& starfieldModeDef(Screensaver::StarMode mode) {
  const auto idx = static_cast<size_t>(mode);
  if (idx >= sizeof(kModes) / sizeof(kModes[0])) return kFallback;
  return kModes[idx];
}

}  // namespace screensaver
