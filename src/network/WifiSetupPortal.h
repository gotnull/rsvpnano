#pragma once

// Captive-portal Wi-Fi setup for rsvpnano. Spins up a soft-AP +
// DNS hijack + HTTP server on the device; the user joins the AP from a
// phone/laptop, picks an SSID from a scan list, enters the password, and
// hits Save. The portal hands the credentials back to App, which runs a
// synchronous WifiConnector::connect() test under user-visible "Testing…"
// status and persists the network into NVS + /wifi.json on success.
//
// Lifecycle: begin(apSsid) → tick(nowMs) per render frame → end().
// The class owns the WebServer + DNSServer; HTTP/DNS pumps run from
// App::update() so the main render loop stays in charge.

#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>

#include <vector>

class WifiSetupPortal {
 public:
  enum class State : uint8_t {
    Idle = 0,        // not running
    Awaiting,        // AP up, waiting for credentials POST
    Pending,         // /save received; App should run the connect test
    Testing,         // App is running WiFi.begin; portal shows "testing"
    Connected,       // last test succeeded
    Failed,          // last test failed (creds saved or not — App decides)
  };

  struct ScanResult {
    String ssid;
    int8_t rssi = 0;
    uint8_t enc = 0;  // WiFi auth mode code
  };

  bool begin(const String &apSsid);
  void end();
  bool isRunning() const { return running_; }

  // Drive DNS + HTTP pumps. Cheap; safe to call every loop tick.
  void tick(uint32_t nowMs);

  State state() const { return state_; }
  String apIp() const { return apIp_; }
  String apSsid() const { return apSsid_; }
  // Set by /save POST; consumed by App when state_ == Pending.
  String pendingSsid() const { return pendingSsid_; }
  String pendingPassword() const { return pendingPassword_; }
  // App calls this after consuming pending credentials so the portal moves
  // out of Pending state.
  void markTesting() { state_ = State::Testing; testingSsid_ = pendingSsid_; }
  // App calls this once it has finished the connect test.
  void markResult(bool ok, const String &errMsg = "") {
    lastError_ = errMsg;
    state_ = ok ? State::Connected : State::Failed;
    if (!ok) {
      pendingSsid_ = "";
      pendingPassword_ = "";
    } else {
      // Keep pendingSsid_ around for the status page until end() / next save.
    }
  }
  String lastError() const { return lastError_; }
  String testingSsid() const { return testingSsid_; }

  // Most recent WiFi scan results. Populated on /scan request (lazy).
  const std::vector<ScanResult> &scans() const { return scans_; }

 private:
  void registerRoutes();
  void doScan();
  String renderPage();
  String renderStatusJson();

  WebServer server_{80};
  DNSServer dns_;
  bool running_ = false;
  bool dnsActive_ = false;
  State state_ = State::Idle;
  String apSsid_;
  String apIp_;
  String pendingSsid_;
  String pendingPassword_;
  String testingSsid_;
  String lastError_;
  std::vector<ScanResult> scans_;
  uint32_t lastScanMs_ = 0;
};
