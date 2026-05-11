#include "app/scene/SceneManager.h"

#include <Arduino.h>

#include "app/event/EventBus.h"
#include "app/event/Events.h"
#include "app/scene/Scene.h"
#include "app/scene/SceneContext.h"
#include "input/TouchHandler.h"

void SceneManager::registerScene(SceneId id, Scene* scene) {
  const size_t idx = static_cast<size_t>(id);
  if (idx >= kRegistrySize) {
    Serial.printf("[scene] register out-of-range id=%u\n",
                  static_cast<unsigned>(idx));
    return;
  }
  registry_[idx] = scene;
}

Scene* SceneManager::sceneFor(SceneId id) const {
  const size_t idx = static_cast<size_t>(id);
  if (idx >= kRegistrySize) return nullptr;
  return registry_[idx];
}

void SceneManager::begin(SceneContext& ctx, uint32_t nowMs) {
  ctx_ = &ctx;
  // Start in the legacy catch-all so unmigrated states keep behaving
  // exactly as before this refactor landed.
  doTransition(SceneId::Legacy, "boot", nowMs);
}

void SceneManager::requestTransition(SceneId next, const char* reason,
                                     uint32_t nowMs) {
  (void)nowMs;
  pendingNext_ = next;
  pendingReason_ = reason ? reason : "request";
  pendingTransition_ = true;
}

void SceneManager::processPendingTransition(uint32_t nowMs) {
  if (!pendingTransition_) return;
  const SceneId next = pendingNext_;
  const char* reason = pendingReason_;
  pendingTransition_ = false;
  pendingNext_ = SceneId::None;
  pendingReason_ = "";
  doTransition(next, reason, nowMs);
}

void SceneManager::doTransition(SceneId next, const char* reason,
                                 uint32_t nowMs) {
  if (next == active_) return;
  Scene* prev = sceneFor(active_);
  const SceneId previous = active_;
  if (prev) {
    Serial.printf("[scene] exit %s\n", prev->name());
    prev->exit(*ctx_, nowMs);
  }
  Serial.printf("[scene] transition from=%s to=%s reason=%s\n",
                sceneName(previous), sceneName(next),
                reason ? reason : "?");
  active_ = next;
  Scene* nxt = sceneFor(next);
  if (nxt) {
    Serial.printf("[scene] enter %s\n", nxt->name());
    nxt->enter(*ctx_, nowMs);
  }
  if (ctx_) {
    Event ev{};
    ev.type = EventType::SceneChanged;
    ev.a = static_cast<uint32_t>(previous);
    ev.b = static_cast<uint32_t>(next);
    ev.timestampMs = nowMs;
    ctx_->events.publish(ev);
  }
}

void SceneManager::tick(uint32_t nowMs) {
  if (!ctx_) return;
  // Apply any pending transition from the previous tick/onTouch before
  // ticking — that way an active scene always sees a fresh frame after
  // its enter(), never a tick on a half-installed state.
  processPendingTransition(nowMs);
  Scene* sc = sceneFor(active_);
  if (!sc) return;
  ctx_->requestedExit = false;
  ctx_->requestedReason = "";
  sc->tick(*ctx_, nowMs);
  if (ctx_->requestedExit) {
    requestTransition(SceneId::Legacy, ctx_->requestedReason, nowMs);
    processPendingTransition(nowMs);
  }
}

bool SceneManager::dispatchTouch(const TouchEvent& event, uint32_t nowMs) {
  if (!ctx_) return false;
  // Legacy scene never consumes touches — let the existing App handler
  // process them while we migrate the remaining states.
  if (active_ == SceneId::None || active_ == SceneId::Legacy) return false;
  Scene* sc = sceneFor(active_);
  if (!sc) return false;
  ctx_->requestedExit = false;
  ctx_->requestedReason = "";
  const bool consumed = sc->onTouch(*ctx_, event, nowMs);
  if (ctx_->requestedExit) {
    requestTransition(SceneId::Legacy, ctx_->requestedReason, nowMs);
    processPendingTransition(nowMs);
  }
  return consumed;
}
