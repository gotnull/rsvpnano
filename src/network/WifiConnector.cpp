#include "network/WifiConnector.h"

#include <WiFi.h>

namespace WifiConnector {

bool connect(const std::vector<OtaManager::Network> &networks, uint32_t perTimeoutMs,
             const char *tag, StatusFn onStatus, void *ctx) {
  if (networks.empty()) {
    Serial.printf("[%s] WifiConnector: no networks configured\n", tag ? tag : "wifi");
    return false;
  }
  WiFi.mode(WIFI_STA);
  for (size_t i = 0; i < networks.size(); ++i) {
    const auto &net = networks[i];
    Serial.printf("[%s] try %u/%u: %s\n", tag ? tag : "wifi",
                  static_cast<unsigned>(i + 1), static_cast<unsigned>(networks.size()),
                  net.ssid.c_str());
    if (onStatus) onStatus(ctx, net.ssid, i + 1, networks.size(), false);
    WiFi.disconnect(true);
    delay(50);
    WiFi.begin(net.ssid.c_str(), net.password.c_str());
    const uint32_t deadline = millis() + perTimeoutMs;
    while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
      delay(150);
      yield();
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("[%s] connected: %s, IP=%s RSSI=%d\n", tag ? tag : "wifi",
                    net.ssid.c_str(), WiFi.localIP().toString().c_str(), WiFi.RSSI());
      if (onStatus) onStatus(ctx, net.ssid, i + 1, networks.size(), true);
      return true;
    }
    Serial.printf("[%s] %s timed out\n", tag ? tag : "wifi", net.ssid.c_str());
  }
  WiFi.disconnect(true);
  return false;
}

}  // namespace WifiConnector
