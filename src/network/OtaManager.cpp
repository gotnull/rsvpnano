#include "network/OtaManager.h"

#include <HTTPClient.h>
#include <SD_MMC.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "network/WifiConnector.h"

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

void OtaManager::setConfig(const Config &cfg) { config_ = cfg; }

void OtaManager::notifyStatus(const char *title, const char *line1, const char *line2,
                              int progressPercent) {
  Serial.printf("[ota] %d%% %s | %s | %s\n", progressPercent,
                title ? title : "", line1 ? line1 : "", line2 ? line2 : "");
  if (statusCallback_) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

namespace {

// Walks the "networks" array (if present) pulling out per-entry ssid+password.
void parseNetworksArray(const String &json, std::vector<OtaManager::Network> &out) {
  const int arrKey = json.indexOf("\"networks\"");
  if (arrKey < 0) return;
  const int arrStart = json.indexOf('[', arrKey);
  if (arrStart < 0) return;
  // Find matching ]
  int depth = 0;
  int arrEnd = -1;
  for (int i = arrStart; i < static_cast<int>(json.length()); ++i) {
    if (json[i] == '[') ++depth;
    else if (json[i] == ']') {
      --depth;
      if (depth == 0) { arrEnd = i; break; }
    }
  }
  if (arrEnd < 0) return;

  // Each object {"ssid": "...", "password": "..."}
  int cursor = arrStart + 1;
  while (cursor < arrEnd) {
    const int objStart = json.indexOf('{', cursor);
    if (objStart < 0 || objStart >= arrEnd) break;
    int objDepth = 0;
    int objEnd = -1;
    for (int i = objStart; i < arrEnd; ++i) {
      if (json[i] == '{') ++objDepth;
      else if (json[i] == '}') {
        --objDepth;
        if (objDepth == 0) { objEnd = i; break; }
      }
    }
    if (objEnd < 0) break;
    const String obj = json.substring(objStart, objEnd + 1);
    OtaManager::Network n;
    n.ssid = extractJsonString(obj, "ssid");
    n.password = extractJsonString(obj, "password");
    if (!n.ssid.isEmpty()) {
      out.push_back(n);
    }
    cursor = objEnd + 1;
  }
}

}  // namespace

bool OtaManager::loadConfigFromSd(const char *path) {
  // Parse into a LOCAL config first so a missing SD or partial parse never
  // wipes whatever's already in config_ (NVS-restored fallback, etc.).
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

  Config parsed;
  parseNetworksArray(contents, parsed.networks);
  const String legacySsid = extractJsonString(contents, "ssid");
  const String legacyPass = extractJsonString(contents, "password");
  if (!legacySsid.isEmpty()) {
    Network n;
    n.ssid = legacySsid;
    n.password = legacyPass;
    parsed.networks.push_back(n);
  }
  parsed.firmwareUrl = extractJsonString(contents, "url");
  parsed.notificationsUrl = extractJsonString(contents, "notifications_url");
  parsed.notificationsToken = extractJsonString(contents, "notifications_token");
  if (parsed.networks.empty() || parsed.firmwareUrl.isEmpty()) {
    lastError_ = "wifi.json missing networks or url";
    return false;
  }
  config_ = parsed;
  return true;
}

bool OtaManager::connectWifi() {
  if (config_.networks.empty()) {
    lastError_ = "No networks configured";
    notifyStatus("OTA", "No WiFi", "Check wifi.json", 100);
    return false;
  }
  notifyStatus("OTA", "Connecting WiFi", config_.networks.front().ssid.c_str(), 5);
  if (WifiConnector::connect(
          config_.networks, 20000, "ota",
          +[](void *ctx, const String &ssid, size_t i, size_t total, bool connected) {
            auto *self = static_cast<OtaManager *>(ctx);
            if (!connected) {
              const String line2 = String("Trying ") + ssid;
              self->notifyStatus("OTA", "Connecting WiFi", line2.c_str(), 5);
            }
          },
          this)) {
    return true;
  }
  lastError_ = "All WiFi networks failed";
  notifyStatus("OTA", "WiFi failed", "All networks tried", 100);
  return false;
}

String OtaManager::resolveGithubLatestAssetUrl(const String &assetName) {
  // Derive api.github.com URL from the configured /releases/latest/download
  // URL. config_.firmwareUrl shape:
  //   https://github.com/<owner>/<repo>/releases/latest/download/<asset>
  // We slice out <owner>/<repo> and build:
  //   https://api.github.com/repos/<owner>/<repo>/releases/latest
  const String prefix = "https://github.com/";
  if (!config_.firmwareUrl.startsWith(prefix)) return "";
  const int repoEnd = config_.firmwareUrl.indexOf("/releases/", prefix.length());
  if (repoEnd < 0) return "";
  const String ownerRepo = config_.firmwareUrl.substring(prefix.length(), repoEnd);
  const String apiUrl = String("https://api.github.com/repos/") + ownerRepo + "/releases/latest";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(15000);
  http.setReuse(false);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  // GitHub API requires a User-Agent; otherwise it 403s.
  http.setUserAgent("rsvp-nano-ota/1.0");
  if (!http.begin(client, apiUrl)) {
    Serial.println("[ota] api: http.begin failed");
    return "";
  }
  const int code = http.GET();
  Serial.printf("[ota] api %s → HTTP %d\n", apiUrl.c_str(), code);
  if (code != HTTP_CODE_OK) {
    http.end();
    return "";
  }
  // The release JSON typically runs 4-8 KB. We don't need full parsing —
  // just scan for the assets-array entry whose "name" matches our asset
  // file, then grab the "browser_download_url" that follows it within the
  // same asset object.
  const String body = http.getString();
  http.end();

  // Find the entry where "name":"<assetName>" appears, then look forward
  // for the next "browser_download_url":"<url>" — that pair is always in
  // the same object in GitHub's response.
  const String nameKey = String("\"name\":\"") + assetName + "\"";
  const int nameIdx = body.indexOf(nameKey);
  if (nameIdx < 0) {
    Serial.printf("[ota] api: asset %s not in response (body=%u bytes)\n",
                  assetName.c_str(), static_cast<unsigned>(body.length()));
    return "";
  }
  const String urlKey = "\"browser_download_url\":\"";
  const int urlIdx = body.indexOf(urlKey, nameIdx);
  if (urlIdx < 0) {
    Serial.println("[ota] api: browser_download_url missing after asset name");
    return "";
  }
  const int urlStart = urlIdx + urlKey.length();
  const int urlEnd = body.indexOf('"', urlStart);
  if (urlEnd < 0) return "";
  return body.substring(urlStart, urlEnd);
}

bool OtaManager::runUpdate() {
  lastError_ = "";
  // App::begin already populated config_ from SD or NVS fallback. Don't try to
  // re-read SD here — if the user pulled the card out since boot, that read
  // would fail and (used to) wipe the in-memory config we still need to OTA.
  if (config_.networks.empty() || config_.firmwareUrl.isEmpty()) {
    notifyStatus("OTA", "No config", "Re-insert SD or fix wifi.json", 100);
    return false;
  }
  if (!connectWifi()) {
    return false;
  }

  String displayName = config_.firmwareUrl;
  const int lastSlash = displayName.lastIndexOf('/');
  if (lastSlash >= 0) displayName = displayName.substring(lastSlash + 1);
  if (displayName.isEmpty()) displayName = "firmware.bin";

  // Resolve the actual asset URL via the GitHub API when the configured URL
  // is the `/releases/latest/download/<asset>` form. That URL chains
  // through 2-3 redirects (github.com → objects.githubusercontent.com via
  // fastly); HTTPClient + WiFiClientSecure historically choke on the
  // cross-host hop with HTTP -1. Hitting api.github.com directly returns a
  // single-response JSON we can parse for the artifact's signed
  // `browser_download_url` and fetch in one shot — no cross-host redirect.
  String fetchUrl = config_.firmwareUrl;
  if (config_.firmwareUrl.startsWith("https://github.com/") &&
      config_.firmwareUrl.indexOf("/releases/latest/download/") > 0) {
    notifyStatus("OTA", "Resolving", "api.github.com", 10);
    const String resolved = resolveGithubLatestAssetUrl(displayName);
    if (!resolved.isEmpty()) {
      Serial.printf("[ota] resolved asset URL via API: %s\n", resolved.c_str());
      fetchUrl = resolved;
    } else {
      Serial.println("[ota] API resolve failed — falling back to /latest/download URL");
    }
  }

  notifyStatus("OTA", "Fetching", displayName.c_str(), 15);

  // Three attempts with a fresh client each, plus a *small* TLS buffer
  // (4 KB rx / 1 KB tx) so the handshake heap requirement drops from
  // ~30 KB to ~8 KB — that's the actual flake source on tight heap
  // (scene infra + audio task have eaten into free heap since OTA was
  // first written).
  constexpr int kOtaMaxAttempts = 3;
  const bool useHttps = fetchUrl.startsWith("https://");
  int code = -1;
  HTTPClient http;
  WiFiClientSecure secureClient;
  for (int attempt = 1; attempt <= kOtaMaxAttempts; ++attempt) {
    http.end();
    secureClient.stop();
    secureClient.setInsecure();
      http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setTimeout(20000);
    http.setReuse(false);
    const bool beginOk = useHttps ? http.begin(secureClient, fetchUrl)
                                  : http.begin(fetchUrl);
    if (!beginOk) {
      lastError_ = "HTTPClient.begin failed";
      notifyStatus("OTA", "URL invalid", fetchUrl.c_str(), 100);
      WiFi.disconnect(true);
      return false;
    }
    code = http.GET();
    Serial.printf("[ota] attempt %d/%d → HTTP %d\n", attempt, kOtaMaxAttempts, code);
    if (code == HTTP_CODE_OK) break;
    if (attempt < kOtaMaxAttempts) {
      String msg = String("Retry ") + attempt + "/" + kOtaMaxAttempts;
      notifyStatus("OTA", msg.c_str(), (String("HTTP ") + code).c_str(), 20);
      delay(800);
    }
  }
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
