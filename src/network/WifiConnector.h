#pragma once

#include <Arduino.h>

#include <vector>

#include "network/OtaManager.h"

namespace WifiConnector {

// Status callback fired before each network attempt and after the final
// outcome. `attemptIdx` is 1-based; `total` is the size of the network list.
// `connected` is true only on the success notification.
using StatusFn = void (*)(void *ctx, const String &ssid, size_t attemptIdx, size_t total,
                          bool connected);

// Iterates `networks` calling WiFi.begin on each in turn, polling for connect
// up to `perTimeoutMs`. Returns true on first connect; false if all fail.
// `tag` shows up in serial logs to differentiate which manager called.
bool connect(const std::vector<OtaManager::Network> &networks, uint32_t perTimeoutMs,
             const char *tag, StatusFn onStatus = nullptr, void *ctx = nullptr);

}  // namespace WifiConnector
