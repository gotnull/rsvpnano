#pragma once

#include <cstddef>

#include "app/scene/SceneId.h"

class Scene;
struct SceneContext;
struct TouchEvent;

// Central scene dispatcher. Owns the active SceneId and is the only
// thing that calls Scene::enter() / Scene::exit() in the project — that
// guarantees exit fires exactly once per scene activation, and never
// concurrently with a tick or onTouch.
//
// The SceneManager itself doesn't hold subsystem references; it routes
// the SceneContext supplied at begin() to each scene call. That keeps
// the manager small and testable.
class SceneManager {
 public:
  // Registers a Scene implementation for the given id. Lifetime of the
  // Scene is caller-managed (App holds them as concrete members).
  void registerScene(SceneId id, Scene* scene);

  // Activates SceneId::Legacy as the starting scene and stashes the
  // context for later dispatch. Must be called once at boot AFTER the
  // subsystems referenced in the context are constructed.
  void begin(SceneContext& ctx, uint32_t nowMs);

  // Schedules a transition. The exit/enter pair runs in
  // processPendingTransition() — never mid-handler. tick() and
  // dispatchTouch() both flush pending transitions before returning.
  void requestTransition(SceneId next, const char* reason, uint32_t nowMs);

  // Advances the active scene + flushes any pending transition.
  void tick(uint32_t nowMs);

  // Routes a touch event to the active scene. Returns true if the scene
  // consumed it; false to fall through to the legacy handler in App.
  // Legacy scene never consumes — touches always reach App in that case.
  bool dispatchTouch(const TouchEvent& event, uint32_t nowMs);

  SceneId active() const { return active_; }

 private:
  Scene* sceneFor(SceneId id) const;
  void doTransition(SceneId next, const char* reason, uint32_t nowMs);
  void processPendingTransition(uint32_t nowMs);

  // Sized to comfortably cover the planned scene set without overflowing
  // — bump alongside SceneId additions if you exceed it.
  static constexpr size_t kRegistrySize = 8;
  Scene* registry_[kRegistrySize] = {nullptr};

  SceneId active_ = SceneId::None;
  SceneId pendingNext_ = SceneId::None;
  const char* pendingReason_ = "";
  bool pendingTransition_ = false;

  SceneContext* ctx_ = nullptr;
};
