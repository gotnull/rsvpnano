#pragma once

#include <Arduino.h>

#include "audio/AudioManager.h"
#include "audio/MicRecorder.h"
#include "board/BoardConfig.h"
#include "display/DisplayManager.h"
#include <Preferences.h>

#include "echoform/EchoParticles.h"
#include "echoform/EchoRender.h"
#include "echoform/EchoState.h"
#include "echoform/EchoWave.h"
#include "echoform/EchoformAudio.h"
#include "echoform/EchoformNet.h"
#include "input/ButtonHandler.h"
#include "screensaver/Screensaver.h"
#include "storage/StorageManager.h"

// Standalone Echoform firmware (docs/ECHOFORM.md). The screensaver IS the
// interface: boot lands directly in the starfield + bobs loop and never
// leaves it. No menus, no reader, no OS chrome — this class is the whole
// device. The reader OS's main-loop rules (App.cpp:1-62) bind update() the
// same way: coordinate only, slow work on workers, 33 ms tick guardrail.
class EchoformApp {
 public:
  void begin();
  void update(uint32_t nowMs);

 private:
  // TEMPORARY M0 harness: "REC" over serial records 5 s to /rec.wav.
  void pollMicRecorderDebug(uint32_t nowMs);
  // Push-to-talk (M2): hold BOOT to capture, release to send. "TALK n"
  // over serial runs a timed utterance for benchless testing.
  void updatePushToTalk(uint32_t nowMs);
  bool startUtterance();
  void finishUtterance();

  DisplayManager display_;
  Screensaver screensaver_;
  AudioManager audio_;
  MicRecorder micRecorder_;
  StorageManager storage_;
  EchoformAudio echoAudio_;
  EchoformNet net_;
  ButtonHandler bootButton_{BoardConfig::PIN_BOOT_BUTTON};

  // Rebuilds the overlay text + waveform feed a few times a second.
  void updateOverlay(uint32_t nowMs);
  // Boot-time OTA: if the latest published echoform.bin is NEWER than this
  // build (timestamp tags both sides), download and flash it with a
  // progress bar, then restart. Dev builds are always newest, so a fresh
  // USB flash is never clobbered.
  void maybeBootOta();
  // Voice intent: "set volume to N" / "full volume" / "mute" applied
  // on-device from the final transcript.
  void handleVoiceIntents();
  // M4 self-update safety net (docs/ECHOFORM.md): an NVS boot counter
  // catches crash loops; a boot that survives the self-test window records
  // its tag as last-known-good; a crash-looping or voice-rejected build is
  // reflashed from that tag's release, and the broken release stamp is
  // remembered so the boot OTA doesn't immediately reinstall it.
  void maybeRecover(uint8_t bootTries);
  bool flashReleaseTag(const String &tag);
  void markBootGoodIfDue(uint32_t nowMs);
  void updatePendingRestart(uint32_t nowMs);
  // Restart once the current exchange finishes (reply spoken), or at the
  // deadline. rollback additionally sets the NVS force-rollback flag.
  void scheduleRestart(const char *reason, bool rollback);
  static void otaStatusTrampoline(void *context, const char *title,
                                  const char *line1, const char *line2,
                                  int progressPercent);

  bool sdReady_ = false;
  uint32_t lastRenderMs_ = 0;
  bool pttActive_ = false;
  // Non-zero while a timed TALK utterance is running: end at this ms.
  uint32_t talkDeadlineMs_ = 0;

  DisplayManager::ScreensaverOverlay overlay_ = {};
  uint32_t lastOverlayMs_ = 0;
  uint32_t lastConversationMs_ = 0;

  // The rusty-nail visual system: wave physics + particles + the composed
  // 128x128 index field. Always alive (the living wave IS Echoform); the
  // screensaver runs normally underneath.
  echoform::EchoWave wave_;
  echoform::EchoParticles particles_;
  echoform::EchoController controller_;
  uint8_t facePrevE_[echoform::kFaceCells] = {0};
  bool speechWasBusy_ = false;
  uint32_t lastFinalHandled_ = 0;
  uint32_t faceForceUntilMs_ = 0;
  bool bootMarkedGood_ = false;
  uint32_t restartDeadlineMs_ = 0;  // non-zero: a restart is scheduled
  uint32_t restartLastBusyMs_ = 0;
  Preferences prefs_;
  uint8_t *waveFieldFb_ = nullptr;    // 16 KB, allocated in begin()
  uint8_t *waveFieldGlow_ = nullptr;  // 16 KB phosphor heat
  uint8_t *faceFieldFb_ = nullptr;    // 16 KB face field (square blit)
  float waveThickness_ = 0.0f;
  float agcPeak_ = 0.06f;  // engine.rs agc_mic peak tracker (FLOOR 0.06)
  uint32_t waveFrame_ = 0;
  uint32_t waveRng_ = 0x9E3779B9u;
};
