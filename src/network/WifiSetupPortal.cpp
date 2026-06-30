#include "network/WifiSetupPortal.h"

#include <WiFi.h>

namespace {

// Encryption-type → short tag for the scan list. The Arduino WiFi enum
// values differ slightly between cores; the open/wpa-only distinction is
// what most users care about.
const char *encTag(uint8_t enc) {
  switch (enc) {
    case WIFI_AUTH_OPEN:           return "open";
    case WIFI_AUTH_WEP:            return "WEP";
    case WIFI_AUTH_WPA_PSK:        return "WPA";
    case WIFI_AUTH_WPA2_PSK:       return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:   return "WPA/2";
    case WIFI_AUTH_WPA2_ENTERPRISE:return "WPA2-E";
    case WIFI_AUTH_WPA3_PSK:       return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:  return "WPA2/3";
    default:                       return "?";
  }
}

constexpr uint32_t kScanCacheMs = 8000;  // re-scan no more than every 8 s

// Single page rendered for /, captive-probe URLs, and any unknown route so a
// phone's captive-portal sheet pops up immediately on join.
const char kPageTemplate[] PROGMEM = R"HTML(<!doctype html>
<html><head><meta name=viewport content="width=device-width,initial-scale=1">
<title>rsvpnano Wi-Fi setup</title>
<style>
body{font:14px -apple-system,Segoe UI,Roboto,sans-serif;background:#111;color:#eee;margin:0;padding:18px;max-width:480px}
h1{font-size:18px;margin:0 0 6px}
p.muted{color:#999;margin:0 0 14px;font-size:12px}
label{display:block;margin:10px 0 4px;font-size:12px;color:#aaa}
select,input,button{width:100%;box-sizing:border-box;padding:10px;border-radius:6px;border:1px solid #333;background:#1c1c1c;color:#eee;font-size:14px}
button{background:#2b6cb0;border:none;font-weight:600;margin-top:14px;cursor:pointer}
button:hover{background:#3e84cd}
#status{margin-top:14px;padding:10px;border-radius:6px;background:#1a1a1a;font-size:13px;color:#ccc}
.ok{color:#7fdc7f}
.err{color:#f88}
.row{display:flex;gap:8px;align-items:center}
.row select{flex:1}
.row button{width:auto;margin-top:0}
</style></head><body>
<h1>rsvpnano Wi-Fi setup</h1>
<p class=muted>Pick a network, enter the password, and hit Save. The device tests the credentials before storing them.</p>
<form id=f>
<label>Network</label>
<div class=row><select id=ssid></select><button type=button onclick=rescan()>Rescan</button></div>
<label>Password</label><input id=pass type=password autocomplete=off>
<button type=submit>Save & test</button>
</form>
<div id=status>Loading networks…</div>
<script>
async function rescan(){
  document.getElementById('status').textContent='Scanning…';
  const r=await fetch('/scan').then(r=>r.json());
  const sel=document.getElementById('ssid');
  sel.innerHTML='';
  r.networks.forEach(n=>{
    const o=document.createElement('option');
    o.value=n.ssid;
    o.textContent=`${n.ssid}  (${n.rssi} dBm, ${n.enc})`;
    sel.appendChild(o);
  });
  document.getElementById('status').textContent=`${r.networks.length} networks found.`;
}
document.getElementById('f').addEventListener('submit',async e=>{
  e.preventDefault();
  const ssid=document.getElementById('ssid').value;
  const pass=document.getElementById('pass').value;
  document.getElementById('status').textContent='Saving…';
  await fetch('/save',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:`ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}`});
  poll();
});
async function poll(){
  const s=await fetch('/status').then(r=>r.json());
  const el=document.getElementById('status');
  if(s.state==='Connected'){el.innerHTML=`<span class=ok>Connected to ${s.ssid}.</span><br>You can disconnect now.`;return;}
  if(s.state==='Failed'){el.innerHTML=`<span class=err>Failed: ${s.error||'connection refused'}.</span> Try again.`;return;}
  if(s.state==='Testing'){el.textContent=`Testing ${s.ssid}… this can take up to 20 s.`;}
  else if(s.state==='Pending'){el.textContent='Saving credentials…';}
  else{el.textContent='Awaiting credentials.';}
  setTimeout(poll,1200);
}
rescan();poll();
</script></body></html>)HTML";

}  // namespace

bool WifiSetupPortal::begin(const String &apSsid) {
  end();
  apSsid_ = apSsid.isEmpty() ? String("rsvpnano-setup") : apSsid;
  // AP_STA mode so the soft-AP serves the portal while we can still attempt
  // a STA connection to test the new credentials without dropping clients.
  WiFi.mode(WIFI_AP_STA);
  // Open AP — the captive portal page is the only thing on this network,
  // so requiring a password just adds friction for the user.
  const bool apOk = WiFi.softAP(apSsid_.c_str());
  if (!apOk) {
    Serial.printf("[wifi-portal] softAP(%s) failed\n", apSsid_.c_str());
    return false;
  }
  delay(50);  // SDK needs a beat to assign the AP IP.
  IPAddress ip = WiFi.softAPIP();
  apIp_ = ip.toString();
  // DNS hijack: any host the client looks up resolves to us → captive-portal
  // sheet pops automatically on iOS / Android.
  dns_.setErrorReplyCode(DNSReplyCode::NoError);
  dnsActive_ = dns_.start(53, "*", ip);
  registerRoutes();
  server_.begin();
  running_ = true;
  state_ = State::Awaiting;
  lastError_ = "";
  pendingSsid_ = "";
  pendingPassword_ = "";
  testingSsid_ = "";
  Serial.printf("[wifi-portal] up ssid=%s ip=%s dns=%d\n", apSsid_.c_str(), apIp_.c_str(),
                dnsActive_ ? 1 : 0);
  return true;
}

void WifiSetupPortal::end() {
  if (!running_) return;
  server_.close();
  server_.stop();
  if (dnsActive_) {
    dns_.stop();
    dnsActive_ = false;
  }
  WiFi.softAPdisconnect(true);
  // Leave WIFI_STA intact so the App's normal network features keep working
  // after the portal exits.
  WiFi.mode(WIFI_STA);
  running_ = false;
  state_ = State::Idle;
  scans_.clear();
  Serial.println("[wifi-portal] down");
}

void WifiSetupPortal::tick(uint32_t nowMs) {
  if (!running_) return;
  if (dnsActive_) dns_.processNextRequest();
  server_.handleClient();
  (void)nowMs;
}

void WifiSetupPortal::registerRoutes() {
  // Serve the page for the root and every captive-portal probe URL so iOS,
  // Android, Windows, and macOS all reliably open the sheet without the user
  // having to type an IP.
  const auto sendPage = [this]() {
    server_.send(200, "text/html; charset=utf-8", renderPage());
  };
  server_.on("/", HTTP_GET, sendPage);
  server_.on("/index.html", HTTP_GET, sendPage);
  server_.on("/hotspot-detect.html", HTTP_GET, sendPage);   // iOS
  server_.on("/library/test/success.html", HTTP_GET, sendPage);
  server_.on("/generate_204", HTTP_GET, sendPage);          // Android
  server_.on("/gen_204", HTTP_GET, sendPage);
  server_.on("/ncsi.txt", HTTP_GET, sendPage);              // Windows
  server_.on("/connecttest.txt", HTTP_GET, sendPage);
  server_.on("/redirect", HTTP_GET, sendPage);
  server_.on("/canonical.html", HTTP_GET, sendPage);
  server_.onNotFound(sendPage);

  server_.on("/scan", HTTP_GET, [this]() {
    doScan();
    String body = "{\"networks\":[";
    for (size_t i = 0; i < scans_.size(); ++i) {
      if (i > 0) body += ",";
      body += "{\"ssid\":\"";
      // Escape inline — SSIDs can contain quotes and backslashes.
      for (size_t j = 0; j < scans_[i].ssid.length(); ++j) {
        const char c = scans_[i].ssid[j];
        if (c == '"' || c == '\\') body += '\\';
        body += c;
      }
      body += "\",\"rssi\":";
      body += String(static_cast<int>(scans_[i].rssi));
      body += ",\"enc\":\"";
      body += encTag(scans_[i].enc);
      body += "\"}";
    }
    body += "]}";
    server_.send(200, "application/json", body);
  });

  server_.on("/status", HTTP_GET, [this]() {
    server_.send(200, "application/json", renderStatusJson());
  });

  server_.on("/save", HTTP_POST, [this]() {
    if (!server_.hasArg("ssid")) {
      server_.send(400, "text/plain", "missing ssid");
      return;
    }
    pendingSsid_ = server_.arg("ssid");
    pendingPassword_ = server_.hasArg("pass") ? server_.arg("pass") : String("");
    state_ = State::Pending;
    lastError_ = "";
    Serial.printf("[wifi-portal] /save ssid=%s passLen=%u\n", pendingSsid_.c_str(),
                  static_cast<unsigned>(pendingPassword_.length()));
    server_.send(202, "text/plain", "accepted");
  });
}

void WifiSetupPortal::doScan() {
  const uint32_t now = millis();
  if (now - lastScanMs_ < kScanCacheMs && !scans_.empty()) return;
  // WiFi.scanNetworks blocks for ~2-3 s. That's acceptable here — the user
  // is sitting on the portal page, not staring at the render loop.
  Serial.println("[wifi-portal] scan…");
  const int n = WiFi.scanNetworks(false /*async*/, false /*show_hidden*/);
  scans_.clear();
  if (n <= 0) {
    Serial.printf("[wifi-portal] scan empty (%d)\n", n);
    lastScanMs_ = now;
    return;
  }
  scans_.reserve(static_cast<size_t>(n));
  for (int i = 0; i < n; ++i) {
    ScanResult r;
    r.ssid = WiFi.SSID(i);
    r.rssi = static_cast<int8_t>(WiFi.RSSI(i));
    r.enc = static_cast<uint8_t>(WiFi.encryptionType(i));
    if (!r.ssid.isEmpty()) scans_.push_back(r);
  }
  // Strongest first.
  std::sort(scans_.begin(), scans_.end(),
            [](const ScanResult &a, const ScanResult &b) { return a.rssi > b.rssi; });
  // De-duplicate by SSID (multiple APs broadcasting same name).
  std::vector<ScanResult> dedup;
  dedup.reserve(scans_.size());
  for (const auto &r : scans_) {
    bool seen = false;
    for (const auto &k : dedup) {
      if (k.ssid == r.ssid) {
        seen = true;
        break;
      }
    }
    if (!seen) dedup.push_back(r);
  }
  scans_.swap(dedup);
  WiFi.scanDelete();
  lastScanMs_ = now;
  Serial.printf("[wifi-portal] scan done count=%u\n", static_cast<unsigned>(scans_.size()));
}

String WifiSetupPortal::renderPage() {
  // FPSTR streams a flash-resident literal so we don't double-allocate.
  return String(FPSTR(kPageTemplate));
}

String WifiSetupPortal::renderStatusJson() {
  const char *name = "Awaiting";
  switch (state_) {
    case State::Idle:      name = "Idle"; break;
    case State::Awaiting:  name = "Awaiting"; break;
    case State::Pending:   name = "Pending"; break;
    case State::Testing:   name = "Testing"; break;
    case State::Connected: name = "Connected"; break;
    case State::Failed:    name = "Failed"; break;
  }
  String body = "{\"state\":\"";
  body += name;
  body += "\",\"ssid\":\"";
  const String &which = (state_ == State::Testing) ? testingSsid_ : pendingSsid_;
  for (size_t i = 0; i < which.length(); ++i) {
    const char c = which[i];
    if (c == '"' || c == '\\') body += '\\';
    body += c;
  }
  body += "\",\"error\":\"";
  for (size_t i = 0; i < lastError_.length(); ++i) {
    const char c = lastError_[i];
    if (c == '"' || c == '\\') body += '\\';
    body += c;
  }
  body += "\"}";
  return body;
}
