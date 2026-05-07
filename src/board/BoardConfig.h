#pragma once

#include <Arduino.h>

namespace BoardConfig {

constexpr int PIN_BOOT_BUTTON = 0;
constexpr int PIN_PWR_BUTTON = 16;
constexpr int PIN_BATTERY_ADC = 4;

constexpr int PIN_LCD_CS = 9;
constexpr int PIN_LCD_SCLK = 10;
constexpr int PIN_LCD_DATA0 = 11;
constexpr int PIN_LCD_DATA1 = 12;
constexpr int PIN_LCD_DATA2 = 13;
constexpr int PIN_LCD_DATA3 = 14;
constexpr int PIN_LCD_RST = 21;
constexpr int PIN_LCD_BACKLIGHT = 8;

constexpr int PANEL_NATIVE_WIDTH = 172;
constexpr int PANEL_NATIVE_HEIGHT = 640;
constexpr int DISPLAY_WIDTH = 640;
constexpr int DISPLAY_HEIGHT = 172;
constexpr bool UI_ROTATED_180 = true;  // Keep BOOT/PWR at the top edge in landscape.

constexpr int PIN_SD_CLK = 41;
constexpr int PIN_SD_CMD = 39;
constexpr int PIN_SD_D0 = 40;
constexpr int PIN_I2C_SDA = 47;
constexpr int PIN_I2C_SCL = 48;
constexpr int PIN_AUDIO_MCLK = 7;
constexpr int PIN_AUDIO_BCLK = 15;
constexpr int PIN_AUDIO_LRCK = 46;
constexpr int PIN_AUDIO_DOUT = 45;
constexpr uint8_t ES8311_I2C_ADDR = 0x18;
constexpr int PIN_TOUCH_SDA = 17;
constexpr int PIN_TOUCH_SCL = 18;

constexpr int TCA9554_ADDRESS = 0x20;
constexpr uint8_t TCA9554_PIN_BATTERY_ADC_ENABLE = 1;
constexpr uint8_t TCA9554_PIN_SYS_EN = 6;
// NS4150B speaker amp CTRL pin (NS_MODE on the schematic). Must be driven
// HIGH to un-shutdown the amp; left floating, the chip is silent regardless of
// what the codec is putting out — which is exactly what was happening for us.
constexpr uint8_t TCA9554_PIN_AUDIO_AMP_EN = 7;

struct BatteryStatus {
  bool present = false;
  float voltage = 0.0f;
  uint8_t percent = 0;
};

void begin();
void lightSleepUntilBootButton();
bool readBatteryStatus(BatteryStatus &status);
bool releaseBatteryPowerHold();
// Set a TCA9554 IO pin as a driven output at the given level. Other pins are
// preserved (read-modify-write). Returns false if the I2C transaction failed.
bool configureTca9554OutputPin(uint8_t pin, bool high);

}  // namespace BoardConfig
