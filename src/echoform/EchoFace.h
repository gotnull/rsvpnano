#pragma once

#include <stdint.h>

// The rusty-nail face - C++ port of crates/fcecho/src/face.rs: the baked
// canonical 2.5D head surface (depth, normals, formation order), the pose
// transform with depth parallax, the orbiting three-light rig, exposure,
// and expression-aware sampling. Float where the original used Q16.16;
// constants are the original's values.
namespace echoform {

// Face-space geometry: 64x88 cells, drawn at field offset (32, 4).
constexpr int kMapW = 64;
constexpr int kMapH = 88;
constexpr int kMapOffX = 32;
constexpr int kMapOffY = 4;
constexpr int kFaceCells = kMapW * kMapH;

// Wave baseline rows (face.rs): idle mid-field; a calm band beneath the
// chin once the face is coherent.
constexpr int kBaseYIdle = 64;
constexpr int kBaseYFace = 87;

struct SurfaceCell {
  uint8_t depth;  // 0 = off-head; 255 = nearest (nose tip)
  int8_t nx;
  int8_t ny;
  uint8_t nz;
  uint8_t formation;  // low forms first
};

SurfaceCell faceCell(int mx, int my);

// Expression pose, all 0..1 (or -1..1 for tilts/gaze), eased upstream.
struct ExpressionPose {
  float eyeOpen = 0.5f;
  float browRaise = 0.0f;
  float browTilt = 0.0f;
  float mouthOpen = 0.0f;
  float mouthWidth = 0.5f;
  float mouthCurve = 0.0f;
  float jawDrop = 0.0f;
  float gazeX = 0.0f;
  float gazeY = 0.0f;
};

// Restrained 3D head pose (1.0 = ~7 degrees of yaw).
struct HeadPose {
  float yaw = 0.0f;
  float pitch = 0.0f;
  float roll = 0.0f;
  float shiftX = 0.0f;    // lateral drift, pixels
  uint16_t lightPhase = 0;  // key-light orbit phase
};

// Pose-derived per-frame constants.
struct PoseTransform {
  float cosYaw, sinYaw, cosPitch, sinPitch, roll, shiftX;
  int keyX, keyY, keyZ;  // this frame's orbited key light (127-scaled)

  explicit PoseTransform(const HeadPose &pose);
  // Screen position of face-space (u, v) at baked depth d: parallax + roll
  // shear + drift.
  void project(float u, float v, uint8_t d, float poseYaw, float posePitch,
               float &outX, float &outY) const;
  void rotateNormal(int8_t nx, int8_t ny, uint8_t nz, int &rx, int &ry,
                    int &rz) const;
};

// Exposure-shaded intensity (0..255) of one rotated normal under the rig.
int faceShade(int nx, int ny, int nz, int keyX, int keyY, int keyZ);

// Expression-aware sample of cell (mx, my): brow shift, socket scaling,
// jaw slide, mouth cavity, muzzle warmth. Returns exposed 0..255 and the
// baked cell.
int faceSample(int mx, int my, const ExpressionPose &pose,
               const PoseTransform &transform, SurfaceCell &outCell);

// Formation gate 0..256 for a cell at emergence 0..1.
int formationStrength(float emergence, uint8_t formation);

// Head emergence from coherence (render.rs): smoothstep over 0.20..0.80.
float faceEmergence(float coherence);

// Eased wave baseline row for a coherence.
float faceBaseY(float coherence);

// Shared helpers (fixed.rs ports) used by face and state.
float fxSin(uint16_t phase);
float fxCos(uint16_t phase);
float fxSmoothstep(float t);
uint32_t fxWang(uint32_t seed);
float fxValueNoise(uint32_t frame, uint32_t period, uint32_t seed);

}  // namespace echoform
