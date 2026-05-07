#include "network/OtaManager.h"

#include <HTTPClient.h>
#include <SD_MMC.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

namespace {

String extractJsonString(const String &json, const char *key) {
  String needle = String("\"") + key + "\"";
  int keyIdx = json.indexOf(needle);
  if (keyIdx < 0) return "";
  int colon = json.indexOf(':', keyIdx + needle.length());
  if (colon < 0) return "";
  int quoteOpen = json.indexOf('"', colon + 1);
  if (quoteOpen < 0) return "";
  int quoteClose = json.indexOf('"', quoteOpen + 1);
  while (quoteClose > 0 && json[quoteClose - 1] == '\\') {
    quoteClose = json.indexOf('"', quoteClose + 1);
  }
  if (quoteClose < 0) return "";
  return json.substring(quoteOpen + 1, quoteClose);
}

}  // namespace

void OtaManager::setStatusCallback(StatusCallback callback, void *context) {
  statusCallback_ = callback;
  statusContext_ = context;
}

void OtaManager::notifyStatus(const char *title, const char *line1, const char *line2,
                              int progressPercent) {
  Serial.printf("[ota] %d%% %s | %s | %s\n", progressPercent,
                title ? title : "", line1 ? line1 : "", line2 ? line2 : "");
  if (statusCallback_) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

bool OtaManager::loadConfigFromSd(const char *path) {
  File f = SD_MMC.open(path);
  if (!f || f.isDirectory()) {
    if (f) f.close();
    lastError_ = "wifi.json missing on SD";
    return false;
  }
  String contents;
  while (f.available()) {
    contents += static_cast<char>(f.read());
    if (contents.length() > 4096) break;
  }
  f.close();

  config_.ssid = extractJsonString(contents, "ssid");
  config_.password = extractJsonString(contents, "password");
  config_.firmwareUrl = extractJsonString(contents, "url");
  config_.notificationsUrl = extractJsonString(contents, "notifications_url");
  config_.notificationsToken = extractJsonString(contents, "notifications_token");
  if (config_.ssid.isEmpty() || config_.firmwareUrl.isEmpty()) {
    lastError_ = "wifi.json missing ssid or url";
    return false;
  }
  return true;
}

bool OtaManager::connectWifi() {
  notifyStatus("OTA", "Connecting WiFi", config_.ssid.c_str(), 5);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(config_.ssid.c_str(), config_.password.c_str());

  const uint32_t deadline = millis() + 20000;
  while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
    delay(250);
    yield();
  }
  if (WiFi.status() != WL_CONNECTED) {
    lastError_ = "WiFi connect timeout";
    notifyStatus("OTA", "WiFi failed", "Check wifi.json", 100);
    return false;
  }
  Serial.printf("[ota] WiFi connected, IP=%s RSSI=%d\n", WiFi.localIP().toString().c_str(),
                WiFi.RSSI());
  return true;
}

bool OtaManager::runUpdate() {
  lastError_ = "";
  if (config_.ssid.isEmpty() || config_.firmwareUrl.isEmpty()) {
    if (!loadConfigFromSd()) {
      notifyStatus("OTA", "No config", "Add /wifi.json on SD", 100);
      return false;
    }
  }
  if (!connectWifi()) {
    return false;
  }

  String displayName = config_.firmwareUrl;
  const int lastSlash = displayName.lastIndexOf('/');
  if (lastSlash >= 0) displayName = displayName.substring(lastSlash + 1);
  if (displayName.isEmpty()) displayName = "firmware.bin";
  notifyStatus("OTA", "Fetching", displayName.c_str(), 15);

  HTTPClient http;
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setTimeout(15000);
  const bool useHttps = config_.firmwareUrl.startsWith("https://");
  const bool beginOk = useHttps ? http.begin(secureClient, config_.firmwareUrl)
                                : http.begin(config_.firmwareUrl);
  if (!beginOk) {
    lastError_ = "HTTPClient.begin failed";
    notifyStatus("OTA", "URL invalid", config_.firmwareUrl.c_str(), 100);
    WiFi.disconnect(true);
    return false;
  }

  const int code = http.GET();
  if (code != HTTP_CODE_OK) {
    lastError_ = String("HTTP ") + code;
    notifyStatus("OTA", "Server error", lastError_.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  const int contentLength = http.getSize();
  if (contentLength <= 0) {
    lastError_ = "Unknown content length";
    notifyStatus("OTA", "Bad response", "No size", 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  if (!Update.begin(static_cast<size_t>(contentLength))) {
    lastError_ = String("Update.begin: ") + Update.errorString();
    notifyStatus("OTA", "Begin failed", lastError_.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  uint8_t buf[1024];
  size_t written = 0;
  uint32_t lastReportMs = millis();
  while (http.connected() && (contentLength < 0 || written < static_cast<size_t>(contentLength))) {
    const size_t available = stream->available();
    if (available > 0) {
      const int got = stream->readBytes(
          buf, std::min<size_t>(sizeof(buf), available));
      if (got > 0) {
        if (Update.write(buf, got) != static_cast<size_t>(got)) {
          lastError_ = String("Update.write: ") + Update.errorString();
          notifyStatus("OTA", "Write failed", lastError_.c_str(), 100);
          Update.abort();
          http.end();
          WiFi.disconnect(true);
          return false;
        }
        written += got;
        const uint32_t now = millis();
        if (now - lastReportMs >= 200) {
          const int dlPercent =
              static_cast<int>((written * 100ULL) / static_cast<size_t>(contentLength));
          const int barPercent = 15 + (dlPercent * 80) / 100;
          const String line2 = String(dlPercent) + "% - " +
                               String(written / 1024) + "/" +
                               String(static_cast<size_t>(contentLength) / 1024) + " KB";
          notifyStatus("OTA", "Downloading", line2.c_str(), std::min(barPercent, 95));
          lastReportMs = now;
        }
      }
    } else {
      delay(2);
      yield();
    }
  }

  if (!Update.end(true)) {
    lastError_ = String("Update.end: ") + Update.errorString();
    notifyStatus("OTA", "Finalize failed", lastError_.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  http.end();
  WiFi.disconnect(true);
  notifyStatus("OTA", "Update OK", "Rebooting...", 100);
  delay(800);
  ESP.restart();
  return true;
}
