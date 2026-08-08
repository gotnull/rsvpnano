#include "echoform/EchoFace.h"

#include <math.h>

#include "echoform/face_head.h"

namespace echoform {

namespace {

constexpr int kCellBytes = 5;
constexpr int kCropX0 = 4;
constexpr int kCropW = 56;

// Light rig (face.rs): key upper-left-forward (orbits), fill front-right,
// rim on the right silhouette; strengths Q8.
constexpr int kKeyX = -57, kKeyY = -70, kKeyZ = 89;
constexpr int kFillX = 57, kFillY = 6, kFillZ = 113;
constexpr int kKeyStrength = 230;
constexpr int kFillStrength = 41;
constexpr int kRimStrength = 97;
constexpr int kAmbientQ8 = 8;
constexpr int kExposeFloorQ8 = 67;
constexpr int kExposeGainQ8 = 371;

constexpr int kYawPhaseMax = 1280;
constexpr float kYawParallaxPx = 4.0f;
constexpr float kPitchParallaxPx = 2.0f;

int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

}  // namespace

float fxSin(uint16_t phase) {
  return sinf(static_cast<float>(phase) * (6.2831853f / 65536.0f));
}

float fxCos(uint16_t phase) {
  return fxSin(static_cast<uint16_t>(phase + 16384));
}

float fxSmoothstep(float t) {
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;
  return t * t * (3.0f - 2.0f * t);
}

uint32_t fxWang(uint32_t seed) {
  seed = (seed ^ 61u) ^ (seed >> 16);
  seed *= 9u;
  seed ^= seed >> 4;
  seed *= 0x27D4EB2Du;
  return seed ^ (seed >> 15);
}

float fxValueNoise(uint32_t frame, uint32_t period, uint32_t seed) {
  const uint32_t k = frame / period;
  const float t = static_cast<float>(frame % period) / period;
  auto level = [seed](uint32_t kk) -> float {
    const uint32_t h = fxWang(kk * 0x9E3779B9u ^ seed);
    return static_cast<float>(h & 0x1FFFF) / 65536.0f - 1.0f;
  };
  const float a = level(k);
  const float b = level(k + 1);
  return a + (b - a) * fxSmoothstep(t);
}

SurfaceCell faceCell(int mx, int my) {
  if (mx < kCropX0 || mx >= kCropX0 + kCropW || my < 0 || my >= kMapH) {
    return SurfaceCell{0, 0, 0, 127, 255};
  }
  const int at = (my * kCropW + (mx - kCropX0)) * kCellBytes;
  return SurfaceCell{kFaceHead[at],
                     static_cast<int8_t>(kFaceHead[at + 1]),
                     static_cast<int8_t>(kFaceHead[at + 2]),
                     kFaceHead[at + 3], kFaceHead[at + 4]};
}

namespace {

// Rotate the base key light: ±22 degree azimuth sweep + ±8 elevation sway
// on a 3x sub-phase (face.rs orbit_key).
void orbitKey(uint16_t phase, int &kx, int &ky, int &kz) {
  const float az = fxSin(phase) * 1600.0f;
  const uint16_t azPhase = static_cast<uint16_t>(static_cast<int>(az));
  const float c = fxCos(azPhase);
  const float s = fxSin(azPhase);
  kx = clampi(static_cast<int>(kKeyX * c + kKeyZ * s), -127, 127);
  kz = clampi(static_cast<int>(kKeyZ * c - kKeyX * s), -127, 127);
  const uint16_t sub = static_cast<uint16_t>(phase * 3 + 9000);
  ky = clampi(kKeyY + static_cast<int>(fxSin(sub) * 16.0f), -127, 127);
}

}  // namespace

PoseTransform::PoseTransform(const HeadPose &pose) {
  const uint16_t yawPhase =
      static_cast<uint16_t>(static_cast<int>(pose.yaw * kYawPhaseMax));
  const uint16_t pitchPhase =
      static_cast<uint16_t>(static_cast<int>(pose.pitch * (kYawPhaseMax / 2)));
  cosYaw = fxCos(yawPhase);
  sinYaw = fxSin(yawPhase);
  cosPitch = fxCos(pitchPhase);
  sinPitch = fxSin(pitchPhase);
  roll = pose.roll;
  shiftX = pose.shiftX;
  orbitKey(pose.lightPhase, keyX, keyY, keyZ);
}

void PoseTransform::project(float u, float v, uint8_t d, float poseYaw,
                            float posePitch, float &outX, float &outY) const {
  const float depth = static_cast<float>(d) / 255.0f;
  const float dx = poseYaw * kYawParallaxPx * depth;
  const float dy = posePitch * kPitchParallaxPx * depth;
  const float cx = u - kMapW / 2.0f;
  const float cy = v - 44.0f;
  const float rx = roll * cy / 8.0f;
  const float ry = -roll * cx / 8.0f;
  outX = kMapOffX + u + dx + rx + shiftX;
  outY = kMapOffY + v + dy + ry;
}

void PoseTransform::rotateNormal(int8_t nxIn, int8_t nyIn, uint8_t nzIn,
                                 int &rxOut, int &ryOut, int &rzOut) const {
  const float nx = nxIn * 2.0f;
  const float ny = nyIn * 2.0f;
  const float nz = nzIn;
  const float rx = nx * cosYaw + nz * sinYaw;
  float rz = nz * cosYaw - nx * sinYaw;
  const float ry = ny * cosPitch - rz * sinPitch;
  rz = rz * cosPitch + ny * sinPitch;
  rxOut = static_cast<int>(rx);
  ryOut = static_cast<int>(ry);
  rzOut = static_cast<int>(rz);
}

int faceShade(int nx, int ny, int nz, int keyX, int keyY, int keyZ) {
  const int dk = nx * keyX + ny * keyY + nz * keyZ;
  const int df = nx * kFillX + ny * kFillY + nz * kFillZ;
  const int rim = ((nx > 0 ? nx : 0) * (255 - nz > 0 ? 255 - nz : 0)) >> 8;
  int i = kAmbientQ8;
  i += ((dk > 0 ? dk : 0) * kKeyStrength) >> 15;
  i += ((df > 0 ? df : 0) * kFillStrength) >> 15;
  i += (rim * kRimStrength) >> 8;
  int e = ((i - kExposeFloorQ8) * kExposeGainQ8) >> 8;
  e = clampi(e, 0, 255);
  e = (e * (141 + ((115 * e) >> 8))) >> 8;
  return clampi(e, 0, 255);
}

int faceSample(int mx, int my, const ExpressionPose &pose,
               const PoseTransform &transform, SurfaceCell &outCell) {
  int sx = mx;
  int sy = my;
  // Brow band shifts with raise (up) and tilt (opposite per side).
  if (my >= 22 && my < 35) {
    const int raiseR = static_cast<int>(floorf(pose.browRaise * 3.0f));
    const int tilt = static_cast<int>(floorf(pose.browTilt * 2.0f));
    sy += raiseR + (mx < kMapW / 2 ? tilt : -tilt);
  }
  // Chin/jaw slides with jaw drop.
  if (my >= 74) {
    sy -= static_cast<int>(floorf(pose.jawDrop * 2.0f));
  }
  const bool inSocket = (my >= 33 && my < 46) &&
                        ((mx >= 11 && mx < 30) || (mx >= 34 && mx < 53));
  if (inSocket) {
    sx -= static_cast<int>(floorf(pose.gazeX * 2.0f));
    sy -= static_cast<int>(floorf(pose.gazeY * 1.0f));
  }

  outCell = faceCell(sx < 0 ? 0 : sx, sy < 0 ? 0 : sy);
  if (outCell.depth == 0) return 0;
  int nx, ny, nz;
  transform.rotateNormal(outCell.nx, outCell.ny, outCell.nz, nx, ny, nz);
  int e = faceShade(nx, ny, nz, transform.keyX, transform.keyY,
                    transform.keyZ);

  // Sockets darken as the eyes close.
  if (inSocket) {
    const float open = 0.30f + pose.eyeOpen * 0.70f;
    e = static_cast<int>(e * open);
  }
  // The mouth cavity: an opening jaw carves darkness under the lip row.
  if (pose.mouthOpen > 0.0f && my >= 69 && my <= 78) {
    const int halfW = 3 + static_cast<int>(pose.mouthWidth * 6.0f);
    const int depthRows = 1 + static_cast<int>(pose.mouthOpen * 5.0f);
    const int dx = mx - kMapW / 2 < 0 ? -(mx - kMapW / 2) : mx - kMapW / 2;
    const int dy = my - 69;
    if (dx < halfW && dy < depthRows) {
      int edge = dx * 256 / (halfW > 1 ? halfW : 1);
      const int edgeY = dy * 256 / (depthRows > 1 ? depthRows : 1);
      if (edgeY > edge) edge = edgeY;
      e = (e * clampi(edge, 0, 256)) >> 9;
    }
  }
  // Speech warms the muzzle a little.
  if (my >= 60 && my < 74 && pose.mouthOpen > 0.0f) {
    e += static_cast<int>(e * pose.mouthOpen * 0.25f);
  }
  return clampi(e, 0, 255);
}

int formationStrength(float emergence, uint8_t formation) {
  // face.rs: Q16 emergence minus formation<<8, /24, clamp 0..256.
  const int d =
      static_cast<int>(emergence * 65536.0f) - (static_cast<int>(formation) << 8);
  return clampi(d / 24, 0, 256);
}

float faceEmergence(float coherence) {
  return fxSmoothstep((coherence - 0.20f) / 0.60f);
}

float faceBaseY(float coherence) {
  const float t = fxSmoothstep(coherence);
  return kBaseYIdle + (kBaseYFace - kBaseYIdle) * t;
}

}  // namespace echoform
