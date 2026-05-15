#include "screensaver/Screensaver.h"

#include <algorithm>
#include <math.h>

#include "screensaver/RotationAnimator.h"
#include "screensaver/ShapeRegistry.h"
#include "screensaver/StarfieldModes.h"
#include "screensaver/Timeline.h"

namespace {

// Camera Z offset added after rotation. The cluster sits closer to the
// viewer than the original 4.0 — invCz is bigger so the projected radius
// fills more of the panel.
constexpr float kCameraZ = 3.0f;
// Z-brightness depth cue range. cz=kBrightNear → full bright (255), cz=kBrightFar
// → kMinBright. Brackets the cluster's typical Z range under kCameraZ=3.
constexpr float kBrightNear = 1.5f;
constexpr float kBrightFar = 4.5f;
constexpr int kMinBright = 90;

// Pico-8 palette in RGB565 (black removed — that's the background).
constexpr uint16_t kPaletteColors[Screensaver::kPaletteSize] = {
    0x18C3, 0x7C09, 0x016A, 0xA9A4, 0x52EA, 0xC618, 0xFFFB, 0xF810,
    0xFD60, 0xFFE0, 0x07E6, 0x2B7F, 0x8418, 0xFE36, 0xFF55,
};

// Star tint palette in RGB565 — applied as a chroma over each star's grayscale
// brightness. Mixed warm + cool + a few neutral whites so the field reads as
// varied without any single color dominating.
constexpr uint16_t kStarTintColors[Screensaver::kStarTintCount] = {
    0xFFFF,  // pure white
    0xFFFE,  // bright white (slight green bias)
    0xC79F,  // pale blue
    0xFFE0,  // warm yellow
    0xFE36,  // soft pink
    0x7DFF,  // light cyan
    0xFCE0,  // peach
    0xAFFF,  // electric blue
};

// Other tables now live with their owners:
//   - shape generators / model scale   → ShapeRegistry.{h,cpp}
//   - shape hold/morph timeline        → Timeline.{h,cpp}
//   - rotation segment table           → RotationAnimator.{h,cpp}
//   - parallax / vortex / rain tuning  → StarfieldModes.cpp

}  // namespace

const uint16_t *Screensaver::palette() { return kPaletteColors; }
const uint16_t *Screensaver::starTints() { return kStarTintColors; }

uint32_t Screensaver::nextRandU32() {
  uint32_t x = prng_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  prng_ = x;
  return x;
}

float Screensaver::nextRandFloat() {
  return static_cast<float>(nextRandU32()) / static_cast<float>(0xFFFFFFFFu);
}

uint8_t Screensaver::currentShapeIndex() const {
  // Delegate to the timeline so the keyframe table is the single source of
  // truth for "which shape is currently dwelled-on".
  return screensaver::sampleTimeline(frameCounter_).shapeIdx;
}

void Screensaver::begin(uint32_t seed) {
  prng_ = (seed != 0) ? seed : 0x12345678u;
  initShapes();
  // Start in a randomly-chosen star mode so back-to-back screensaver
  // entries don't always look identical. Spans all 5 modes uniformly.
  starMode_ = static_cast<StarMode>(nextRandU32() %
                                    static_cast<uint32_t>(StarMode::kCount));
  initStarsForMode();
  for (int i = 0; i < kPointCount; ++i) {
    drawOrder_[i] = static_cast<uint16_t>(i);
    Point &p = points_[i];
    p.x = shapes_[0][i][0];
    p.y = shapes_[0][i][1];
    p.z = shapes_[0][i][2];
    p.cx = p.cy = 0.0f;
    p.cz = 1.0f;
    p.brightness = 255;
    // Spread the Pico-8 palette across the cluster — bias toward the brighter
    // half (indices 6..14) so the cube shape doesn't read as muddy.
    p.colorIndex = 6 + (i * 7) % 9;
  }
  frameCounter_ = 0;
  rotation_.reset();
  yBias_ = 0.0f;
  lastShapeIdx_ = 0;
}

void Screensaver::initShapes() {
  // Drive the shape-generator registry. Deterministic seed so cloud/random
  // shapes reproduce identically across re-entries — same constant used by
  // the previous in-place implementation, kept for byte-identical output.
  screensaver::ShapeRng rng{0xC0DECAFEu};
  const auto* registry = screensaver::shapeRegistry();
  for (int i = 0; i < screensaver::shapeRegistryCount(); ++i) {
    registry[i].generate(shapes_[i], rng);
  }
}

void Screensaver::initStarsForMode() {
  screensaver::StarFieldRng rng(prng_);
  screensaver::starfieldModeDef(starMode_).init(stars_, kStarCount, rng);
}

void Screensaver::rerollStarMode() {
  // Pick a random new mode different from the current one — guarantees a
  // visible change every shape transition.
  const uint8_t curr = static_cast<uint8_t>(starMode_);
  uint8_t next = curr;
  while (next == curr) {
    next = static_cast<uint8_t>(nextRandU32() %
                                static_cast<uint32_t>(StarMode::kCount));
  }
  starMode_ = static_cast<StarMode>(next);
  initStarsForMode();
}

void Screensaver::updateStars() {
  screensaver::StarFieldRng rng(prng_);
  screensaver::starfieldModeDef(starMode_).update(stars_, kStarCount, rng);
}


void Screensaver::updateAngles() {
  rotation_.advance();
}

void Screensaver::buildRotationMatrix(float &m00, float &m01, float &m02,
                                      float &m10, float &m11, float &m12,
                                      float &m20, float &m21, float &m22) const {
  rotation_.buildMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
}

void Screensaver::shapeAt(int i, float &x, float &y, float &z) const {
  // Delegate to the timeline + morph-style layer. The seed lets future morph
  // styles vary per-point deterministically; MorphStyle::Linear ignores it.
  float out[3];
  screensaver::resolvePoint(frameCounter_, i, shapes_, 0xC0FFEEu, out);
  x = out[0];
  y = out[1];
  z = out[2];
}

void Screensaver::tick() {
  ++frameCounter_;
  // Detect shape transition; reroll star mode each time the held shape
  // index advances. The ~9 s hold means the mode change is occasional, not
  // every frame — and a 50/50 toss inside reroll keeps it interesting.
  const uint8_t shapeIdx = currentShapeIndex();
  if (shapeIdx != lastShapeIdx_) {
    lastShapeIdx_ = shapeIdx;
    rerollStarMode();
  }
  updateAngles();
  updateStars();
  // Subtle Y-bias bob — pulls the cluster gently up/down across the screen
  // so it doesn't feel locked to the centre. ~10 s period at 60 fps.
  yBias_ = 0.25f * sinf(frameCounter_ * 0.01f);

  float m00, m01, m02, m10, m11, m12, m20, m21, m22;
  buildRotationMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);

  for (int i = 0; i < kPointCount; ++i) {
    Point &p = points_[i];
    shapeAt(i, p.x, p.y, p.z);
    const float rx = m00 * p.x + m01 * p.y + m02 * p.z;
    const float ry = m10 * p.x + m11 * p.y + m12 * p.z + yBias_;
    const float rz = m20 * p.x + m21 * p.y + m22 * p.z + kCameraZ;
    p.cx = rx;
    p.cy = ry;
    p.cz = (rz < 0.1f) ? 0.1f : rz;
    // Map cz onto kBrightNear..kBrightFar → 255..kMinBright.
    float t = (p.cz - kBrightNear) / (kBrightFar - kBrightNear);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const int b = 255 - static_cast<int>(t * (255 - kMinBright));
    p.brightness = static_cast<uint8_t>(b);
  }
}

void Screensaver::sortPoints() {
  // Insertion sort over the index array — n=216, mostly already-sorted
  // between frames (rotation is small per tick). Each swap moves 2 bytes
  // instead of ~52 — eliminates the Point struct copy that dominated the
  // previous sort. points_ stays put.
  for (int i = 1; i < kPointCount; ++i) {
    const uint16_t key = drawOrder_[i];
    const float keyCz = points_[key].cz;
    int j = i - 1;
    while (j >= 0 && points_[drawOrder_[j]].cz < keyCz) {
      drawOrder_[j + 1] = drawOrder_[j];
      --j;
    }
    drawOrder_[j + 1] = key;
  }
}
