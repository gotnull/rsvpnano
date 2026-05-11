#pragma once

#include <stdint.h>

// Compact scene IDs. Keep in sync with SceneManager::kRegistrySize.
// Adding a new scene: bump the enum, register the instance in App::begin(),
// and migrate the corresponding App::update() branch into the new scene's
// tick/render path.
enum class SceneId : uint8_t {
  None = 0,
  // Catch-all that defers to the legacy App::update() path so the existing
  // menu / reader / settings code keeps running unchanged. Only fully-
  // migrated scenes get their own class.
  Legacy = 1,
  Screensaver = 2,
  // Future scenes (not yet migrated):
  // Menu, Reader, ModPlayer, DemoPlayer, CameraStream
};

inline const char* sceneName(SceneId id) {
  switch (id) {
    case SceneId::None: return "None";
    case SceneId::Legacy: return "Legacy";
    case SceneId::Screensaver: return "Screensaver";
  }
  return "Unknown";
}
