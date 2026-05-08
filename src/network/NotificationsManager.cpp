#include "network/NotificationsManager.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

namespace {

constexpr uint32_t kWifiConnectTimeoutMs = 20000;
constexpr uint32_t kHttpTimeoutMs = 8000;

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

void NotificationsManager::notifyStatus(const char *title, const char *line1, const char *line2,
                                        int progressPercent) {
  Serial.printf("[notif] %d%% %s | %s | %s\n", progressPercent,
                title ? title : "", line1 ? line1 : "", line2 ? line2 : "");
  if (statusCallback_) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

bool NotificationsManager::poll() {
  if (!enabled_ || networks_.empty() || url_.isEmpty()) {
    return false;
  }

  WiFi.mode(WIFI_STA);
  bool connected = false;
  for (const auto &net : networks_) {
    WiFi.disconnect(true);
    delay(50);
    WiFi.begin(net.ssid.c_str(), net.password.c_str());
    const uint32_t deadline = millis() + kWifiConnectTimeoutMs;
    while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
      delay(150);
      yield();
    }
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      Serial.printf("[notif] WiFi connected: %s\n", net.ssid.c_str());
      break;
    }
    Serial.printf("[notif] WiFi %s timed out\n", net.ssid.c_str());
  }
  if (!connected) {
    Serial.println("[notif] all WiFi networks failed");
    WiFi.disconnect(true);
    return false;
  }

  String fullUrl = url_;
  fullUrl += (fullUrl.indexOf('?') >= 0 ? "&since=" : "?since=") + String(lastSeenTs_);

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

  uint32_t newestTs = lastSeenTs_;
  for (const String &obj : objs) {
    Notification n;
    n.id = extractJsonString(obj, "id");
    n.title = extractJsonString(obj, "title");
    n.body = extractJsonString(obj, "body");
    n.ts = extractJsonNumber(obj, "ts");
    if (n.ts > newestTs) newestTs = n.ts;
    pending_.push_back(n);
  }
  lastSeenTs_ = newestTs;
  return !objs.empty();
}

std::vector<NotificationsManager::Notification> NotificationsManager::drainPending() {
  std::vector<Notification> out;
  out.swap(pending_);
  return out;
}
