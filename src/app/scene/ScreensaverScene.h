#pragma once

#include "app/scene/Scene.h"

// First migrated scene — the demoscene-style dots/shapes/stars saver.
//
// What this scene owns:
//   - calling Screensaver::begin() exactly once per activation
//   - 60 fps frame cap + Display::renderScreensaverFrame()
//   - touch-to-dismiss → requestExit("touch")
//   - publishing ScreensaverEntered / ScreensaverExited on the event bus
//
// What it deliberately does NOT own (yet):
//   - idle-detection that triggers screensaver entry — App still drives
//     that because it needs cross-state visibility (Menu, Paused, …)
//     that hasn't been migrated. App calls SceneManager.requestTransition
//     when its idle timer fires.
//   - AppState bookkeeping. App keeps AppState::Screensaver as the
//     coarse marker until the rest of the state machine moves over.
//   - audio cleanup. ModPlayer's task does that on core 0; the scene
//     publishes ModCleanupStarted/ModCleanupFinished so App can observe
//     without polling.
class ScreensaverScene : public Scene {
 public:
  const char* name() const override { return "Screensaver"; }
  void enter(SceneContext& ctx, uint32_t nowMs) override;
  void tick(SceneContext& ctx, uint32_t nowMs) override;
  void exit(SceneContext& ctx, uint32_t nowMs) override;
  bool onTouch(SceneContext& ctx, const TouchEvent& event,
               uint32_t nowMs) override;

 private:
  // The SceneManager guarantees a single active instance, but we also
  // gate tick/render on this flag so a stale tick (between exit() and
  // the next requestTransition flush) can't draw a frame after we've
  // already left.
  bool active_ = false;
  uint32_t lastRenderMs_ = 0;
};
