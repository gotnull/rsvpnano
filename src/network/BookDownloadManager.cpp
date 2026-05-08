#include "network/BookDownloadManager.h"

#include <HTTPClient.h>
#include <SD_MMC.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "network/WifiConnector.h"

namespace {

constexpr uint32_t kWifiConnectTimeoutMs = 20000;
constexpr uint32_t kHttpTimeoutMs = 15000;

// Manual scan for tree entries with type=blob and path ending in .epub. The
// GitHub tree response can be ~250 KB for medium repos; rather than parse the
// full JSON we walk it once looking for the structurally-consistent path/size
// fields each entry exposes.
void extractEpubEntries(const String &payload,
                        std::vector<BookDownloadManager::RemoteBook> &out) {
  int idx = 0;
  while (true) {
    int pathKey = payload.indexOf("\"path\":\"", idx);
    if (pathKey < 0) break;
    const int pathStart = pathKey + 8;
    int pathEnd = pathStart;
    while (pathEnd < static_cast<int>(payload.length())) {
      const char c = payload[pathEnd];
      if (c == '"' && payload[pathEnd - 1] != '\\') break;
      ++pathEnd;
    }
    if (pathEnd >= static_cast<int>(payload.length())) break;

    String path = payload.substring(pathStart, pathEnd);
    String lowered = path;
    lowered.toLowerCase();
    idx = pathEnd + 1;
    if (!lowered.endsWith(".epub")) continue;

    // Walk forward to grab type + size in this same tree entry. Each entry
    // closes with a `}`, so bound the search there.
    const int entryEnd = payload.indexOf('}', idx);
    if (entryEnd < 0) break;
    const String entry = payload.substring(idx, entryEnd);
    if (entry.indexOf("\"type\":\"blob\"") < 0) {
      idx = entryEnd + 1;
      continue;
    }
    uint32_t size = 0;
    const int sizeKey = entry.indexOf("\"size\":");
    if (sizeKey >= 0) {
      int p = sizeKey + 7;
      while (p < static_cast<int>(entry.length()) && entry[p] == ' ') ++p;
      int q = p;
      while (q < static_cast<int>(entry.length()) && isdigit(entry[q])) ++q;
      if (q > p) size = static_cast<uint32_t>(entry.substring(p, q).toInt());
    }

    BookDownloadManager::RemoteBook book;
    book.path = path;
    book.sizeBytes = size;
    String basename = path;
    const int slash = basename.lastIndexOf('/');
    if (slash >= 0) basename = basename.substring(slash + 1);
    if (basename.endsWith(".epub")) basename = basename.substring(0, basename.length() - 5);
    book.displayName = basename;
    out.push_back(book);
    idx = entryEnd + 1;
  }
}

}  // namespace

void BookDownloadManager::setStatusCallback(StatusCallback callback, void *context) {
  statusCallback_ = callback;
  statusContext_ = context;
}

void BookDownloadManager::configure(const std::vector<OtaManager::Network> &networks,
                                    const String &owner, const String &repo,
                                    const String &branch) {
  networks_ = networks;
  owner_ = owner;
  repo_ = repo;
  branch_ = branch.isEmpty() ? String("HEAD") : branch;
}

void BookDownloadManager::notifyStatus(const char *title, const char *line1,
                                       const char *line2, int progressPercent) {
  Serial.printf("[booksdl] %d%% %s | %s | %s\n", progressPercent,
                title ? title : "", line1 ? line1 : "", line2 ? line2 : "");
  if (statusCallback_) {
    statusCallback_(statusContext_, title, line1, line2, progressPercent);
  }
}

bool BookDownloadManager::connectWifi() {
  if (networks_.empty()) {
    lastError_ = "No WiFi networks configured";
    notifyStatus("Books", "WiFi not set", "Add networks to /wifi.json", 100);
    return false;
  }
  notifyStatus("Books", "Connecting WiFi", networks_.front().ssid.c_str(), 5);
  if (!WifiConnector::connect(networks_, kWifiConnectTimeoutMs, "booksdl")) {
    lastError_ = "All WiFi networks failed";
    notifyStatus("Books", "WiFi failed", "All networks tried", 100);
    return false;
  }
  return true;
}

bool BookDownloadManager::listAvailable(std::vector<RemoteBook> &out) {
  out.clear();
  if (owner_.isEmpty() || repo_.isEmpty()) {
    lastError_ = "Repo not configured";
    return false;
  }
  if (!connectWifi()) return false;

  const String url = String("https://api.github.com/repos/") + owner_ + "/" + repo_ +
                     "/git/trees/" + branch_ + "?recursive=1";
  notifyStatus("Books", "Fetching index", repo_.c_str(), 20);

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();
  http.setTimeout(kHttpTimeoutMs);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  // GitHub API requires a User-Agent header.
  http.setUserAgent("rsvpnano/1.0");
  if (!http.begin(client, url)) {
    lastError_ = "http.begin failed";
    WiFi.disconnect(true);
    return false;
  }
  http.addHeader("Accept", "application/vnd.github+json");
  const int code = http.GET();
  if (code != HTTP_CODE_OK) {
    lastError_ = String("HTTP ") + code;
    notifyStatus("Books", "Index failed", lastError_.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  const String payload = http.getString();
  http.end();
  WiFi.disconnect(true);

  extractEpubEntries(payload, out);
  notifyStatus("Books", "Index ready", String(out.size()).c_str(), 100);
  return true;
}

String BookDownloadManager::urlEncodePath(const String &path) {
  String out;
  out.reserve(path.length() * 3);
  for (size_t i = 0; i < path.length(); ++i) {
    const uint8_t c = static_cast<uint8_t>(path[i]);
    const bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
                            c == '~' || c == '/';
    if (unreserved) {
      out += static_cast<char>(c);
    } else {
      char buf[4];
      snprintf(buf, sizeof(buf), "%%%02X", c);
      out += buf;
    }
  }
  return out;
}

bool BookDownloadManager::download(const RemoteBook &book, const String &destPath) {
  if (book.path.isEmpty() || destPath.isEmpty()) {
    lastError_ = "Invalid args";
    return false;
  }
  if (!connectWifi()) return false;

  const String url = String("https://raw.githubusercontent.com/") + owner_ + "/" + repo_ +
                     "/" + branch_ + "/" + urlEncodePath(book.path);
  notifyStatus("Books", "Downloading", book.displayName.c_str(), 5);

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();
  http.setTimeout(kHttpTimeoutMs);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setUserAgent("rsvpnano/1.0");
  if (!http.begin(client, url)) {
    lastError_ = "http.begin failed";
    WiFi.disconnect(true);
    return false;
  }
  const int code = http.GET();
  if (code != HTTP_CODE_OK) {
    lastError_ = String("HTTP ") + code;
    notifyStatus("Books", "Server error", lastError_.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }
  const int contentLength = http.getSize();
  const uint32_t totalBytes = contentLength > 0 ? static_cast<uint32_t>(contentLength)
                                                : book.sizeBytes;

  // Make sure the parent dir exists. SD_MMC handles single-level mkdir; this
  // matters for e.g. /books/.
  const int destSlash = destPath.lastIndexOf('/');
  if (destSlash > 0) {
    const String parent = destPath.substring(0, destSlash);
    if (!SD_MMC.exists(parent)) {
      SD_MMC.mkdir(parent);
    }
  }
  // Remove a previous incomplete copy so a fresh write doesn't get appended.
  if (SD_MMC.exists(destPath)) {
    SD_MMC.remove(destPath);
  }
  File f = SD_MMC.open(destPath, FILE_WRITE);
  if (!f) {
    lastError_ = "SD open failed";
    notifyStatus("Books", "SD write failed", destPath.c_str(), 100);
    http.end();
    WiFi.disconnect(true);
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  uint8_t buf[1024];
  uint32_t written = 0;
  uint32_t lastReportMs = millis();
  while (http.connected() && (totalBytes == 0 || written < totalBytes)) {
    const size_t available = stream->available();
    if (available > 0) {
      const int got = stream->readBytes(buf, std::min<size_t>(sizeof(buf), available));
      if (got > 0) {
        if (f.write(buf, got) != static_cast<size_t>(got)) {
          lastError_ = "SD write short";
          notifyStatus("Books", "SD write failed", lastError_.c_str(), 100);
          f.close();
          http.end();
          WiFi.disconnect(true);
          return false;
        }
        written += got;
        const uint32_t now = millis();
        if (now - lastReportMs >= 250) {
          lastReportMs = now;
          if (totalBytes > 0) {
            const int dlPercent = static_cast<int>((written * 100ULL) / totalBytes);
            const String line2 = String(dlPercent) + "% - " +
                                 String(written / 1024) + "/" + String(totalBytes / 1024) + " KB";
            notifyStatus("Books", "Downloading", line2.c_str(), 5 + (dlPercent * 90) / 100);
          } else {
            notifyStatus("Books", "Downloading", String(written / 1024).c_str(), -1);
          }
        }
      }
    } else {
      delay(2);
      yield();
    }
  }
  f.close();
  http.end();
  WiFi.disconnect(true);
  notifyStatus("Books", "Saved", destPath.c_str(), 100);
  return true;
}
