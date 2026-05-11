#include "app/scene/ScreensaverScene.h"

#include <Arduino.h>

#include "app/event/EventBus.h"
#include "app/event/Events.h"
#include "app/scene/SceneContext.h"
#include "display/DisplayManager.h"
#include "input/TouchHandler.h"
#include "screensaver/Screensaver.h"

namespace {
// 60 fps cap. renderScreensaverFrame() does the SPI flush (~12 ms after
// the native-stripe rewrite); throttling at the scene level keeps the
// tick loop predictable and gives time back for other work.
constexpr uint32_t kFrameIntervalMs = 16;
}  // namespace

void ScreensaverScene::enter(SceneContext& ctx, uint32_t nowMs) {
  active_ = true;
  lastRenderMs_ = 0;
  ctx.screensaver.begin(nowMs);
  Event ev{};
  ev.type = EventType::ScreensaverEntered;
  ev.timestampMs = nowMs;
  ctx.events.publish(ev);
}

void ScreensaverScene::tick(SceneContext& ctx, uint32_t nowMs) {
  // Re-check active_ rather than trusting the SceneManager — guards
  // against any path that might tick a freshly-exited scene.
  if (!active_) return;
  if (nowMs - lastRenderMs_ < kFrameIntervalMs) return;
  lastRenderMs_ = nowMs;
  ctx.display.renderScreensaverFrame(ctx.screensaver);
}

void ScreensaverScene::exit(SceneContext& ctx, uint32_t nowMs) {
  active_ = false;
  Event ev{};
  ev.type = EventType::ScreensaverExited;
  ev.timestampMs = nowMs;
  ctx.events.publish(ev);
  // Cleanup of any background subsystem (the MOD player is the obvious
  // one) is *not* run synchronously here. ModPlayer.stop() is already
  // async — it signals the core-0 audio task to free its context. The
  // UI thread returns immediately and observes ModCleanupFinished via
  // the event bus when the work is done.
}

bool ScreensaverScene::onTouch(SceneContext& ctx, const TouchEvent& event,
                                 uint32_t nowMs) {
  (void)nowMs;
  if (!active_) return false;
  // Any touch dismisses. We don't care about End vs Start vs Move — the
  // legacy `dismissTouchPending_` machinery in App.cpp still suppresses
  // the rest of the touch session so the next fresh tap is a clean menu
  // interaction.
  ctx.requestExit(event.phase == TouchPhase::End ? "touch-end" : "touch");
  return true;
}
