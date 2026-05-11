#pragma once

#include "app/scene/Scene.h"

// Catch-all scene that does nothing — the existing App::update() path
// continues to handle Menu/Paused/Playing/Settings/etc. while we migrate
// scenes incrementally. The SceneManager activates this scene at boot
// and falls back to it whenever a real scene requests exit. When a state
// migrates, its branch gets pulled out of App::update() and into a new
// Scene subclass; until then, LegacyScene is the no-op fence around the
// not-yet-migrated path.
class LegacyScene : public Scene {
 public:
  const char* name() const override { return "Legacy"; }
  void enter(SceneContext& /*ctx*/, uint32_t /*nowMs*/) override {}
  void tick(SceneContext& /*ctx*/, uint32_t /*nowMs*/) override {}
  void exit(SceneContext& /*ctx*/, uint32_t /*nowMs*/) override {}
  bool onTouch(SceneContext& /*ctx*/, const TouchEvent& /*ev*/,
               uint32_t /*nowMs*/) override {
    return false;
  }
};
