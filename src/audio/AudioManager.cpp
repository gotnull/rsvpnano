#include "audio/AudioManager.h"

#include <Wire.h>
#include <driver/i2s.h>
#include <math.h>

#include "board/BoardConfig.h"

namespace {

constexpr uint32_t kSampleRateHz = 16000;
constexpr float kBaseAmplitude = 0.55f;  // scaled further by volumePercent_
constexpr size_t kSampleBatch = 256;
constexpr uint16_t kRtttlDefaultDuration = 4;
constexpr uint16_t kRtttlDefaultOctave = 6;
constexpr uint16_t kRtttlDefaultBpm = 63;

// MIDI-style frequency table for octave 4 (a4 = 440 Hz).
constexpr float kNoteFrequencyOct4[12] = {
    261.63f,  // c
    277.18f,  // c#
    293.66f,  // d
    311.13f,  // d#
    329.63f,  // e
    349.23f,  // f
    369.99f,  // f#
    392.00f,  // g
    415.30f,  // g#
    440.00f,  // a
    466.16f,  // a#
    493.88f,  // b
};

int noteIndexForChar(char c) {
  switch (c) {
    case 'c': return 0;
    case 'd': return 2;
    case 'e': return 4;
    case 'f': return 5;
    case 'g': return 7;
    case 'a': return 9;
    case 'b': return 11;
    case 'h': return 11;  // some RTTTL dialects
    default: return -1;
  }
}

// Tiny ES8311 register-init sequence sufficient for line-out playback at 16 kHz
// with MCLK driven externally by the ESP32-S3 I2S peripheral.
struct CodecRegister {
  uint8_t reg;
  uint8_t value;
};

constexpr CodecRegister kEs8311InitSequence[] = {
    {0x00, 0x1F}, {0x45, 0x00}, {0x01, 0x30}, {0x02, 0x10}, {0x03, 0x10},
    {0x16, 0x24}, {0x04, 0x10}, {0x05, 0x00}, {0x06, 0x03}, {0x07, 0x00},
    {0x08, 0xFF}, {0x09, 0x0C}, {0x0A, 0x0C}, {0x0B, 0x00}, {0x0C, 0x00},
    {0x10, 0x1F}, {0x11, 0x7F}, {0x00, 0x80}, {0x0D, 0x01}, {0x01, 0x3F},
    {0x14, 0x1A}, {0x12, 0x00}, {0x13, 0x10}, {0x0E, 0x02}, {0x0F, 0x44},
    {0x15, 0x40}, {0x1B, 0x0A}, {0x1C, 0x6A}, {0x37, 0x08}, {0x44, 0x08},
    {0x32, 0xC0},  // ADC vol
    {0x31, 0x00},  // DAC mute off
    {0x17, 0xC0},  // DAC vol — full scale (we control loudness via amplitude)
};

constexpr i2s_port_t kI2sPort = I2S_NUM_0;
bool s_i2sInstalled = false;

}  // namespace

bool AudioManager::writeCodecRegister(uint8_t reg, uint8_t value) {
  Wire1.beginTransmission(BoardConfig::ES8311_I2C_ADDR);
  Wire1.write(reg);
  Wire1.write(value);
  return Wire1.endTransmission() == 0;
}

bool AudioManager::initCodec() {
  // Wire1 is already initialized by BoardConfig::begin() for the shared 47/48
  // bus (TCA9554 IO expander + ES8311 codec live on it).
  Wire1.beginTransmission(BoardConfig::ES8311_I2C_ADDR);
  if (Wire1.endTransmission() != 0) {
    Serial.println("[audio] ES8311 not detected on I2C bus 1");
    return false;
  }
  for (const CodecRegister &reg : kEs8311InitSequence) {
    if (!writeCodecRegister(reg.reg, reg.value)) {
      Serial.printf("[audio] codec write 0x%02X failed\n", reg.reg);
      return false;
    }
    delayMicroseconds(80);
  }
  Serial.println("[audio] ES8311 init sequence written");
  return true;
}

bool AudioManager::initI2sChannel() {
  if (s_i2sInstalled) return true;
  i2s_config_t cfg = {};
  cfg.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate = kSampleRateHz;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = 0;
  cfg.dma_buf_count = 4;
  cfg.dma_buf_len = 256;
  cfg.use_apll = false;
  cfg.tx_desc_auto_clear = true;
  cfg.fixed_mclk = 0;
  cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
  cfg.bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT;
  if (i2s_driver_install(kI2sPort, &cfg, 0, nullptr) != ESP_OK) {
    Serial.println("[audio] i2s_driver_install failed");
    return false;
  }
  i2s_pin_config_t pins = {};
  pins.mck_io_num = BoardConfig::PIN_AUDIO_MCLK;
  pins.bck_io_num = BoardConfig::PIN_AUDIO_BCLK;
  pins.ws_io_num = BoardConfig::PIN_AUDIO_LRCK;
  pins.data_out_num = BoardConfig::PIN_AUDIO_DOUT;
  pins.data_in_num = I2S_PIN_NO_CHANGE;
  if (i2s_set_pin(kI2sPort, &pins) != ESP_OK) {
    Serial.println("[audio] i2s_set_pin failed");
    i2s_driver_uninstall(kI2sPort);
    return false;
  }
  i2s_zero_dma_buffer(kI2sPort);
  s_i2sInstalled = true;
  Serial.println("[audio] I2S TX ready");
  return true;
}

bool AudioManager::begin() {
  if (initialized_) return true;
  if (!initI2sChannel()) {
    return false;
  }
  if (!initCodec()) {
    return false;
  }
  initialized_ = true;
  return true;
}

void AudioManager::playToneSamples(uint16_t frequencyHz, uint16_t durationMs) {
  if (!s_i2sInstalled || durationMs == 0) return;
  if (frequencyHz == 0) {
    playSilence(durationMs);
    return;
  }
  const uint32_t totalSamples =
      static_cast<uint32_t>(kSampleRateHz) * durationMs / 1000U;
  const float twoPi = 6.2831853f;
  const float radPerSample = twoPi * frequencyHz / kSampleRateHz;
  const float volumeScale = static_cast<float>(volumePercent_) / 100.0f;
  const int16_t amplitude = static_cast<int16_t>(kBaseAmplitude * volumeScale * INT16_MAX);

  int16_t buffer[kSampleBatch];
  uint32_t produced = 0;
  float phase = 0.0f;
  while (produced < totalSamples) {
    const uint32_t batch = std::min<uint32_t>(kSampleBatch, totalSamples - produced);
    for (uint32_t i = 0; i < batch; ++i) {
      buffer[i] = static_cast<int16_t>(sinf(phase) * amplitude);
      phase += radPerSample;
      if (phase > twoPi) phase -= twoPi;
    }
    size_t written = 0;
    i2s_write(kI2sPort, buffer, batch * sizeof(int16_t), &written, pdMS_TO_TICKS(200));
    produced += batch;
  }
}

void AudioManager::playSilence(uint16_t durationMs) {
  if (!s_i2sInstalled || durationMs == 0) return;
  const uint32_t totalSamples =
      static_cast<uint32_t>(kSampleRateHz) * durationMs / 1000U;
  int16_t silence[kSampleBatch] = {0};
  uint32_t remaining = totalSamples;
  while (remaining > 0) {
    const uint32_t batch = std::min<uint32_t>(kSampleBatch, remaining);
    size_t written = 0;
    i2s_write(kI2sPort, silence, batch * sizeof(int16_t), &written, pdMS_TO_TICKS(200));
    remaining -= batch;
  }
}

void AudioManager::setVolumePercent(uint8_t percent) {
  if (percent > 100) percent = 100;
  volumePercent_ = percent;
}

namespace {

// Strip whitespace and lowercase; modifies in place.
String normalizeRtttl(const String &input) {
  String out;
  out.reserve(input.length());
  for (size_t i = 0; i < input.length(); ++i) {
    const char c = input[i];
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
    out += static_cast<char>(tolower(static_cast<unsigned char>(c)));
  }
  return out;
}

float frequencyForNote(int noteIdx, int octave) {
  if (noteIdx < 0) return 0.0f;
  const float octaveScale = powf(2.0f, static_cast<float>(octave) - 4.0f);
  return kNoteFrequencyOct4[noteIdx] * octaveScale;
}

}  // namespace

bool AudioManager::playRtttl(const String &melodyIn) {
  if (!initialized_ && !begin()) return false;
  const String melody = normalizeRtttl(melodyIn);
  if (melody.isEmpty()) return false;

  // Format: name:defaults:notes
  const int firstColon = melody.indexOf(':');
  if (firstColon < 0) return false;
  const int secondColon = melody.indexOf(':', firstColon + 1);
  if (secondColon < 0) return false;

  const String defaults = melody.substring(firstColon + 1, secondColon);
  const String notesSection = melody.substring(secondColon + 1);

  uint16_t defaultDuration = kRtttlDefaultDuration;
  uint16_t defaultOctave = kRtttlDefaultOctave;
  uint16_t bpm = kRtttlDefaultBpm;
  {
    int start = 0;
    while (start < static_cast<int>(defaults.length())) {
      int comma = defaults.indexOf(',', start);
      if (comma < 0) comma = defaults.length();
      const String token = defaults.substring(start, comma);
      const int eq = token.indexOf('=');
      if (eq > 0) {
        const String key = token.substring(0, eq);
        const String value = token.substring(eq + 1);
        if (key == "d") defaultDuration = static_cast<uint16_t>(value.toInt());
        else if (key == "o") defaultOctave = static_cast<uint16_t>(value.toInt());
        else if (key == "b") bpm = static_cast<uint16_t>(value.toInt());
      }
      start = comma + 1;
    }
  }
  if (bpm == 0) bpm = kRtttlDefaultBpm;
  if (defaultDuration == 0) defaultDuration = kRtttlDefaultDuration;
  if (defaultOctave == 0) defaultOctave = kRtttlDefaultOctave;

  // Whole-note duration in ms = 4 * (60000/bpm).
  const uint32_t wholeNoteMs = (240000UL / bpm);

  int cursor = 0;
  while (cursor < static_cast<int>(notesSection.length())) {
    int comma = notesSection.indexOf(',', cursor);
    if (comma < 0) comma = notesSection.length();
    String note = notesSection.substring(cursor, comma);
    cursor = comma + 1;
    note.trim();
    if (note.isEmpty()) continue;

    int idx = 0;
    // Optional duration prefix (digits)
    uint16_t duration = defaultDuration;
    if (idx < static_cast<int>(note.length()) && isdigit(note[idx])) {
      int end = idx;
      while (end < static_cast<int>(note.length()) && isdigit(note[end])) ++end;
      duration = static_cast<uint16_t>(note.substring(idx, end).toInt());
      idx = end;
      if (duration == 0) duration = defaultDuration;
    }
    if (idx >= static_cast<int>(note.length())) continue;
    const char pitchChar = note[idx++];
    int noteIdx = (pitchChar == 'p') ? -2 : noteIndexForChar(pitchChar);
    // Sharp
    if (idx < static_cast<int>(note.length()) && note[idx] == '#') {
      ++noteIdx;
      ++idx;
    }
    // Dotted (extends 1.5x). The dot can appear before or after the octave digit.
    bool dotted = false;
    while (idx < static_cast<int>(note.length()) && note[idx] == '.') {
      dotted = true;
      ++idx;
    }
    // Octave
    uint16_t octave = defaultOctave;
    if (idx < static_cast<int>(note.length()) && isdigit(note[idx])) {
      octave = static_cast<uint16_t>(note[idx] - '0');
      ++idx;
    }
    while (idx < static_cast<int>(note.length()) && note[idx] == '.') {
      dotted = true;
      ++idx;
    }

    uint32_t durationMs = wholeNoteMs / std::max<uint16_t>(1, duration);
    if (dotted) durationMs = (durationMs * 3) / 2;

    if (noteIdx == -2) {
      playSilence(static_cast<uint16_t>(std::min<uint32_t>(durationMs, UINT16_MAX)));
    } else if (noteIdx >= 0 && noteIdx < 12) {
      const float freq = frequencyForNote(noteIdx, static_cast<int>(octave));
      // Reserve ~10 % at the end as silence so consecutive notes feel detached.
      const uint16_t toneMs =
          static_cast<uint16_t>(std::min<uint32_t>(durationMs * 9 / 10, UINT16_MAX));
      const uint16_t restMs =
          static_cast<uint16_t>(std::min<uint32_t>(durationMs - toneMs, UINT16_MAX));
      playToneSamples(static_cast<uint16_t>(freq + 0.5f), toneMs);
      if (restMs > 0) playSilence(restMs);
    }
  }
  return true;
}
