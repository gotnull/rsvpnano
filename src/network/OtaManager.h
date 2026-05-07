#pragma once

#include <Arduino.h>

class OtaManager {
 public:
  using StatusCallback = void (*)(void *context, const char *title, const char *line1,
                                  const char *line2, int progressPercent);

  struct Config {
    String ssid;
    String password;
    String firmwareUrl;
    String notificationsUrl;
    String notificationsToken;
  };

  void setStatusCallback(StatusCallback callback, void *context);
  bool loadConfigFromSd(const char *path = "/wifi.json");
  bool runUpdate();
  const String &lastError() const { return lastError_; }
  const Config &config() const { return config_; }

 private:
  void notifyStatus(const char *title, const char *line1 = "", const char *line2 = "",
                    int progressPercent = -1);
  bool connectWifi();

  Config config_;
  StatusCallback statusCallback_ = nullptr;
  void *statusContext_ = nullptr;
  String lastError_;
};
