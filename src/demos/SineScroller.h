#pragma once

#include <Arduino.h>

// Big-text sine-scroller in the demoscene tradition. Greetz string scrolls
// right-to-left, each glyph wave-bouncing on a vertical sine. Backdrop is a
// dim copper gradient so the text reads against motion.
//
// The actual glyphs are drawn by DisplayManager::renderSineScrollerFrame using
// the existing serif glyph helpers — this class only owns scroll/phase state.
class SineScroller {
 public:
  void begin(uint32_t seedMs, const char *message = nullptr);
  void tick(uint32_t nowMs);

  const char *message() const { return message_; }
  // Pixels of horizontal offset since the message head re-entered the right
  // edge. Increases monotonically; wraps when whole message has scrolled past.
  float scrollPx() const { return scrollPx_; }
  float wavePhase() const { return wavePhase_; }

 private:
  // Default greetz — keep short-ish so the loop has visible cadence.
  static constexpr const char *kDefaultMessage =
      "  RSVP NANO PRESENTS  ***  GREETINGS TO ALL DEMOSCENERS  ***  "
      "FUTURE CREW  ***  THE BLACK LOTUS  ***  PSYCHEDELIC HACKERS  "
      "***  THIS IS WHAT 60 FPS ON A 240 MHZ CHIP LOOKS LIKE  ***  ";

  const char *message_ = kDefaultMessage;
  float scrollPx_ = 0.0f;
  float wavePhase_ = 0.0f;
  uint32_t lastMs_ = 0;
};
