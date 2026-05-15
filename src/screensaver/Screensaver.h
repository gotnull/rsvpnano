#pragma once

#include <Arduino.h>

#include "screensaver/RotationAnimator.h"

// Demoscene-style 3D screensaver. Orchestrator only — every extension axis
// lives in a sibling file so adding shapes / morph styles / star modes /
// rotation timelines is a one- or two-line edit:
//
//   ShapeRegistry.{h,cpp}    — 15 named shape generators + descriptor table
//   Timeline.{h,cpp}         — keyframe table, EaseCurve, MorphStyle, sampler
//   RotationAnimator.{h,cpp} — segment table + per-frame Euler accumulator
//   StarfieldModes.{h,cpp}   — init/update strategies for the 5 star modes
//
// Behaviour preserved exactly versus the previous monolithic implementation
// — same 216 points, same 15 shapes, same 360/120 hold/morph, same rotation
// timeline, same 150 stars, same palette + tint behaviour. Rendering lives
// in DisplayManager::renderScreensaverFrame; this class only updates state.
class Screensaver {
 public:
  // 6³ = 216 points — bumped from the original 5³ now that the per-frame
  // compose budget had headroom (was ~700 us at 125 dots).
  static constexpr int kPointsPerAxis = 6;
  static constexpr int kPointCount = kPointsPerAxis * kPointsPerAxis * kPointsPerAxis;
  // 150 stars — bumped from 60 for a busier starfield. Star count drives
  // stack sprite buffer (~16 B each) so kept in budget at 150.
  static constexpr int kStarCount = 150;
  static constexpr int kPaletteSize = 15;
  // 15 shape templates: cube, sphere, torus, helix, double-helix, cloud,
  // wave-plane, Lissajous, octahedron-edges, trefoil, mobius, hyperboloid,
  // saddle, pyramid-edges, icosahedron-edges. Adds 6 geometric shapes over
  // the previous 10.
  static constexpr int kShapeCount = 15;
  // Palette of star tint colors (RGB565). Renderer modulates by brightness.
  static constexpr int kStarTintCount = 8;

  // Screen dimensions used to project stars in screen-space; must match the
  // display panel's logical resolution. Stars compute their own sx/sy in
  // tick() so the renderer doesn't switch on mode.
  static constexpr int16_t kScreenWidth = 640;
  static constexpr int16_t kScreenHeight = 172;

  enum class StarMode : uint8_t {
    Forward3D,  // classic perspective starfield — stars stream toward viewer
    Parallax,   // 5-layer horizontal scroll, Starfield-demo style
    Vortex,     // stars spiral outward from center
    Twinkle,    // random field of pulsing sparkles
    Rain,       // diagonal streaks falling NE→SW
    kCount,
  };

  struct Point {
    // Current model-space position (post shape-morph). Written every tick.
    float x, y, z;
    // Post-rotation, post-projection screen-space coords. Renderer reads
    // cx/cy/cz + colorIndex; cz also drives the painter-order sort.
    float cx, cy, cz;
    int colorIndex;
    // Z-derived brightness scale 0..255 — applied as RGB multiplier in the
    // renderer so far points fade to dim instead of staying saturated.
    uint8_t brightness;
  };

  struct Star {
    // Screen-space coords + grayscale brightness — pre-computed by tick()
    // so the renderer doesn't need to know which star mode is active.
    int16_t sx;       // 0..kScreenWidth-1
    int16_t sy;       // 0..kScreenHeight-1
    uint8_t brightness;
    // Tint index into kStarTints[] — renderer modulates this by brightness.
    uint8_t tint;
    // 0 = 1×1 pixel, 1 = small disc (3 px), 2 = bigger disc (5 px). Lets the
    // brightest stars feel punchy without the dim ones blooming.
    uint8_t size;
    // Animation state, interpreted per current starMode_:
    //   Forward3D: a/b/c = normalised 3D coords  (-1..1, -1..1, 0..1)
    //   Parallax:  a = screen X (float),  b = screen Y, c = layer (0..1)
    //   Vortex:    a = angle (rad), b = radius (0..1.5), c = angular speed
    //   Twinkle:   a = screen X,    b = screen Y,        c = phase (0..1)
    //   Rain:      a = screen X,    b = screen Y,        c = fall speed
    float a, b, c;
  };

  void begin(uint32_t seed = 0);
  // Advances one animation frame; call once per draw at ~60 fps.
  void tick();
  // Builds drawOrder() back-to-front along cz (largest first) so painter's
  // algorithm in the renderer produces correct depth. Sorts indices, not
  // Point structs — points_ stays put so we don't move ~52 bytes per swap.
  void sortPoints();

  const Point *points() const { return points_; }
  size_t pointCount() const { return kPointCount; }
  const uint16_t *drawOrder() const { return drawOrder_; }
  const Star *stars() const { return stars_; }
  size_t starCount() const { return kStarCount; }
  // RGB565 Pico-8 palette (no black). Index by colorIndex % kPaletteSize.
  static const uint16_t *palette();
  // RGB565 star tint palette. Index by Star::tint % kStarTintCount.
  static const uint16_t *starTints();

 private:
  void initShapes();
  void initStarsForMode();
  void updateStars();
  void rerollStarMode();
  void updateAngles();
  void buildRotationMatrix(float &m00, float &m01, float &m02,
                           float &m10, float &m11, float &m12,
                           float &m20, float &m21, float &m22) const;
  // Resolves the current model-space position for point i, applying linear
  // morph between two consecutive shapes if we're inside a transition.
  void shapeAt(int i, float &x, float &y, float &z) const;
  // Computes which shape index we're currently dwelling on (0..kShapeCount-1)
  // — used to detect shape transitions for the star-mode reroll.
  uint8_t currentShapeIndex() const;

  // xorshift32 — keeps the screensaver off the global rand()/srand() so
  // re-entry is deterministic from the seed and we don't stomp anyone else's
  // PRNG state.
  uint32_t nextRandU32();
  float nextRandFloat();

  // 15 shape templates × 216 points × (x,y,z) ≈ 38 KB. Stored as float for
  // code simplicity; values are in roughly ±1.5 model units. Populated by
  // the shape registry in begin(); never resized at runtime.
  float shapes_[kShapeCount][kPointCount][3];

  Point points_[kPointCount];
  uint16_t drawOrder_[kPointCount];
  Star stars_[kStarCount];

  uint32_t prng_ = 0x12345678u;  // re-seeded in begin()
  uint32_t frameCounter_ = 0;

  // Rotation state — segment cursor, accumulated angles, and matrix build
  // all live inside the animator. See RotationAnimator.h.
  screensaver::RotationAnimator rotation_;

  // Subtle Y-bias bob (model-space units). Animated each tick.
  float yBias_ = 0.0f;

  // Stars state — mode is re-rolled each time the shape index advances.
  StarMode starMode_ = StarMode::Forward3D;
  uint8_t lastShapeIdx_ = 0xFF;
};
