#pragma once

#include <Arduino.h>

#include <vector>

class OtaManager {
 public:
  using StatusCallback = void (*)(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);

  struct Network {
    String ssid;
    String password;
  };

  struct Config {
    // Networks tried in order until one connects. Single-network legacy
    // wifi.json (top-level "ssid" / "password") still works — the loader
    // pushes that into networks[0].
    std::vector<Network> networks;
    String firmwareUrl;
    String notificationsUrl;
    String notificationsToken;
  };

  void setStatusCallback(StatusCallback callback, void *context);
  bool loadConfigFromSd(const char *path = "/wifi.json");
  // Direct injection of a previously-cached Config (e.g. from NVS) so OTA
  // and other network features still work when the SD card has been pulled
  // out and the wifi.json read fails.
  void setConfig(const Config &cfg);
  bool runUpdate();
  const String &lastError() const { return lastError_; }
  const Config &config() const { return config_; }

 private:
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  bool connectWifi();
  // Hits api.github.com/repos/<owner>/<repo>/releases/latest, parses the
  // JSON for the named asset's signed `browser_download_url`, and
  // returns it. The signed URL points directly at
  // objects.githubusercontent.com so the firmware fetch is a single hop
  // — no cross-host redirect chain. Returns empty on any failure; the
  // caller falls back to the legacy /releases/latest/download/ URL.
  String resolveGithubLatestAssetUrl(const String &assetName);

  Config config_;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
  String lastError_;
};
