#pragma once

#include <Arduino.h>

#include <vector>

#include "network/OtaManager.h"

class NotificationsManager {
 public:
  struct Notification {
    String id;
    String title;
    String body;
    uint32_t ts = 0; // server-side unix seconds
  };

  using StatusCallback = void (*)(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);

  void setStatusCallback(StatusCallback callback, void *context);
  // Configures the network endpoint and (optional) bearer token. Polling is a no-op
  // when ssid/url is empty, so the feature gracefully degrades if wifi.json doesn't
  // include notification config.
  void configure(const std::vector<OtaManager::Network> &networks, const String &url,
                 const String &token);
  bool enabled() const { return enabled_; }
  void setEnabled(bool enabled);

  // Performs one polling cycle: WiFi connect, GET, parse, disconnect.
  // Returns true if at least one new notification arrived.
  bool poll();

  // Caller persists this across reboots so previously-seen notifications
  // aren't re-shown after a power cycle.
  uint32_t lastSeenTs() const { return lastSeenTs_; }
  void setLastSeenTs(uint32_t ts) { lastSeenTs_ = ts; }

  // Drains pending notifications (those returned by the latest poll). Caller
  // takes ownership and is expected to surface them to the user.
  std::vector<Notification> drainPending();

 private:
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);

  std::vector<OtaManager::Network> networks_;
  String url_;
  String token_;
  bool enabled_ = true;
  uint32_t lastSeenTs_ = 0;
  std::vector<Notification> pending_;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
};
