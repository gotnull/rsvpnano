#pragma once

#include <Arduino.h>

// Demoscene-style 3D screensaver — heavily upgraded with Vectorball-style
// shape morph + multi-segment Euler-angle rotation + Z-brightness depth
// cue. Cycles through 10 hand-built point clouds (cube, sphere, torus,
// helix, double-helix, random cloud, wave-plane, Lissajous, tight-spiral,
// trefoil) with linear-interp morphs between them, all rotating through a
// Tait-Bryan ZYX matrix whose per-axis rates change every few seconds.
//
// Stars on top alternate between two modes — re-rolled each shape
// transition: 3D-forward (the classic "stars come at you") and Parallax
// (multi-layer horizontal scroll, ported from the Starfield demo).
//
// State + animation lives here; rendering happens in
// DisplayManager::renderScreensaverFrame.
class Screensaver {
 public:
  // 6³ = 216 points — bumped from the original 5³ now that the per-frame
  // compose budget had headroom (was ~700 us at 125 dots).
  static constexpr int kPointsPerAxis = 6;
  static constexpr int kPointCount = kPointsPerAxis * kPointsPerAxis * kPointsPerAxis;
  static constexpr int kStarCount = 60;
  static constexpr int kPaletteSize = 15;
  static constexpr int kShapeCount = 10;

  // Screen dimensions used to project stars in screen-space; must match the
  // display panel's logical resolution. Stars compute their own sx/sy in
  // tick() so the renderer doesn't switch on mode.
  static constexpr int16_t kScreenWidth = 640;
  static constexpr int16_t kScreenHeight = 172;

  enum class StarMode : uint8_t {
    Forward3D,  // classic perspective starfield — stars stream toward viewer
    Parallax,   // 3-layer horizontal scroll, Starfield-demo style
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
    // Animation state, interpreted per current starMode_:
    //   Forward3D: a/b/c = normalised 3D coords  (-1..1, -1..1, 0..1)
    //   Parallax:  a = screen X (float),  b = screen Y, c = layer (0..1)
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

  // 10 shape templates × 216 points × (x,y,z) ≈ 26 KB. Stored as float for
  // code simplicity; values are in roughly ±1.5 model units.
  float shapes_[kShapeCount][kPointCount][3];

  Point points_[kPointCount];
  uint16_t drawOrder_[kPointCount];
  Star stars_[kStarCount];

  uint32_t prng_ = 0x12345678u;  // re-seeded in begin()
  uint32_t frameCounter_ = 0;

  // Multi-segment rotation DSL state — varies which axis dominates over
  // time so the cluster doesn't feel like a single uniform spin.
  uint8_t rotSegIdx_ = 0;
  uint16_t rotSegFrame_ = 0;
  float angleX_ = 0.0f, angleY_ = 0.0f, angleZ_ = 0.0f;

  // Subtle Y-bias bob (model-space units). Animated each tick.
  float yBias_ = 0.0f;

  // Stars state — mode is re-rolled each time the shape index advances.
  StarMode starMode_ = StarMode::Forward3D;
  uint8_t lastShapeIdx_ = 0xFF;
};
