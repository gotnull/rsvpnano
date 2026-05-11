#pragma once

#include <stdint.h>

class DisplayManager;
class Screensaver;
class ModPlayer;
class EventBus;

// Bundle of subsystem references handed to every Scene method, plus a
// per-call output slot that scenes use to request transitions. The
// SceneManager resets the output slot before each invocation so scenes
// can't accidentally observe stale state from a sibling.
//
// We deliberately do NOT pass the whole App in here — that would let
// scenes call into the legacy state machine, which is exactly what this
// refactor is moving away from. If a scene needs something not in the
// context, add it as a dedicated dependency.
struct SceneContext {
  // Explicit ctor so we can mix `T&` members with default-initialized
  // output fields without tripping the pre-C++17 aggregate-init rules
  // that the project's build flags currently use.
  SceneContext(DisplayManager& d, Screensaver& s, ModPlayer& m, EventBus& e)
      : display(d), screensaver(s), modPlayer(m), events(e) {}

  DisplayManager& display;
  Screensaver& screensaver;
  ModPlayer& modPlayer;
  EventBus& events;

  // Output — set by a scene method to request a transition back to
  // SceneId::Legacy. The SceneManager picks it up after the method
  // returns and runs the transition in a well-defined place.
  bool requestedExit = false;
  const char* requestedReason = "";

  void requestExit(const char* reason) {
    requestedExit = true;
    requestedReason = reason ? reason : "scene-request";
  }
};
