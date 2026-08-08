#include "echoform/EchoformAudio.h"

#include <algorithm>

#include "audio/AudioManager.h"

namespace {

// 256-sample / 16 ms blocks: the cadence the VAD (M4) will consume.
constexpr size_t kMicBlockSamples = 256;
constexpr uint32_t kMicReadTimeoutMs = 200;
constexpr uint32_t kTaskStackBytes = 4096;
constexpr BaseType_t kAudioCore = 0;
// Speech writes go out in 256-sample chunks; i2s_write blocks on DMA space,
// which paces the task naturally.
constexpr size_t kSpeechChunkSamples = 256;

}  // namespace

void EchoformAudio::begin(AudioManager *audio) {
  audio_ = audio;
  xTaskCreatePinnedToCore(&EchoformAudio::speechTaskTrampoline, "echo_speech",
                          kTaskStackBytes, this, tskIDLE_PRIORITY + 2, nullptr,
                          kAudioCore);
}

bool EchoformAudio::startCapture() {
  if (capturing_.load()) return true;
  if (speechBusy()) return false;  // half duplex: never capture over speech
  if (!audio_->beginMicCapture()) return false;
  txHead_.store(0);
  txTail_.store(0);
  capturing_.store(true);
  if (!micTaskAlive_.exchange(true)) {
    if (xTaskCreatePinnedToCore(&EchoformAudio::micTaskTrampoline, "echo_mic",
                                kTaskStackBytes, this, tskIDLE_PRIORITY + 3,
                                nullptr, kAudioCore) != pdPASS) {
      micTaskAlive_.store(false);
      capturing_.store(false);
      audio_->endMicCapture();
      return false;
    }
  }
  return true;
}

void EchoformAudio::captureTeardown() {
  capturing_.store(false);
  // The mic task exits on capturing_ false; wait for it so the I2S driver
  // swap below never races an in-flight i2s_read.
  while (micTaskAlive_.load()) {
    vTaskDelay(pdMS_TO_TICKS(5));
  }
  audio_->endMicCapture();
}

void EchoformAudio::scopeFeed(const int16_t *samples, size_t count,
                              size_t stride) {
  // Decimate by 4: 16 kHz -> 4 kHz scope rate, so the 216-point window
  // spans ~54 ms — enough cycles of voice to read as a waveform.
  const size_t fresh = count / 4;
  if (fresh == 0) return;
  const size_t keep = fresh >= kScopePoints ? 0 : kScopePoints - fresh;
  memmove(scope_, scope_ + (kScopePoints - keep), keep * sizeof(int16_t));
  for (size_t i = 0; i < kScopePoints - keep; ++i) {
    scope_[keep + i] = samples[(i * 4) * stride];
  }
  scopeStampMs_.store(millis());
}

bool EchoformAudio::scopeSnapshot(int16_t *out) const {
  memcpy(out, scope_, sizeof(scope_));
  return millis() - scopeStampMs_.load() < 150;
}

void EchoformAudio::publishFeatures(const echoform::AudioFeatures &f,
                                    bool voice) {
  featRms_.store(f.rms);
  featTransient_.store(f.transient);
  featLow_.store(f.low);
  featMid_.store(f.mid);
  featHigh_.store(f.high);
  featVoice_.store(voice);
  featStampMs_.store(millis());
}

bool EchoformAudio::featuresSnapshot(LiveFeatures &out) const {
  out.rms = featRms_.load();
  out.transient = featTransient_.load();
  out.low = featLow_.load();
  out.mid = featMid_.load();
  out.high = featHigh_.load();
  out.voice = featVoice_.load();
  return millis() - featStampMs_.load() < 150;
}

void EchoformAudio::micTaskTrampoline(void *arg) {
  static_cast<EchoformAudio *>(arg)->micTaskRun();
  vTaskDelete(nullptr);
}

void EchoformAudio::micTaskRun() {
  // Interleaved stereo (TDM slots 0+2 folded by readMicSamples); we keep
  // slot 0 - the primary mic - as the mono utterance stream.
  int16_t stereo[kMicBlockSamples * 2];
  int16_t mono[kMicBlockSamples];
  while (capturing_.load()) {
    const size_t got =
        audio_->readMicSamples(stereo, kMicBlockSamples * 2, kMicReadTimeoutMs);
    if (got == 0) continue;
    const size_t frames = got / 2;
    for (size_t i = 0; i < frames; ++i) {
      // 2x digital gain with clipping: at conversational distance the
      // analog chain alone leaves whisper in hallucination territory.
      int32_t boosted = static_cast<int32_t>(stereo[i * 2]) * 2;
      if (boosted > INT16_MAX) boosted = INT16_MAX;
      if (boosted < INT16_MIN) boosted = INT16_MIN;
      mono[i] = static_cast<int16_t>(boosted);
    }
    const size_t head = txHead_.load(std::memory_order_relaxed);
    const size_t tail = txTail_.load(std::memory_order_acquire);
    const size_t used = head - tail;
    const size_t space = kTxRingSamples - used;
    const size_t take = std::min(frames, space);  // overflow drops newest
    for (size_t i = 0; i < take; ++i) {
      txRing_[(head + i) % kTxRingSamples] = mono[i];
    }
    txHead_.store(head + take, std::memory_order_release);
    // Visuals: scope window + block features from the mono stream.
    scopeFeed(mono, frames, 1);
    publishFeatures(micExtractor_.process(mono, frames),
                    micExtractor_.voiceActive());
  }
  micTaskAlive_.store(false);
}

size_t EchoformAudio::txDrain(int16_t *out, size_t maxSamples) {
  const size_t head = txHead_.load(std::memory_order_acquire);
  const size_t tail = txTail_.load(std::memory_order_relaxed);
  const size_t available = head - tail;
  const size_t take = std::min(available, maxSamples);
  for (size_t i = 0; i < take; ++i) {
    out[i] = txRing_[(tail + i) % kTxRingSamples];
  }
  txTail_.store(tail + take, std::memory_order_release);
  return take;
}

void EchoformAudio::pushSpeech(const int16_t *samples, size_t count) {
  speechEnded_.store(false);
  speechPushMs_.store(millis());
  const size_t head = speechHead_.load(std::memory_order_relaxed);
  const size_t len = speechLen_.load(std::memory_order_acquire);
  const size_t space = kSpeechRingSamples - len;
  const size_t take = std::min(count, space);
  for (size_t i = 0; i < take; ++i) {
    speechRing_[(head + i) % kSpeechRingSamples] = samples[i];
  }
  speechHead_.store(head + take, std::memory_order_release);
  speechLen_.fetch_add(take, std::memory_order_release);
  if (take < count) {
    speechDropped_.fetch_add(static_cast<uint32_t>(count - take));
  }
}

void EchoformAudio::speechTaskTrampoline(void *arg) {
  static_cast<EchoformAudio *>(arg)->speechTaskRun();
  vTaskDelete(nullptr);
}

void EchoformAudio::speechTaskRun() {
  int16_t chunk[kSpeechChunkSamples];
  for (;;) {
    // Wait for the prime threshold (or stream end with a short tail).
    const size_t len = speechLen_.load(std::memory_order_acquire);
    // Stale tail: if the relay stopped pushing mid-reply (died, cancelled)
    // a sub-prime residue would otherwise wedge speechBusy() forever and
    // block push-to-talk. Flush anything older than 1.5 s.
    const bool stale = len > 0 && millis() - speechPushMs_.load() > 1500;
    const bool primed = len >= kSpeechPrimeSamples ||
                        (len > 0 && (speechEnded_.load() || stale));
    if (!primed || capturing_.load()) {
      speechPlaying_.store(false);
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    speechPlaying_.store(true);
    // Stream until the ring runs dry.
    for (;;) {
      const size_t avail = speechLen_.load(std::memory_order_acquire);
      if (avail == 0) break;
      const size_t take = std::min(avail, kSpeechChunkSamples);
      const size_t tail = speechTail_.load(std::memory_order_relaxed);
      for (size_t i = 0; i < take; ++i) {
        chunk[i] = speechRing_[(tail + i) % kSpeechRingSamples];
      }
      speechTail_.store(tail + take, std::memory_order_release);
      speechLen_.fetch_sub(take, std::memory_order_release);
      scopeFeed(chunk, take, 1);
      publishFeatures(speechExtractor_.process(chunk, take),
                      speechExtractor_.voiceActive());
      audio_->writeSpeech(chunk, take);
    }
    speechPlaying_.store(false);
  }
}
