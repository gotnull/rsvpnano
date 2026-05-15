#pragma once

#include <Arduino.h>

#include <atomic>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "network/OtaManager.h"

// Polls a remote notifications endpoint and surfaces fresh items to the UI.
//
// The network round-trip (WiFi connect up to 20 s + HTTPS GET up to 8 s)
// runs on a FreeRTOS worker task pinned to APP_CPU. Main loop never blocks —
// it just calls requestPollAsync() when the 5-min cadence fires and drains
// drainPending() every tick. Honours the "main loop is sacred" rule.
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

  ~NotificationsManager();

  void setStatusCallback(StatusCallback callback, void *context);
  // Configures the network endpoint and (optional) bearer token. Polling is a no-op
  // when ssid/url is empty, so the feature gracefully degrades if wifi.json doesn't
  // include notification config.
  void configure(const std::vector<OtaManager::Network> &networks, const String &url,
                 const String &token);
  bool enabled() const { return enabled_; }
  void setEnabled(bool enabled);

  // Schedules one polling cycle on the worker task and returns immediately.
  // No-op when a poll is already in flight (the in-flight request consumes
  // the new tick). The worker pushes results into pending_ which the caller
  // consumes via drainPending().
  void requestPollAsync();
  // True while the worker is mid-cycle. Lets the caller suppress duplicate
  // polls or surface a "checking…" indicator if desired.
  bool pollInProgress() const { return pollInProgress_.load(); }

  // Caller persists this across reboots so previously-seen notifications
  // aren't re-shown after a power cycle. Reads under the worker mutex —
  // safe to call from the main loop.
  uint32_t lastSeenTs() const;
  void setLastSeenTs(uint32_t ts);

  // Drains pending notifications (those produced by the most recent poll).
  // Safe to call every tick; returns an empty vector when nothing is ready.
  // Caller takes ownership and is expected to surface them to the user.
  std::vector<Notification> drainPending();

 private:
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  // Body of one poll cycle. Runs on the worker task; touches pending_ /
  // lastSeenTs_ only under mutex_. Returns true if any notifications were
  // appended.
  bool pollSyncImpl();
  void ensureMutex();

  static void pollTaskTrampoline(void *arg);
  void pollTaskRun();

  std::vector<OtaManager::Network> networks_;
  String url_;
  String token_;
  bool enabled_ = true;
  // Both fields are touched by the worker; mutex_ guards them.
  uint32_t lastSeenTs_ = 0;
  std::vector<Notification> pending_;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;

  // Worker task plumbing.
  std::atomic<bool> pollInProgress_{false};
  TaskHandle_t pollTaskHandle_ = nullptr;
  SemaphoreHandle_t mutex_ = nullptr;
};
