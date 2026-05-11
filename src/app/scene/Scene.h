#pragma once

#include <Arduino.h>

#include "input/TouchHandler.h"

struct SceneContext;

// Lifecycle contract for every migrated scene:
//
//   enter()    one-time setup when this scene becomes active. May call
//              renderers but must not block on I/O or call delay().
//   tick()     called by the SceneManager once per UI frame. Advances
//              animation state and pushes frames. Must not block —
//              long-running work belongs on a background task.
//   exit()     one-time teardown. Posts cleanup to background tasks
//              (e.g. ModPlayer's audio task) rather than running it
//              synchronously here.
//   onTouch()  event-driven touch dispatch. Returns true if the event
//              was consumed; false lets the legacy touch handler take
//              over for any state that hasn't been migrated yet.
//
// Scenes are owned by the App (as concrete members) and registered with
// the SceneManager at boot. They live for the entire process; enter/exit
// are NOT destruction events.
class Scene {
 public:
  virtual ~Scene() = default;
  virtual const char* name() const = 0;
  virtual void enter(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual void tick(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual void exit(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual bool onTouch(SceneContext& ctx, const TouchEvent& event,
                       uint32_t nowMs) = 0;
};
