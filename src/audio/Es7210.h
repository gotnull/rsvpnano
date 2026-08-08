#pragma once

#include <Arduino.h>

// Minimal ES7210 quad-mic ADC driver for Echoform capture (docs/ECHOFORM.md).
// Register sequences are transcribed from Espressif's esp_codec_dev es7210.c
// (the clean reference the Waveshare demo uses); slave-mode I2S path only,
// MIC1 + MIC3 selected — the pair the demo picks for plain stereo I2S on this
// board. The ESP32-S3 is I2S master, so no clock coefficient programming is
// needed here.
namespace Es7210 {

// True if the chip ACKs on the shared I2C bus (Wire1).
bool detect();

// Full bring-up: reset, slave mode, I2S 16-bit, MIC1+MIC3 at +30 dB PGA,
// ADC running. Call after the I2S clocks are up (the chip locks onto
// BCLK/LRCK from us). Returns false on any failed register write.
bool startCapture();

// Powers the mics and ADC back down. Safe to call when not started.
void stopCapture();

// Bench diagnostic (M0 level bug): writes test patterns to TIME_CONTROL0
// (0x09, harmless scratch) and reads them back two ways — repeated-start
// and stop-then-read — logging every step. Distinguishes "writes don't
// stick" from "readback path lies" from "wrong device ACKing".
void probeRegisterReadback();

// Bench diagnostic: raw register write, no verification. For behavioural
// probes (e.g. poking gain/mute mid-recording to test whether writes land).
bool debugWriteReg(uint8_t reg, uint8_t value);

}  // namespace Es7210
