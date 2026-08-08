#include "echoform/EchoRender.h"

#include <math.h>
#include <string.h>

namespace echoform {

namespace {

constexpr uint8_t kRamp[4] = {0, 5, 6, 7};
constexpr int kEchoClipBottom2 = 126;

uint8_t shade(int brightness) {
  if (brightness <= 39) return kRamp[0];
  if (brightness <= 109) return kRamp[1];
  if (brightness <= 189) return kRamp[2];
  return kRamp[3];
}

uint8_t rampRank(uint8_t index) {
  switch (index) {
    case 5: return 1;
    case 6: return 2;
    case 7: return 3;
    default: return 0;
  }
}

void putRanked(uint8_t *fb, int x, int y, uint8_t rankIndex, uint8_t out) {
  if (x < 0 || x >= kFieldW || y < 0 || y >= kFieldH) return;
  uint8_t &p = fb[y * kFieldW + x];
  if (rampRank(rankIndex) > rampRank(p)) {
    p = out;
  }
}

void raise(uint8_t *col, int y, uint8_t index) {
  if (y < 0 || y >= kFieldH) return;
  if (rampRank(index) > rampRank(col[y])) {
    col[y] = index;
  }
}

}  // namespace

namespace {

constexpr int kBayer4[4][4] = {
    {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

void putMax(uint8_t *fb, int x, int y, uint8_t index) {
  putRanked(fb, x, y, index, index);
}

// The lit head pass (render.rs shade_head): sample -> formation gate ->
// one-pole temporal smooth -> screen-stable Bayer quantise -> parallax
// span write, plus the eye catchlights.
void shadeHead(uint8_t *fb, const FaceFrame &face) {
  const float em = faceEmergence(face.coherence);
  const PoseTransform transform(*face.head);
  const int ditherAx = static_cast<int>(floorf(face.head->shiftX));
  if (em <= 0.0f) {
    for (int i = 0; i < kFaceCells; ++i) {
      face.prevE[i] -= face.prevE[i] >> 2;
    }
    return;
  }
  for (int my = 0; my < kMapH; ++my) {
    bool havePrev = false;
    int prevSx = 0;
    for (int mx = 0; mx < kMapW; ++mx) {
      const int at = my * kMapW + mx;
      SurfaceCell cellOut;
      const int raw = faceSample(mx, my, *face.pose, transform, cellOut);
      if (cellOut.depth == 0) {
        face.prevE[at] -= face.prevE[at] >> 2;
        havePrev = false;
        continue;
      }
      const int gate = formationStrength(em, cellOut.formation);
      const int target = (raw * gate) >> 8;
      const int smoothed = (face.prevE[at] * 3 + target) >> 2;
      face.prevE[at] = static_cast<uint8_t>(smoothed);
      float pxF, pyF;
      transform.project(static_cast<float>(mx), static_cast<float>(my),
                        cellOut.depth, face.head->yaw, face.head->pitch, pxF,
                        pyF);
      const int sx = static_cast<int>(floorf(pxF));
      const int sy = static_cast<int>(floorf(pyF));
      if (smoothed < 6) {
        prevSx = sx;
        havePrev = true;
        continue;
      }
      const int v = smoothed * 784;
      int bin = v >> 16;
      if (bin > 3) bin = 3;
      const int frac = (v >> 8) & 255;
      const int t = kBayer4[sy & 3][(sx - ditherAx) & 3] * 16 + 8;
      if (bin < 3 && frac > t) ++bin;
      if (bin == 0) {
        prevSx = sx;
        havePrev = true;
        continue;
      }
      const uint8_t index = kRamp[bin];
      int x0 = sx;
      if (havePrev && prevSx + 1 < sx) x0 = prevSx + 1;
      for (int px = x0; px <= sx; ++px) {
        putMax(fb, px, sy, index);
      }
      prevSx = sx;
      havePrev = true;
    }
  }

  // Eye catchlights: one white pixel per eye on the light side, absent
  // while the eyes are shut and until the face is substantially formed.
  if (em > 0.60f && face.pose->eyeOpen > 0.30f) {
    int lightDx = transform.keyX / 48;
    if (lightDx < -2) lightDx = -2;
    if (lightDx > 0) lightDx = 0;
    const int gazeDx = static_cast<int>(floorf(face.pose->gazeX * 2.0f));
    const int eyes[2] = {20, 43};
    for (int e = 0; e < 2; ++e) {
      const SurfaceCell c = faceCell(eyes[e], 39);
      if (c.depth == 0) continue;
      float pxF, pyF;
      transform.project(static_cast<float>(eyes[e]), 39.0f, c.depth,
                        face.head->yaw, face.head->pitch, pxF, pyF);
      putMax(fb, static_cast<int>(floorf(pxF)) + lightDx + gazeDx,
             static_cast<int>(floorf(pyF)), kRamp[3]);
    }
  }
}

}  // namespace

void composeFaceField(uint8_t *faceFb, const FaceFrame &face) {
  memset(faceFb, 0, kFieldW * kFieldH);
  if (face.pose != nullptr && face.head != nullptr && face.prevE != nullptr) {
    shadeHead(faceFb, face);
  }
}

void composeField(uint8_t *fb, uint8_t *glow, const EchoWave &wave,
                  const EchoParticles &particles, float thicknessBoost,
                  float baseY, float coherence) {
  memset(fb, 0, kFieldW * kFieldH);

  // -- the phosphor wake: cool every cell, draw the still-warm ones --
  for (int i = 0; i < kFieldW * kFieldH; ++i) {
    glow[i] = glow[i] > 10 ? static_cast<uint8_t>(glow[i] - 10) : 0;
    uint8_t heat;
    if (glow[i] >= 180) {
      heat = kRamp[2];
    } else if (glow[i] >= 90) {
      heat = kRamp[1];
    } else {
      continue;
    }
    putRanked(fb, i % kFieldW, i / kFieldW, kRamp[1], heat);
  }

  // -- the wave ribbon --
  // As the face forms the band drops beneath the chin AND compresses (to
  // an eighth at full coherence); a hard floor forbids wave pixels above
  // row 85 once the head owns the frame.
  const float em = faceEmergence(coherence);
  const float squeeze = 1.0f - em * (7.0f / 8.0f);
  const int clipTop = em > 0.5f ? 85 : 0;
  const EchoWave::Point *pts = wave.points();
  uint8_t col[kFieldH] = {0};
  auto clampRowDyn = [clipTop](int v) -> int {
    return v < clipTop ? clipTop : (v > kEchoClipBottom2 ? kEchoClipBottom2 : v);
  };
  int prevRow = clampRowDyn(static_cast<int>(baseY + floorf(pts[0].y * squeeze)));
  for (int x = 0; x < static_cast<int>(kWavePoints); ++x) {
    const int row = clampRowDyn(static_cast<int>(baseY + floorf(pts[x].y * squeeze)));
    float e = pts[x].energy * squeeze + thicknessBoost;
    if (e < 0.0f) e = 0.0f;
    if (e > 3.0f) e = 3.0f;
    const int half = static_cast<int>(e);
    int bright = 190 + static_cast<int>(e * 40.0f);
    if (bright > 255) bright = 255;
    const int lo = prevRow <= row ? prevRow : row;
    const int hi = prevRow <= row ? row : prevRow;
    for (int y = lo; y <= hi; ++y) {
      raise(col, y, shade(bright));
    }
    for (int d = 1; d <= half; ++d) {
      if (row - d >= clipTop) raise(col, row - d, shade(150));
      if (row + d <= kEchoClipBottom2) raise(col, row + d, shade(150));
    }
    if (e > 2.0f && row + half + 1 <= kEchoClipBottom2) {
      raise(col, row + half + 1, shade(70));
    }
    int top = lo < row - half ? lo : row - half;
    if (top < 0) top = 0;
    int bot = hi > row + half + 1 ? hi : row + half + 1;
    if (bot > kFieldH - 1) bot = kFieldH - 1;
    for (int y = top; y <= bot; ++y) {
      const uint8_t idx = col[y];
      if (idx == kRamp[0]) continue;
      col[y] = kRamp[0];
      putRanked(fb, x, y, idx, idx);
      glow[y * kFieldW + x] = 255;
    }
    prevRow = row;
  }

  // -- particles (dust + splashes) with the bright-droplet tail pixel --
  const EchoParticles::Particle *pool = particles.pool();
  for (size_t i = 0; i < kParticlePool; ++i) {
    const EchoParticles::Particle &p = pool[i];
    if (!p.alive()) continue;
    const int x = static_cast<int>(floorf(p.x));
    const int y = static_cast<int>(floorf(p.y));
    const uint8_t idx = shade(p.brightness);
    if (idx == kRamp[0]) continue;
    putRanked(fb, x, y, idx, idx);
    if (p.brightness >= 190) {
      const uint8_t tail = shade(p.brightness - 80);
      putRanked(fb, x + 1, y, tail, tail);
    }
  }
}

}  // namespace echoform
