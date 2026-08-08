#include "audio/MicRecorder.h"

#include <SD_MMC.h>

#include <algorithm>

#include "audio/AudioManager.h"
#include "audio/Es7210.h"

namespace {

constexpr uint32_t kSampleRateHz = 16000;
// Matches the 256-sample / 16 ms block the Echoform VAD will consume, so this
// harness exercises the exact read cadence the real mic task will use.
constexpr size_t kBlockSamples = 256;
constexpr uint32_t kReadTimeoutMs = 200;
// Consecutive empty reads before we give up — the clock is ours (I2S master),
// so silence here means the RX path is broken, not that the room is quiet.
constexpr int kMaxConsecutiveTimeouts = 10;
constexpr size_t kWavHeaderBytes = 44;

// Recorder task: SD writes + blocking i2s_read, so keep it off the UI core.
constexpr BaseType_t kTaskCore = 0;
constexpr uint32_t kTaskStackBytes = 8192;

void writeLE16(uint8_t *p, uint16_t v) {
  p[0] = v & 0xFF;
  p[1] = v >> 8;
}

void writeLE32(uint8_t *p, uint32_t v) {
  p[0] = v & 0xFF;
  p[1] = (v >> 8) & 0xFF;
  p[2] = (v >> 16) & 0xFF;
  p[3] = (v >> 24) & 0xFF;
}

// M0 bring-up records STEREO — both I2S slots — because we don't yet know
// which slot the live mic pair lands on. The per-channel peak logs and the
// WAV's two channels identify it; the real Echoform mic task will go back
// to mono once the slot is known.
constexpr uint16_t kChannels = 2;

// Bench diagnostic: record the raw 32-bit I2S slot words verbatim instead of
// folded PCM16, so the host can inspect where in each slot the ADC's bits
// actually live. The file is not a playable WAV in this mode. Leave false
// for normal recordings (stereo PCM16: the two live mics, TDM slots 0+2).
constexpr bool kRawSlotCapture = false;

void buildWavHeader(uint8_t *h, uint32_t dataBytes) {
  memcpy(h, "RIFF", 4);
  writeLE32(h + 4, 36 + dataBytes);
  memcpy(h + 8, "WAVE", 4);
  memcpy(h + 12, "fmt ", 4);
  writeLE32(h + 16, 16);        // fmt chunk size
  writeLE16(h + 20, 1);         // PCM
  writeLE16(h + 22, kChannels);
  writeLE32(h + 24, kSampleRateHz);
  writeLE32(h + 28, kSampleRateHz * 2 * kChannels);  // byte rate
  writeLE16(h + 32, 2 * kChannels);                  // block align
  writeLE16(h + 34, 16);        // bits per sample
  memcpy(h + 36, "data", 4);
  writeLE32(h + 40, dataBytes);
}

}  // namespace

bool MicRecorder::start(AudioManager &audio, const String &path, uint32_t seconds) {
  if (recording_.exchange(true)) {
    Serial.println("[rec] already recording");
    return false;
  }
  audio_ = &audio;
  path_ = path;
  seconds_ = seconds;
  const BaseType_t ok = xTaskCreatePinnedToCore(
      &MicRecorder::taskTrampoline, "mic_rec", kTaskStackBytes, this,
      tskIDLE_PRIORITY + 2, nullptr, kTaskCore);
  if (ok != pdPASS) {
    Serial.println("[rec] xTaskCreate FAILED");
    recording_.store(false);
    return false;
  }
  return true;
}

void MicRecorder::taskTrampoline(void *arg) {
  static_cast<MicRecorder *>(arg)->taskRun();
  vTaskDelete(nullptr);
}

void MicRecorder::taskRun() {
  // Interleaved sample count: frames * channels.
  const uint32_t totalSamples = kSampleRateHz * seconds_ * kChannels;
  bool ok = false;
  uint32_t captured = 0;

  Serial.printf("[rec] recording %lu s to %s\n",
                static_cast<unsigned long>(seconds_), path_.c_str());

  if (!audio_->beginMicCapture()) {
    Serial.println("[rec] mic capture bring-up failed");
  } else {
    File f = SD_MMC.open(path_, FILE_WRITE);
    if (!f) {
      Serial.printf("[rec] open failed: %s\n", path_.c_str());
    } else {
      uint8_t header[kWavHeaderBytes];
      buildWavHeader(header, totalSamples * sizeof(int16_t));
      f.write(header, sizeof(header));

      int16_t block[kBlockSamples * kChannels];
      int32_t rawBlock[kBlockSamples * kChannels];
      int consecutiveTimeouts = 0;
      int16_t secondPeak[kChannels] = {0};
      uint32_t nextLogSample = kSampleRateHz * kChannels;

      while (captured < totalSamples) {
        const size_t want =
            std::min<size_t>(kBlockSamples * kChannels, totalSamples - captured);
        size_t got = 0;
        if (kRawSlotCapture) {
          got = audio_->readMicRaw(rawBlock, want, kReadTimeoutMs);
        } else {
          got = audio_->readMicSamples(block, want, kReadTimeoutMs);
        }
        if (got == 0) {
          if (++consecutiveTimeouts >= kMaxConsecutiveTimeouts) {
            Serial.println("[rec] i2s_read starved — aborting");
            break;
          }
          continue;
        }
        consecutiveTimeouts = 0;
        if (kRawSlotCapture) {
          f.write(reinterpret_cast<uint8_t *>(rawBlock), got * sizeof(int32_t));
          for (size_t i = 0; i < got; ++i) {
            const int16_t top = static_cast<int16_t>(rawBlock[i] >> 16);
            const int16_t mag = top < 0 ? -top : top;
            const size_t ch = i % kChannels;
            if (mag > secondPeak[ch]) secondPeak[ch] = mag;
          }
        } else {
          f.write(reinterpret_cast<uint8_t *>(block), got * sizeof(int16_t));
          for (size_t i = 0; i < got; ++i) {
            const int16_t mag = block[i] < 0 ? -block[i] : block[i];
            const size_t ch = i % kChannels;
            if (mag > secondPeak[ch]) secondPeak[ch] = mag;
          }
        }
        captured += got;
        if (captured >= nextLogSample) {
          const uint32_t second =
              captured / (kSampleRateHz * kChannels);
          Serial.printf("[rec] %lus peak ch0=%d ch1=%d\n",
                        static_cast<unsigned long>(second), secondPeak[0],
                        secondPeak[1]);
          secondPeak[0] = 0;
          secondPeak[1] = 0;
          nextLogSample += kSampleRateHz * kChannels;
        }
      }

      if (captured < totalSamples) {
        // Short recording — patch the header so the file still parses.
        buildWavHeader(header, captured * sizeof(int16_t));
        f.seek(0);
        f.write(header, sizeof(header));
      }
      f.close();
      ok = captured > 0;
    }
    audio_->endMicCapture();
  }

  Serial.printf("[rec] done: %lu/%lu samples, %s\n",
                static_cast<unsigned long>(captured),
                static_cast<unsigned long>(totalSamples),
                ok ? "OK" : "FAILED");
  success_.store(ok);
  finishedOneShot_.store(true);
  recording_.store(false);
}
