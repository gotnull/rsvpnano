#include "audio/ModPlayer.h"

#include <SD_MMC.h>
#include <driver/i2s.h>
#include <esp_heap_caps.h>

#include <libxmp-lite/xmp.h>

namespace {

// MOD/XM rendering is 44.1 kHz stereo 16-bit by libxmp default. AudioManager
// initialises I2S at 16 kHz mono — playFile() reconfigures the clock to
// match, stop() restores AudioManager's defaults so RTTTL stays sane.
constexpr uint32_t kRenderSampleRateHz = 44100;
constexpr i2s_port_t kI2sPort = I2S_NUM_0;

// Chunk size ≈ 23 ms of stereo audio. Small enough that ducking takes
// effect quickly; big enough to amortise the xmp_play_buffer call cost.
constexpr int kChunkSamples = 1024;          // per-channel
constexpr int kChunkBytes = kChunkSamples * 2 /* stereo */ * sizeof(int16_t);

constexpr UBaseType_t kAudioTaskStack = 6144;
constexpr UBaseType_t kAudioTaskPrio = 5;
constexpr BaseType_t kAudioTaskCore = 0;

constexpr size_t kMaxModuleBytes = 1024 * 1024;  // 1 MB cap — typical MOD/XM

}  // namespace

ModPlayer::~ModPlayer() {
  stop();
  if (taskHandle_) {
    vTaskDelete(taskHandle_);
    taskHandle_ = nullptr;
  }
  if (lock_) {
    vSemaphoreDelete(lock_);
    lock_ = nullptr;
  }
}

bool ModPlayer::begin() {
  if (initialised_) return true;
  lock_ = xSemaphoreCreateMutex();
  if (!lock_) {
    Serial.println("[mod] failed to create mutex");
    return false;
  }
  // Reserve a stereo render buffer in PSRAM for the task — kept on the
  // task's stack would blow our 6 KB allocation.
  if (xTaskCreatePinnedToCore(&ModPlayer::audioTaskTrampoline, "mod_audio",
                              kAudioTaskStack, this, kAudioTaskPrio,
                              &taskHandle_, kAudioTaskCore) != pdPASS) {
    Serial.println("[mod] failed to spawn audio task");
    vSemaphoreDelete(lock_);
    lock_ = nullptr;
    return false;
  }
  initialised_ = true;
  Serial.println("[mod] ModPlayer initialised (audio task on core 0)");
  return true;
}

bool ModPlayer::playFile(const String &path) {
  if (!initialised_ && !begin()) return false;

  // Stop anything currently playing before swapping modules.
  stop();

  File f = SD_MMC.open(path);
  if (!f || f.isDirectory()) {
    if (f) f.close();
    Serial.printf("[mod] open failed: %s\n", path.c_str());
    return false;
  }
  const size_t fileSize = f.size();
  if (fileSize == 0 || fileSize > kMaxModuleBytes) {
    Serial.printf("[mod] file size out of range (%u): %s\n",
                  static_cast<unsigned>(fileSize), path.c_str());
    f.close();
    return false;
  }

  // libxmp loads from a memory buffer; load the whole file into PSRAM so
  // the renderer doesn't pin SD I/O. Free in stop().
  uint8_t *buf = static_cast<uint8_t *>(
      heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (!buf) {
    buf = static_cast<uint8_t *>(heap_caps_malloc(fileSize, MALLOC_CAP_8BIT));
  }
  if (!buf) {
    Serial.printf("[mod] alloc failed for %u bytes\n",
                  static_cast<unsigned>(fileSize));
    f.close();
    return false;
  }
  const int read = f.read(buf, fileSize);
  f.close();
  if (read != static_cast<int>(fileSize)) {
    Serial.printf("[mod] short read: %d/%u\n", read,
                  static_cast<unsigned>(fileSize));
    heap_caps_free(buf);
    return false;
  }

  xmp_context ctx = xmp_create_context();
  if (!ctx) {
    Serial.println("[mod] xmp_create_context failed");
    heap_caps_free(buf);
    return false;
  }
  const int loadRc = xmp_load_module_from_memory(ctx, buf, fileSize);
  if (loadRc < 0) {
    Serial.printf("[mod] load failed rc=%d (%s)\n", loadRc, path.c_str());
    xmp_free_context(ctx);
    heap_caps_free(buf);
    return false;
  }
  // Stereo, 16-bit, 44.1 kHz — matches our I2S re-clock below.
  if (xmp_start_player(ctx, kRenderSampleRateHz, 0) < 0) {
    Serial.println("[mod] xmp_start_player failed");
    xmp_release_module(ctx);
    xmp_free_context(ctx);
    heap_caps_free(buf);
    return false;
  }

  // Reconfigure I2S for 44.1 kHz stereo. AudioManager's WAV path already
  // uses i2s_set_clk for the same purpose.
  if (i2s_set_clk(kI2sPort, kRenderSampleRateHz, I2S_BITS_PER_SAMPLE_16BIT,
                  I2S_CHANNEL_STEREO) != ESP_OK) {
    Serial.println("[mod] i2s_set_clk failed");
    xmp_end_player(ctx);
    xmp_release_module(ctx);
    xmp_free_context(ctx);
    heap_caps_free(buf);
    return false;
  }

  xSemaphoreTake(lock_, portMAX_DELAY);
  xmpCtx_ = ctx;
  fileBuf_ = buf;
  fileBufBytes_ = fileSize;
  // Capture basename for the now-playing UI.
  const int slash = path.lastIndexOf('/');
  currentTrack_ = (slash >= 0) ? path.substring(slash + 1) : path;
  duck_ = 100;
  stopRequested_ = false;
  running_ = true;
  xSemaphoreGive(lock_);

  // Wake the task — it polls running_ and starts rendering.
  xTaskNotifyGive(taskHandle_);
  Serial.printf("[mod] playing %s (%u bytes)\n", currentTrack_.c_str(),
                static_cast<unsigned>(fileSize));
  return true;
}

void ModPlayer::stop() {
  if (!initialised_) return;
  // Signal the task to stop; wait briefly for it to drain.
  stopRequested_ = true;
  // Give the task a chance to notice on its next chunk boundary (~23 ms).
  for (int i = 0; i < 10 && running_; ++i) {
    vTaskDelay(pdMS_TO_TICKS(20));
  }

  xSemaphoreTake(lock_, portMAX_DELAY);
  if (xmpCtx_) {
    xmp_end_player(static_cast<xmp_context>(xmpCtx_));
    xmp_release_module(static_cast<xmp_context>(xmpCtx_));
    xmp_free_context(static_cast<xmp_context>(xmpCtx_));
    xmpCtx_ = nullptr;
  }
  if (fileBuf_) {
    heap_caps_free(fileBuf_);
    fileBuf_ = nullptr;
    fileBufBytes_ = 0;
  }
  currentTrack_ = "";
  running_ = false;
  stopRequested_ = false;
  xSemaphoreGive(lock_);

  // Restore AudioManager's default I2S clock (16 kHz mono) so subsequent
  // RTTTL playback isn't pitched up.
  i2s_set_clk(kI2sPort, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void ModPlayer::setVolumePercent(uint8_t pct) {
  if (pct > 100) pct = 100;
  volume_ = pct;
}

void ModPlayer::setDuckPercent(uint8_t pct) {
  if (pct > 100) pct = 100;
  duck_ = pct;
}

void ModPlayer::getNowPlaying(NowPlaying &out) {
  out.valid = false;
  out.channelCount = 0;
  if (!initialised_) return;
  xSemaphoreTake(lock_, portMAX_DELAY);
  xmp_context ctx = static_cast<xmp_context>(xmpCtx_);
  if (ctx && running_) {
    xmp_frame_info fi;
    xmp_get_frame_info(ctx, &fi);
    xmp_module_info mi;
    xmp_get_module_info(ctx, &mi);
    out.valid = true;
    out.pos = fi.pos;
    out.row = fi.row;
    out.numRows = fi.num_rows;
    out.bpm = fi.bpm;
    out.speed = fi.speed;
    out.timeMs = fi.time;
    out.totalMs = fi.total_time;
    out.virtChannels = fi.virt_channels;
    out.virtUsed = fi.virt_used;
    // Take up to 32 channel volumes — enough for any tracker format. Some
    // XM tunes go up to 32; MOD is always 4. We clamp here so the bar
    // renderer always knows how many to draw.
    const int sourceCount = (mi.mod && mi.mod->chn > 0) ? mi.mod->chn : 0;
    const int bound = sourceCount < 32 ? sourceCount : 32;
    for (int i = 0; i < bound; ++i) {
      out.channelVolumes[i] = fi.channel_info[i].volume;
    }
    out.channelCount = static_cast<uint8_t>(bound);
    if (mi.mod) {
      strncpy(out.title, mi.mod->name, sizeof(out.title) - 1);
      strncpy(out.format, mi.mod->type, sizeof(out.format) - 1);
    }
  }
  xSemaphoreGive(lock_);
}

void ModPlayer::audioTaskTrampoline(void *arg) {
  static_cast<ModPlayer *>(arg)->audioTaskLoop();
}

void ModPlayer::audioTaskLoop() {
  // Stack budget is 6 KB; the chunk buffer at 4 KB lives in PSRAM via
  // heap_caps_malloc rather than as a task-local.
  int16_t *chunk = static_cast<int16_t *>(
      heap_caps_malloc(kChunkBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (!chunk) {
    chunk = static_cast<int16_t *>(heap_caps_malloc(kChunkBytes, MALLOC_CAP_8BIT));
  }
  if (!chunk) {
    Serial.println("[mod] task: failed to allocate chunk buffer");
    vTaskDelete(nullptr);
    return;
  }

  for (;;) {
    if (!running_) {
      // Idle until playFile() notifies us.
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      continue;
    }
    if (stopRequested_) {
      running_ = false;
      continue;
    }

    // Render the next chunk under the lock so stop() doesn't pull the rug.
    xSemaphoreTake(lock_, portMAX_DELAY);
    xmp_context ctx = static_cast<xmp_context>(xmpCtx_);
    int rc = -1;
    if (ctx) {
      rc = xmp_play_buffer(ctx, chunk, kChunkBytes, /*loop=*/0);
    }
    const int scale = (static_cast<int>(volume_) * static_cast<int>(duck_)) / 100;
    xSemaphoreGive(lock_);

    if (rc < 0) {
      // End of song or error — stop gracefully so the App can pick the
      // next track on its own schedule.
      running_ = false;
      Serial.printf("[mod] track ended (rc=%d)\n", rc);
      continue;
    }
    if (scale != 100) {
      const int sampleCount = kChunkSamples * 2;
      for (int i = 0; i < sampleCount; ++i) {
        chunk[i] = static_cast<int16_t>(
            (static_cast<int32_t>(chunk[i]) * scale) / 100);
      }
    }
    size_t written = 0;
    i2s_write(kI2sPort, chunk, kChunkBytes, &written, pdMS_TO_TICKS(500));
  }
}
