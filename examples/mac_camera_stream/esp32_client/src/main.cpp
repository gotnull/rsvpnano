#include <Arduino.h>
#include <HTTPClient.h>
#include <JPEGDecoder.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

// ---- Required configuration -------------------------------------------------
// Keep these simple for the first test. Use your Mac's LAN IP, not localhost.
static const char *WIFI_SSID = "YOUR_WIFI_SSID";
static const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
static const char *SERVER_HOST = "192.168.1.100";
static const uint16_t SERVER_PORT = 8080;
static const char *SNAPSHOT_PATH = "/snapshot.jpg";

// Snapshot polling is easier to recover from than a long-lived MJPEG stream.
static const uint32_t FRAME_INTERVAL_MS = 150;      // ~6.6 FPS request target
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
static const uint32_t HTTP_TIMEOUT_MS = 5000;
static const size_t MAX_JPEG_BYTES = 80 * 1024;     // 320x240 q50-60 is usually well below this

// Set to 1 and implement renderJpegBlock() for a real TFT/display.
static const bool ENABLE_DISPLAY_HOOK = false;

static uint8_t *jpegBuffer = nullptr;
static size_t jpegCapacity = 0;
static uint32_t framesOk = 0;
static uint32_t framesFailed = 0;
static uint32_t lastFpsReportMs = 0;
static uint32_t lastFrameMs = 0;

static String snapshotUrl() {
  return String("http://") + SERVER_HOST + ":" + String(SERVER_PORT) + SNAPSHOT_PATH;
}

static uint32_t freeHeap8() {
  return heap_caps_get_free_size(MALLOC_CAP_8BIT);
}

static uint32_t freePsram() {
#if BOARD_HAS_PSRAM
  return heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
#else
  return 0;
#endif
}

static bool ensureJpegBuffer(size_t bytes) {
  if (bytes == 0 || bytes > MAX_JPEG_BYTES) {
    Serial.printf("[frame] refusing jpeg size=%u max=%u\n", static_cast<unsigned>(bytes),
                  static_cast<unsigned>(MAX_JPEG_BYTES));
    return false;
  }
  if (jpegBuffer != nullptr && jpegCapacity >= bytes) return true;

  if (jpegBuffer != nullptr) {
    heap_caps_free(jpegBuffer);
    jpegBuffer = nullptr;
    jpegCapacity = 0;
  }

#if BOARD_HAS_PSRAM
  jpegBuffer = static_cast<uint8_t *>(heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
#endif
  if (jpegBuffer == nullptr) {
    jpegBuffer = static_cast<uint8_t *>(heap_caps_malloc(bytes, MALLOC_CAP_8BIT));
  }

  if (jpegBuffer == nullptr) {
    Serial.printf("[heap] failed to allocate jpeg buffer bytes=%u free8=%lu psram=%lu\n",
                  static_cast<unsigned>(bytes), static_cast<unsigned long>(freeHeap8()),
                  static_cast<unsigned long>(freePsram()));
    return false;
  }

  jpegCapacity = bytes;
  return true;
}

static void connectWifi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.printf("[wifi] connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[wifi] connected ip=%s rssi=%d\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    Serial.println("[wifi] connect timeout");
  }
}

// Display integration point.
// JPEGDecoder exposes decoded MCU blocks in JpegDec.pImage. Real display code
// should clip (x, y, w, h) and push the RGB565 block to the panel.
static void renderJpegBlock(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t *pixels) {
  (void)x;
  (void)y;
  (void)w;
  (void)h;
  (void)pixels;
  if (ENABLE_DISPLAY_HOOK) {
    // Example shape for TFT_eSPI-like libraries:
    // tft.pushImage(x, y, w, h, pixels);
  }
}

static bool decodeAndRenderJpeg(const uint8_t *data, size_t length) {
  const uint32_t heapBefore = freeHeap8();
  const uint32_t psramBefore = freePsram();

  if (!JpegDec.decodeArray(const_cast<uint8_t *>(data), static_cast<uint32_t>(length))) {
    Serial.println("[jpeg] decode failed");
    return false;
  }

  Serial.printf("[jpeg] image=%dx%d mcu=%dx%d comps=%d\n", JpegDec.width, JpegDec.height,
                JpegDec.MCUWidth, JpegDec.MCUHeight, JpegDec.comps);

  while (JpegDec.read()) {
    const int32_t x = JpegDec.MCUx * JpegDec.MCUWidth;
    const int32_t y = JpegDec.MCUy * JpegDec.MCUHeight;
    uint16_t w = JpegDec.MCUWidth;
    uint16_t h = JpegDec.MCUHeight;

    if (x + w > JpegDec.width) w = JpegDec.width - x;
    if (y + h > JpegDec.height) h = JpegDec.height - y;
    renderJpegBlock(x, y, w, h, JpegDec.pImage);
  }

  Serial.printf("[heap] before8=%lu after8=%lu beforepsram=%lu afterpsram=%lu\n",
                static_cast<unsigned long>(heapBefore), static_cast<unsigned long>(freeHeap8()),
                static_cast<unsigned long>(psramBefore), static_cast<unsigned long>(freePsram()));
  return true;
}

static bool fetchSnapshot() {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  const String url = snapshotUrl();
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setReuse(false);

  Serial.printf("[http] GET %s\n", url.c_str());
  if (!http.begin(url)) {
    Serial.println("[http] begin failed");
    return false;
  }

  const uint32_t requestStart = millis();
  const int status = http.GET();
  if (status != HTTP_CODE_OK) {
    Serial.printf("[http] status=%d error=%s\n", status, http.errorToString(status).c_str());
    http.end();
    return false;
  }

  const int contentLength = http.getSize();
  Serial.printf("[http] status=200 content_length=%d\n", contentLength);
  if (contentLength <= 0 || static_cast<size_t>(contentLength) > MAX_JPEG_BYTES) {
    Serial.println("[http] invalid content length");
    http.end();
    return false;
  }

  if (!ensureJpegBuffer(static_cast<size_t>(contentLength))) {
    http.end();
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t offset = 0;
  uint32_t lastByteMs = millis();
  while (offset < static_cast<size_t>(contentLength)) {
    const int available = stream->available();
    if (available > 0) {
      const size_t want = min(static_cast<size_t>(available), static_cast<size_t>(contentLength) - offset);
      const int got = stream->readBytes(jpegBuffer + offset, want);
      if (got > 0) {
        offset += static_cast<size_t>(got);
        lastByteMs = millis();
      }
    } else {
      if (millis() - lastByteMs > HTTP_TIMEOUT_MS) {
        Serial.printf("[http] read timeout bytes=%u/%d\n", static_cast<unsigned>(offset), contentLength);
        http.end();
        return false;
      }
      delay(1);
    }
  }
  http.end();

  const bool ok = decodeAndRenderJpeg(jpegBuffer, offset);
  const uint32_t elapsed = millis() - requestStart;
  Serial.printf("[frame] bytes=%u decode=%s elapsed=%lu ms\n", static_cast<unsigned>(offset),
                ok ? "ok" : "fail", static_cast<unsigned long>(elapsed));
  return ok;
}

static void reportFps() {
  const uint32_t now = millis();
  if (lastFpsReportMs == 0) {
    lastFpsReportMs = now;
    return;
  }
  const uint32_t elapsed = now - lastFpsReportMs;
  if (elapsed < 5000) return;

  const float fps = (framesOk * 1000.0f) / elapsed;
  Serial.printf("[stats] ok=%lu failed=%lu approx_fps=%.2f rssi=%d free8=%lu psram=%lu\n",
                static_cast<unsigned long>(framesOk), static_cast<unsigned long>(framesFailed), fps,
                WiFi.RSSI(), static_cast<unsigned long>(freeHeap8()),
                static_cast<unsigned long>(freePsram()));
  framesOk = 0;
  framesFailed = 0;
  lastFpsReportMs = now;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("[boot] ESP32 Mac camera snapshot client");
  Serial.printf("[boot] server=%s max_jpeg=%u interval=%lu ms\n", snapshotUrl().c_str(),
                static_cast<unsigned>(MAX_JPEG_BYTES), static_cast<unsigned long>(FRAME_INTERVAL_MS));
  Serial.printf("[heap] free8=%lu psram=%lu\n", static_cast<unsigned long>(freeHeap8()),
                static_cast<unsigned long>(freePsram()));
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
    delay(1000);
    return;
  }

  const uint32_t now = millis();
  if (now - lastFrameMs >= FRAME_INTERVAL_MS) {
    lastFrameMs = now;
    if (fetchSnapshot()) {
      framesOk++;
    } else {
      framesFailed++;
      delay(500);
    }
  }

  reportFps();
}
