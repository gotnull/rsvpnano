#pragma once

#include <Arduino.h>

#include <vector>

#include "network/OtaManager.h"

class BookDownloadManager {
 public:
  struct RemoteBook {
    String path;          // full path inside the repo, e.g. "GEB1996/Gödel….epub"
    String displayName;   // basename minus .epub, used for the picker label
    uint32_t sizeBytes = 0;
  };

  using StatusCallback = void (*)(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);

  void setStatusCallback(StatusCallback callback, void *context);
  void configure(const std::vector<OtaManager::Network> &networks, const String &owner,
                 const String &repo, const String &branch = "HEAD");

  // Connects WiFi, queries the GitHub tree API, fills `out` with every .epub
  // blob in the repo. Disconnects WiFi on return. Returns true on success.
  bool listAvailable(std::vector<RemoteBook> &out);

  // Streams `book` from raw.githubusercontent into destPath on SD. Reuses the
  // same WiFi/HTTPClient pattern as listAvailable but with chunked progress
  // reporting through the status callback so the user gets a live %.
  bool download(const RemoteBook &book, const String &destPath);

  const String &lastError() const { return lastError_; }

 private:
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  bool connectWifi();
  // Percent-encodes each path segment (preserves '/'). GitHub raw URLs require
  // proper encoding for non-ASCII characters in filenames.
  static String urlEncodePath(const String &path);

  std::vector<OtaManager::Network> networks_;
  String owner_;
  String repo_;
  String branch_ = "HEAD";
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
  String lastError_;
};
