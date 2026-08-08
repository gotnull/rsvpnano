#pragma once

#include <stdint.h>

#include "echoform/EchoFace.h"
#include "echoform/EchoParticles.h"
#include "echoform/EchoWave.h"

// The rusty-nail compose pass (crates/fcecho/src/render.rs), non-music
// no-face path: phosphor glow decay + draw, the ranked wave ribbon with
// column accumulation and neighbour connection, then the particles with
// their motion-tail pixels. Composes into a 128x128 index field using the
// monochrome ramp {0, 5, 6, 7}; the display layer maps ramp levels to
// panel colours and scales to the panel.
namespace echoform {

constexpr int kFieldW = 128;
constexpr int kFieldH = 128;
// face::base_y at coherence 0.
constexpr float kFieldBaseY = 64.0f;

// Face inputs for the lit-head pass (render.rs shade_head). prevE is the
// caller-owned kFaceCells temporal-smoothing history.
struct FaceFrame {
  const ExpressionPose *pose;
  const HeadPose *head;
  float coherence;
  uint8_t *prevE;
};

// fb and glow are kFieldW * kFieldH byte buffers owned by the caller.
// baseY/coherence move and squeeze the ribbon as the face forms (render.rs
// compose): squeeze to an eighth at full emergence, hard clip above row 85
// once the head owns the frame.
void composeField(uint8_t *fb, uint8_t *glow, const EchoWave &wave,
                  const EchoParticles &particles, float thicknessBoost,
                  float baseY, float coherence);

// The face composes into its OWN kFieldW x kFieldH field so the display can
// blit it with square pixels (the shared wave field is stretched 5x1 for
// the full-width ribbon; a face must never be).
void composeFaceField(uint8_t *faceFb, const FaceFrame &face);

}  // namespace echoform
