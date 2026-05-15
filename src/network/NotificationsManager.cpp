#include "network/NotificationsManager.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "network/WifiConnector.h"

namespace {

constexpr uint32_t kWifiConnectTimeoutMs = 20000;
constexpr uint32_t kHttpTimeoutMs = 8000;

// Pinned to APP_CPU (core 1, same core as the main loop) — FreeRTOS preempts
// between tasks at every tick so the worker still makes progress without
// starving the UI. 12 KB stack: WiFiClientSecure's TLS handshake pushes large
// frames; 8 KB was tight in early tests.
constexpr UBaseType_t kPollTaskPrio = tskIDLE_PRIORITY + 1;
constexpr uint32_t kPollTaskStack = 12 * 1024;
constexpr BaseType_t kPollTaskCore = 1;

String extractJsonString(const String &json, const char *key) {
  String needle = String("\"") + key + "\"";
  int idx = json.indexOf(needle);
  if (idx < 0) return "";
  int colon = json.indexOf(':', idx + needle.length());
  if (colon < 0) return "";
  int q1 = json.indexOf('"', colon + 1);
  if (q1 < 0) return "";
  int q2 = json.indexOf('"', q1 + 1);
  while (q2 > 0 && json[q2 - 1] == '\\') {
    q2 = json.indexOf('"', q2 + 1);
  }
  if (q2 < 0) return "";
  return json.substring(q1 + 1, q2);
}

uint32_t extractJsonNumber(const String &json, const char *key) {
  String needle = String("\"") + key + "\"";
  int idx = json.indexOf(needle);
  if (idx < 0) return 0;
  int colon = json.indexOf(':', idx + needle.length());
  if (colon < 0) return 0;
  int start = colon + 1;
  while (start < static_cast<int>(json.length()) &&
         (json[start] == ' ' || json[start] == '\t')) {
    ++start;
  }
  int end = start;
  while (end < static_cast<int>(json.length()) &&
         (isdigit(json[end]) || json[end] == '-')) {
    ++end;
  }
  if (end == start) return 0;
  return static_cast<uint32_t>(json.substring(start, end).toInt());
}

// Each notification object is a brace-delimited block; split the array into them.
std::vector<String> splitJsonObjects(const String &arrayBody) {
  std::vector<String> objs;
  int depth = 0;
  int start = -1;
  for (int i = 0; i < static_cast<int>(arrayBody.length()); ++i) {
    const char c = arrayBody[i];
    if (c == '{') {
      if (depth == 0) start = i;
      ++depth;
    } else if (c == '}') {
      --depth;
      if (depth == 0 && start >= 0) {
        objs.push_back(arrayBody.substring(start, i + 1));
        start = -1;
      }
    }
  }
  return objs;
}

}  // namespace

NotificationsManager::~NotificationsManager() {
  // Don't try to delete the task: poll runs are short-lived (worst-case ~28 s)
  // and the manager only ever lives for the program lifetime. If we ever
  // teardown for real, send a cancellation flag instead of nuking mid-HTTP.
  if (mutex_ != nullptr) {
    vSemaphoreDelete(mutex_);
    mutex_ = nullptr;
  }
}

void NotificationsManager::ensureMutex() {
  if (mutex_ == nullptr) {
    mutex_ = xSemaphoreCreateMutex();
    if (mutex_ == nullptr) {
      Serial.println("[notif] FATAL: could not create poll mutex");
    }
  }
}

void NotificationsManager::setStatusCallback(StatusCallback callback, void *context) {
  statusCallback_ = callback;
  statusContext_ = context;
}

void NotificationsManager::configure(const std::vector<OtaManager::Network> &networks,
                                     const String &url, const String &token) {
  networks_ = networks;
  url_ = url;
  token_ = token;
}

void NotificationsManager::setEnabled(bool enabled) { enabled_ = enabled; }

uint32_t NotificationsManager::lastSeenTs() const {
  // Mutex protects against torn reads while the worker is mid-update.
  if (mutex_ == nullptr) return lastSeenTs_;
  uint32_t snapshot = 0;
  if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
    snapshot = lastSeenTs_;
    xSemaphoreGive(mutex_);
  }
  return snapshot;
}

void NotificationsManager::setLastSeenTs(uint32_t ts) {
  // Called only from the main loop / config restore (no worker in flight here),
  // but use the mutex for symmetry with the reader.
  ensureMutex();
  if (mutex_ != nullptr && xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
    lastSeenTs_ = ts;
    xSemaphoreGive(mutex_);
  } else {
    lastSeenTs_ = ts;
  }
}

void NotificationsManager::notifyStatus(const char *title, const char *line1, const char *line2,
                                        int progressPercent) {
  Serial.printf("[notif] %d%% %s | %s | %s\n", progressPercent,
                title ? title : "", line1 ? line1 : "", line2 ? line2 : "");
  if (statusCallback_) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

void NotificationsManager::requestPollAsync() {
  if (!enabled_ || networks_.empty() || url_.isEmpty()) {
    return;
  }
  // CAS guard: only one worker in flight at a time. The atomic compare-exchange
  // makes this safe regardless of how many threads hit requestPollAsync().
  bool expected = false;
  if (!pollInProgress_.compare_exchange_strong(expected, true)) {
    return;  // another poll already running; let it finish
  }
  ensureMutex();
  Serial.println("[notif] async poll: spawning worker");
  const BaseType_t ok = xTaskCreatePinnedToCore(
      &NotificationsManager::pollTaskTrampoline,
      "notif_poll",
      kPollTaskStack,
      this,
      kPollTaskPrio,
      &pollTaskHandle_,
      kPollTaskCore);
  if (ok != pdPASS) {
    Serial.println("[notif] xTaskCreate FAILED — dropping this poll cycle");
    pollTaskHandle_ = nullptr;
    pollInProgress_.store(false);
  }
}

void NotificationsManager::pollTaskTrampoline(void *arg) {
  auto *self = static_cast<NotificationsManager *>(arg);
  self->pollTaskRun();
  self->pollTaskHandle_ = nullptr;
  self->pollInProgress_.store(false);
  vTaskDelete(nullptr);
}

void NotificationsManager::pollTaskRun() {
  const uint32_t startMs = millis();
  const bool ok = pollSyncImpl();
  Serial.printf("[notif-worker] poll %s in %lu ms\n",
                ok ? "succeeded" : "(no fresh items)",
                static_cast<unsigned long>(millis() - startMs));
}

bool NotificationsManager::pollSyncImpl() {
  if (!enabled_ || networks_.empty() || url_.isEmpty()) {
    return false;
  }

  if (!WifiConnector::connect(networks_, kWifiConnectTimeoutMs, "notif")) {
    return false;
  }

  // Snapshot lastSeenTs under the mutex so the URL we build is consistent
  // with whatever the main loop last persisted.
  uint32_t lastSeenSnapshot = 0;
  if (mutex_ != nullptr && xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
    lastSeenSnapshot = lastSeenTs_;
    xSemaphoreGive(mutex_);
  }

  String fullUrl = url_;
  fullUrl += (fullUrl.indexOf('?') >= 0 ? "&since=" : "?since=") + String(lastSeenSnapshot);

  HTTPClient http;
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  http.setTimeout(kHttpTimeoutMs);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  const bool useHttps = fullUrl.startsWith("https://");
  const bool beginOk =
      useHttps ? http.begin(secureClient, fullUrl) : http.begin(fullUrl);
  if (!beginOk) {
    Serial.println("[notif] http.begin failed");
    WiFi.disconnect(true);
    return false;
  }
  if (!token_.isEmpty()) {
    http.addHeader("Authorization", String("Bearer ") + token_);
  }

  const int code = http.GET();
  if (code != HTTP_CODE_OK) {
    Serial.printf("[notif] HTTP %d\n", code);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  const String payload = http.getString();
  http.end();
  WiFi.disconnect(true);

  // Pull `notifications` array section out of the response.
  const int arrayStart = payload.indexOf('[');
  const int arrayEnd = payload.lastIndexOf(']');
  if (arrayStart < 0 || arrayEnd <= arrayStart) {
    return false;
  }
  const String arrayBody = payload.substring(arrayStart + 1, arrayEnd);
  const std::vector<String> objs = splitJsonObjects(arrayBody);

  // Parse first, then commit the results under the mutex in one go — keeps
  // the critical section short and avoids holding the lock through any of
  // the network or JSON work.
  std::vector<Notification> fresh;
  fresh.reserve(objs.size());
  uint32_t newestTs = lastSeenSnapshot;
  for (const String &obj : objs) {
    Notification n;
    n.id = extractJsonString(obj, "id");
    n.title = extractJsonString(obj, "title");
    n.body = extractJsonString(obj, "body");
    n.ts = extractJsonNumber(obj, "ts");
    if (n.ts > newestTs) newestTs = n.ts;
    fresh.push_back(std::move(n));
  }

  if (mutex_ != nullptr && xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
    for (auto &n : fresh) pending_.push_back(std::move(n));
    lastSeenTs_ = newestTs;
    xSemaphoreGive(mutex_);
  } else {
    // Mutex creation failed — degrade to direct write. Better to lose
    // perfect synchronisation than to drop the notifications entirely.
    for (auto &n : fresh) pending_.push_back(std::move(n));
    lastSeenTs_ = newestTs;
  }
  return !objs.empty();
}

std::vector<NotificationsManager::Notification> NotificationsManager::drainPending() {
  std::vector<Notification> out;
  if (mutex_ == nullptr) {
    out.swap(pending_);
    return out;
  }
  if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE) {
    out.swap(pending_);
    xSemaphoreGive(mutex_);
  }
  return out;
}
