#pragma once

#include <stdint.h>

// Shared shape for one cell of a tracker pattern. Filled by ModPlayer from
// libxmp's xmp_event, consumed by DisplayManager's tracker pattern view.
// Kept in its own header so DisplayManager.h doesn't transitively pull in
// libxmp + FreeRTOS from ModPlayer.h.
//
// Field meanings mirror xmp_event:
//   note: 0 = empty cell, 1..96 = C-0..B-7, 254 = key-off, 255 = release.
//   ins:  1-based instrument number, 0 = no change.
//   vol:  volume column (format-specific scale; render as 2-digit hex when nonzero).
//   fxt:  effect type byte.
//   fxp:  effect parameter byte.
struct ModulePatternCell {
  uint8_t note = 0;
  uint8_t ins = 0;
  uint8_t vol = 0;
  uint8_t fxt = 0;
  uint8_t fxp = 0;
};
