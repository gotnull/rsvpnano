#pragma once

#include <stdint.h>

// Compact event payload. The two uint32 slots are deliberately generic —
// embedded firmware doesn't need rich variant types and we want this to
// stay allocation-free. Document the meaning per-event-type next to the
// enum below.
enum class EventType : uint8_t {
  None = 0,
  TouchTap,                 // a = x, b = y
  SceneChanged,             // a = previous SceneId, b = next SceneId
  ScreensaverEntered,
  ScreensaverExited,
  ModCleanupStarted,
  ModCleanupFinished,
  OtaAttempt,               // a = attempt number (1..3), b = HTTP code
  OtaResult,                // a = success(1)/fail(0), b = HTTP code
};

struct Event {
  EventType type = EventType::None;
  uint32_t a = 0;
  uint32_t b = 0;
  uint32_t timestampMs = 0;
};
