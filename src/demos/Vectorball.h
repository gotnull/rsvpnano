#pragma once

#include <Arduino.h>

// 1:1 port of the Equinox "Vectorball" effect by Keops (Atari ST 1992,
// Java applet 2001). See tools/demoref/equinox/vectorball/PORT_NOTES.md for
// the full reverse-engineering spec — this class implements that spec
// verbatim except where noted.
//
// Pipeline per frame:
//   1. Update accumulating Euler angles (a6, a4, a9) from the 8-segment DSL
//   2. Update Y screen-center bias (a3) for the dive-in/climb-out bob
//   3. Build the Tait-Bryan ZYX rotation matrix
//   4. Compute current shape — held shape, or linear morph between two
//      consecutive shapes per the bf[] threshold timeline
//   5. Rotate every ball through the matrix
//   6. Bucket-sort by Z so the renderer draws back-to-front
//   7. Project to 300×280 screen space and assign sprite brightness 1..6
class Vectorball {
 public:
  static constexpr int kBallCount = 400;       // aw / ab
  static constexpr int kShapeCount = 10;       // 10 hand-coded shape sets
  static constexpr int kFrameWidth = 300;      // SCREEN_W (ag)
  static constexpr int kFrameHeight = 280;     // SCREEN_H (af)
  static constexpr float kCameraZ = -1100.0f;  // a8 (camera Z offset)
  static constexpr float kFocal = 300.0f;      // perspective focal length
  static constexpr float kInitialYBias = 891.0f;  // a3 starts here
  static constexpr int kSpriteSize = 15;       // each ball sprite is 15×15
  static constexpr size_t kFramebufferBytes =
      static_cast<size_t>(kFrameWidth) * kFrameHeight;  // 84 000 bytes

  // Per-ball state — written each frame by tick().
  struct Ball {
    int16_t sx;        // screen x in 300×280 space (sprite top-left)
    int16_t sy;        // screen y
    int8_t brightness; // 1..6 = sprite case index (6=nearest, 1=farthest)
  };

  ~Vectorball();

  void begin(uint32_t seedMs);
  void tick(uint32_t nowMs);

  // Bucket-sort by post-rotation Z so the renderer can iterate
  // drawOrder()[0..N) and paint back→front (painter's algorithm).
  // Per the original a.r() — O(N) bucket sort with linear collision probing.
  void sortBalls();

  // Clear the 300×280 palette-indexed framebuffer and stamp every ball's
  // sprite (brightness-selected from the 6 hand-encoded sprite tables) in
  // drawOrder. Call after tick() + sortBalls(); DisplayManager then reads
  // framebuffer() for the panel scale + LUT pass.
  void paintFramebuffer();

  const Ball *balls() const { return balls_; }
  size_t ballCount() const { return kBallCount; }
  const uint16_t *drawOrder() const { return drawOrder_; }
  const uint8_t *framebuffer() const { return framebuffer_; }

 private:
  // 10 shape sets, each 400 points × (X,Y,Z) packed as int16. ±400 model
  // space units fits comfortably in int16. Total: 24 KB as a class member.
  int16_t shapes_[kShapeCount][kBallCount][3];

  // Post-rotation Z per ball — used by sortBalls() and the projection step.
  // Stored separately so sortBalls() doesn't need to dereference Ball.
  int16_t rotZ_[kBallCount];

  Ball balls_[kBallCount];
  uint16_t drawOrder_[kBallCount];

  // Animation state.
  float a6_ = 0.0f;       // accumulated Euler X angle
  float a4_ = 0.0f;       // accumulated Euler Y angle
  float a9_ = 0.0f;       // accumulated Euler Z angle
  float yBias_ = kInitialYBias;  // a3 — Y screen-center bias
  uint32_t frameCounter_ = 0;    // ba — drives the shape morph timeline
  uint8_t segIdx_ = 0;     // a0 — current rotation DSL segment (0..7)
  uint16_t segFrame_ = 0;  // a1 — frame within current segment

  // Bucket-sort scratch — 1600 slots of (ballIndex+1) or 0 for empty.
  uint16_t buckets_[1600];

  // 300×280 palette-indexed framebuffer in PSRAM. Lazily allocated in
  // begin() so the App's static construction stays cheap.
  uint8_t *framebuffer_ = nullptr;

  void generateShapes();
  void updateAngles();
  void buildRotationMatrix(float &ar, float &aq, float &ap,
                           float &ao, float &an, float &am,
                           float &al, float &ak, float &aj);
  // Resolves the current shape for ball n, applying linear morph between two
  // shape sets if we're inside a transition window. Outputs in model space.
  void shapeAt(int n, float &x, float &y, float &z) const;
};
