// Entry point for the standalone Echoform firmware (env:echoform).
// The reader OS's entry point is src/main.cpp; build filters keep the two
// out of each other's images.
#include <Arduino.h>
#include <esp_log.h>

#include "board/BoardConfig.h"
#include "echoform/EchoformApp.h"

EchoformApp app;

void setup() {
  Serial.begin(115200);
  // USB CDC writes block up to 250 ms per line when no host is draining the
  // port (USBCDC.cpp: tx_timeout_ms default). With per-second diagnostics
  // that reads as the starfield locking up every few seconds whenever the
  // device runs without a serial monitor attached. Zero = drop instantly.
  Serial.setTxTimeoutMs(0);
  esp_log_level_set("*", ESP_LOG_INFO);
  delay(50);
  BoardConfig::begin();
  const uint32_t serialWaitStart = millis();
  while (!Serial && millis() - serialWaitStart < 2000) {
    delay(10);
  }
  Serial.println("[main] echoform setup");
  app.begin();
}

void loop() {
  app.update(millis());
}
