# Echoform on RSVP Nano - port plan

Status: IN PROGRESS. Written 7th August 2026 in the rusty-nail session that
mapped both codebases. REVISED the same day on Fulvio's direction: Echoform is
a STANDALONE FIRMWARE, not a mode inside the reader OS. M0 mic path is
mechanically proven (duplex I2S + ES7210 up, 5 s WAV recorded end to end) but
capture levels are near-silent (~ -64 dBFS peaks); level debugging continues
inside the standalone firmware.

## What this is

Port the Echoform voice loop from rusty-nail onto the RSVP Nano handheld
(Waveshare ESP32-S3-Touch-LCD-3.49), as its own firmware image. Only the
transcription pipeline moves: microphone capture, the ECHO1 link to the Mac
relay (whisper STT -> Claude -> TTS), wake word flow, and the transcript text.
None of rusty-nail's visuals come across. The interface is this device's
screensaver/demo engine promoted to the entire device: the firmware boots
straight into the starfield, which runs permanently as the backdrop, and the
216 morphing bobs become the voice visualisation (they morph into the live
waveform while Echoform listens or speaks, then return to the shape timeline).

## Firmware split (the revised shape)

Echoform is NOT a scene, menu item, or boot mode of the reader OS. It is a
separate PlatformIO environment (`echoform`) in this repo with its own slim
main loop, sharing subsystem code with the reader firmware as libraries:
BoardConfig, DisplayManager + native-stripe renderer, Screensaver engine,
AudioManager/Es7210, StorageManager (SD mount + config files only), and the
network stack as needed from M1 on. Explicitly excluded: `src/app/` (App,
scenes, menus), reader, book library, mod player UI, notifications, USB
transfer UI, demos menu - the whole OS chrome.

Build: `pio run -e echoform`. Flash over USB:
`pio run -e echoform -t upload`. The GitHub-releases OTA channel stays owned
by the reader firmware; do not publish echoform builds to it. (If Echoform
later needs its own OTA lane it gets its own release asset name.)

The main-loop rules from `App.cpp:1-62` carry over verbatim to the Echoform
loop: coordinate only, workers + atomics for slow paths, [tick] SLOW guardrail
at 33 ms.

Music policy is a hard rule: nothing autoplays. The existing screensaver
music autostart is disabled in this mode. The only music path is Spotify,
driven by voice through the relay's existing Spotify skill.

## Scope

In:
- ES7210 microphone capture (the board has a dual digital mic array; the
  firmware has never used it)
- A C++ ECHO1 client (protocol subset) over WiFi TCP to the relay on the Mac
- TTS playback through the existing ES8311/NS4150B output path
- Wake word flow (device VAD + preroll, relay-side phrase gate)
- Transcript display in this device's visual language
- Screensaver-as-interface scene with waveform bobs
- Spotify voice control + now-playing display (relay does all the work)

Out (explicitly not ported):
- The rusty-nail face, transcript terminal styling, dream scenes, demo reel
- The fcecho Rust crate as a linked library (we port the small pieces we
  need to C++ instead; the relay keeps linking fcecho as the wire-format
  source of truth)
- Any relay changes beyond configuration

## Source of truth: what to extract from ~/development/rusty-nail

| Path | Use here |
|---|---|
| `crates/fcecho/src/proto.rs` (1205 lines) | THE wire format. Port the framing, message types, CRC and flag semantics to C++. Do not invent; transcribe. |
| `crates/fcecho/src/features.rs` | The VAD: block RMS/band extractor with hysteresis (`voice_active`). Port the logic, 256-sample 16 ms blocks. |
| `firmware/src/echoform_net.rs` (550 lines) | Reference for session shape: Hello -> HelloAck -> ConfigureSession, 30 ms drain tick, 5 s keepalive Ping, 30 s request timeout, reconnect with 2 s backoff, NET_STATE 0/1/2. |
| `firmware/src/echoform.rs` (1381 lines) | Reference for the wake monitor state machine, speech ring sizing, in-flight guard, follow-up window. Constants below. |
| `crates/fcecho/src/text.rs` | Relay-side only (sentence chunking). Nothing to port. |
| `tools/echoform-relay/` | Reused unchanged. Runs on the Mac from the rusty-nail checkout. |
| `docs/ECHOFORM.md` (768 lines) | The design doc: personas, wake flow, pacing rationale, capture-window notes. |

Warning: rusty-nail currently has UNCOMMITTED changes to
`firmware/src/echoform.rs` and `tools/echoform-relay/src/session.rs` (the
closing prompt "Anything else, sir?" via BeginUtterance flag bit 2, plus a
music-chain cap). Another session owns those files. If we want closing-prompt
behaviour here, run the relay from that working tree; do not commit those
files from this effort.

## Hardware facts (verified 7th August 2026)

From the Waveshare demo repo at `~/development/ESP32-S3-Touch-LCD-3.49`
(`Examples/Arduino/08_Audio_Test/src/codec_board/board_cfg.h`, board
`S3_LCD_3_49`) and this repo's `src/board/BoardConfig.h`:

- I2S bus is full duplex on the PCB: mclk 7, bclk 15, ws/lrck 46, dout 45
  (ES8311 DAC), **din 6** (ES7210 ADC). `PIN_AUDIO_DIN = 6` is the missing
  constant; this firmware is TX-only today (`AudioManager.cpp` sets
  `data_in_num = I2S_PIN_NO_CHANGE`).
- ES7210 on the shared I2C bus (SDA 47, SCL 48), default address 0x80
  8-bit (0x40 7-bit). The Waveshare demo selects `MIC1 | MIC3` for plain
  stereo I2S; copy its init (`esp_codec_dev/device/es7210/es7210.c` is a
  clean reference driver).
- ES8311 at 0x18 is already fully initialised by `AudioManager.cpp:55-67`
  (verbatim register sequence; the 0x00=0x80 mid-sequence write is CSM
  power-up, not a reset; do not reorder). Its ADC is not what we use; the
  mics go through the ES7210.
- Speaker amp (NS4150B) is gated by TCA9554 expander P7. Silent unless
  enabled.
- Panel: logical 640x172 landscape, native-stripe render path (~16 KB DMA
  buffer, ~47 native rows per stripe). All fast screens compose per stripe.
- Both TTS and capture run at 16 kHz mono PCM16, which is exactly ECHO1's
  native format. No resampling anywhere on the device (rusty-nail had to
  resample 16000 -> 44118 for its DAC; we do not).

## Architecture on this device

```
ESP32-S3 (Arduino/FreeRTOS)                      Mac                    Cloud
+------------------------------------+           +-----------------+
| EchoformScene (main loop, 60 fps)  |           | echoform-relay  |--> OpenAI whisper STT
|   starfield + bobs + transcript    |  WiFi TCP | (rusty-nail,    |--> Anthropic chat
| mic task (core 0): I2S RX ES7210   | --------> |  unchanged)     |--> OpenAI tts-1
|   -> VAD -> preroll -> TX ring     |   :8125   |                 |--> Spotify Web API
| net task: ECHO1 client (C++ port)  |           +-----------------+
| audio out: SpeechPcm16 -> ES8311   |
+------------------------------------+
```

Three new FreeRTOS pieces, following this repo's own worker rules
(`App.cpp:1-62`: the main loop never blocks; slow work lives on tasks that
publish results via atomics/events):

1. **Mic task**: duplex-reconfigured I2S0 (16 kHz mono both directions while
   Echoform owns audio), `i2s_read` in 256-sample blocks, VAD features into
   atomics for the scene, PCM into a TX ring, rolling 1 s ring for the
   375 ms wake preroll.
2. **Net task**: TCP client to the relay, the echoform_net.rs session shape
   transcribed: frame parser with single-byte resync, request tracker,
   keepalive, reconnect backoff. Publishes transcript/status/PCM inbound.
3. **Speech drain**: inbound SpeechPcm16 into a ring, fed to `i2s_write`
   ahead of the DAC. Also feeds the same samples to the feature extractor so
   the bobs animate from what the speaker is actually playing.

## ECHO1 subset the device implements

Header (little-endian): magic "Ef" (0x45 0x66), u8 version = 1, u8 type,
u16 request id, u16 sequence, u16 flags, u16 payload len (max 1408), then
payload, then CRC-16/CCITT-FALSE over header+payload. Incremental parser,
resync by single-byte discard, never trust lengths.

Outbound: Hello 0x01, ConfigureSession 0x03, BeginUtterance 0x10,
AudioPcm16 0x11, EndUtterance 0x12, CancelRequest 0x13, Ping 0x30.
Inbound: HelloAck 0x02, TranscriptPartial 0x20, TranscriptFinal 0x21,
ModelStatus 0x22, TextDelta 0x23, SpeechPcm16 0x24, Completed 0x27,
Error 0x28, Pong 0x31, NowPlaying 0x47 (display string "Name, by Artist").
Ignore on arrival: EmotionHint 0x25, Usage 0x26, AmbientFeatures 0x46.
Later, optional: the voice menu family 0x40..0x45 (note from rusty-nail:
these are exempt from the request tracker by design; the tracker eats
unknown request ids as stale).

BeginUtterance flags: bit 0 = wake candidate (relay gates on the phrase),
bit 1 = trusted follow-up (no phrase needed), bit 2 = closing prompt (no
audio, canned line; only if running the relay tree that has it).

Relay endpoint configuration: `/echoform.json` on the SD card,
`{"host": "<mac-ip>", "port": 8125}`, NVS-cached like wifi.json. No
compile-time endpoint baking (that is a rusty-nail convention we drop).

## Tuning numbers carried over from rusty-nail (bench-proven there)

- Preroll 375 ms (6000 samples) from a rolling ring; the wake phrase is
  spoken before VAD trips, clips arrive truncated without it.
- Wake monitor (converted from RN's 60 fps frames to 16 ms audio blocks):
  trigger ~18 voiced blocks, end on ~45 silent (0.75 s), cap 6 s,
  cooldown 2 s, follow-up window 8 s with no VAD gate (hot mic after acks).
- Speech ring at least 600 ms (>= 9600 samples at 16 kHz); the relay bursts
  250 ms then paces 40 ms frames, and primes playback at ~180 ms. RN
  shipped overflow crackle until ring > burst with headroom.
- Wake eligibility requires the whole pipeline idle, not just "speaker
  silent": the ring drains between TTS sentences and RN's monitor fired
  into those gaps, cancelling replies mid-stream. Gate on session state.
- No echo cancellation in firmware: half-duplex policy. The mic is gated
  while speech is playing plus a short tail. The board claims an AEC
  circuit on the mic array; treat that as a bonus, not a design input.
- Cost honesty: every wake candidate is one whisper call (~1/30 cent).
  Noisy rooms trip VAD. On-device keyword spotting (ESP-SR/WakeNet is
  native on ESP32-S3) is the eventual fix and a natural M7.

## The interface: screensaver as the whole UI

The engine: `renderScreensaverFrame` draws 150 palette-tinted stars (7 motion
modes, re-rolled each shape change) behind 216 depth-sorted 3-tone shaded
balls morphing through 15 shapes (120 s cycle), no text, pure black
background, retro effect chain per stripe.

Port shape:
- `EchoformApp` (its own main loop in `src/echoform/`) drives `Screensaver`
  directly from boot. There is nothing to exit to; touch is input only
  (tap = push-to-talk, per milestone).
- **Waveform morph**: add one dynamic morph target beside the 15 static
  shapes: a 216-point ribbon spanning the logical 640 width whose Y
  displacement follows the live feature extractor (mic features while
  LISTENING, speech-playback features while SPEAKING). On voice activity
  the timeline suspends and points ease into the ribbon; on idle they ease
  back and the shape cycle resumes. The morph machinery already lerps
  point-for-point between 216-wide tables, so this is one new target
  buffer updated per frame, not a new renderer.
- Starfield never stops. Consider mode selection by conversation state
  (e.g. lightspeed while THINKING) once the basics land; keep it random
  first.
- **Transcript strip**: bottom rows of the logical screen, drawn into the
  same stripe composition (the screensaver renderer has no text today; the
  DisplayManager text renderers exist and draw into stripes elsewhere).
  User line then assistant line, marquee for overflow, partials replaced
  by finals. Status chip in a corner: NO IP / NO RELAY / LISTENING /
  THINKING / SPEAKING, plus the NowPlaying string when Spotify is active.
  Keep it minimal; the bobs are the show.
- Retro effect chain stays on (it is the device's look).

Music policy implementation: trivial under the firmware split - ModPlayer is
simply not part of the echoform build, so nothing can autoplay and nothing
can reclock I2S0 away from the duplex 16 kHz capture. Single audio owner by
construction. Spotify playback happens on Spotify devices via the relay
skill; this device only displays NowPlaying (0x47) and speaks confirmations.

## Milestones

M-1 - Standalone firmware skeleton (added in the revision)
  `echoform` env + `EchoformApp`: boots straight into the starfield/bobs
  loop, SD mounted, audio up, REC serial harness carried over. No menus,
  no reader, no chrome.
  Accept: device boots into the screensaver and REC still records.

M0 - Mic bring-up (hardware proof, no protocol)
  Add `PIN_AUDIO_DIN = 6`, reconfigure I2S0 duplex 16 kHz, minimal ES7210
  init (from the Waveshare reference driver), record 5 s to `/rec.wav` on
  SD via a temporary trigger, copy off over USB MSC.
  Accept: intelligible speech in the WAV at sane levels. (Rusty-nail's mic
  took two days over three stacked bugs; a WAV-first bring-up is the
  lesson.)
  STATUS 7th Aug 2026: MICS WORKING, root cause found and fixed. The
  ES7210 datasheet caps I2C data hold time at 900 ns after the clock edge;
  the ESP32 hardware I2C holds for microseconds, so every register write
  landed with bits dropped (and readbacks were garbage). All earlier
  symptoms - "frozen register file", "reads lie", ADC stuck at fs/8 - were
  this one violation. Fix: Es7210.cpp bit-bangs the chip's I2C with
  sub-microsecond hold; writes now verify perfectly and capture is
  continuous 16 kHz. Working config: ES7210 slave TDM I2S (0x12=0x02,
  four ADCs, 16-bit, MCLK 256x fs, 0x02=0xC1, OSR 0x20, 0x08=0x10);
  ESP32 legacy I2S duplex 32-bit slots stereo; live mics are TDM slots 0
  and 2 (top halfword of each 32-bit word).
  ACCEPTED 7th Aug 2026: user judged voice recordings "perfect" and music
  capture hearable. Bench commands REC/PLAY/BEEP/DUMP/I2C/ESRW/OFF stay in
  the echoform build for M1/M2 bring-up.

M1 - Wire client
  C++ ECHO1 framing + parser + CRC (transcribed from proto.rs), net task,
  `/echoform.json` config, Hello/HelloAck/ConfigureSession, keepalive.
  Accept: relay logs the session; serial monitor shows stable Pong RTT;
  survives relay restart via reconnect backoff.
  DONE 7th Aug 2026: src/echoform/Echo1.{h,cpp} (host golden test in
  tools/echo1_host_test.cpp, ALL PASS incl. CRC vector 0x29B1),
  EchoformNet worker task (WiFi via wifi.json + WifiConnector, endpoint
  via /echoform.json or the SETRELAY serial command). All three acceptance
  criteria verified on the bench: relay logged the session and configured
  voice 'fable'; Pong RTT stable at 7-21 ms; relay kill/restart recovered
  automatically through the 2 s backoff. Starfield held ~58 fps with the
  session live.

M2 - Push-to-talk loop end to end
  Hold BOOT (or hold touch) = capture -> BeginUtterance(flags 0) ->
  AudioPcm16 -> EndUtterance; play SpeechPcm16 through the speaker (amp
  enable, prime at ~180 ms); transcript + deltas to serial.
  Accept: ask a question aloud, hear the answer through the console
  speaker.
  DONE 7th Aug 2026: EchoformAudio (mic TX ring + speech ring, worker
  tasks), EchoformNet utterance commands + request tracker + mic pump,
  BOOT hold PTT + TALK serial command. Three live rounds against the
  relay; replies audible on the speaker. Tuning en route: mic PGA 36 dB +
  2x digital gain (33 dB left whisper hallucinating Japanese on quiet
  audio), speaker volume 90%. Relay recordings/transcript confirm the
  full loop. Known nit: one benign "sequence gap (resynced)" per reply.

M3 - The interface
  Waveform morph target + transcript strip + status chip layered into the
  EchoformApp render loop.
  Accept: user judges it on the device (bobs must visibly ride the voice
  both directions).

M4 - Self-update + self-modification
  Three legs. (a) On-device safety net: an NVS boot counter counts every
  boot attempt and clears only after a 30 s self-test window (render loop
  alive, heap above floor); three strikes, or a voice/serial rollback
  request, reflashes the last-known-good release (pinned tag resolved via
  the GitHub tags API) and holds the abandoned release stamp so the boot
  OTA will not reinstall it until told to. Voice intents: "install your
  update" / "update yourself" (restart into the boot OTA check, clears the
  hold), "roll back your update". Serial: UPDATE / ROLLBACK / BOOTINFO.
  Restarts wait for the spoken reply to finish. (b) CI coding agent:
  .github/workflows/echoform-selfmod.yml takes a voice instruction via
  workflow_dispatch, runs headless Claude Code on the repo (edits confined
  to src/ + docs/, mechanically enforced), gates with the ECHO1 host
  golden test plus both firmware builds, then commits to main and
  publishes the release. (c) Voice trigger: the relay's webhook skill
  gained per-hook headers and {transcript} substitution (substituted
  inside the parsed JSON tree, so quotes cannot break out); a hook on
  "change your code" phrases POSTs the transcript to the workflow's
  dispatch endpoint.
  Accept: say "change your code: <something small>"; the workflow runs and
  publishes; "install your update" installs it; a deliberately broken
  build rolls itself back and holds.
  STATUS 9th Aug 2026: implemented (firmware + workflow + relay skill).
  Manual steps pending: ANTHROPIC_API_KEY secret on the repo, the
  ECHOFORM_WEBHOOKS Railway var carrying the dispatch hook + GitHub token,
  and a relay redeploy - the credential handling stays in the owner's
  hands.

M5 - Extraction to a private repo (directed 9th Aug 2026)
  Everything Echoform - this firmware, the relay, the fcecho crate, the
  Docker/Railway setup, the selfmod workflow - moves to a standalone
  private repo at ~/development/echoform (gotnull/echoform) so firmware
  releases stop being public on gotnull/rsvpnano. Private releases mean
  the device OTA must authenticate (token on SD, asset fetched via the
  assets API with a manual-redirect two-hop). Full plan and repo map:
  HANDOFF.md in ~/development/echoform.

M6 - Wake word
  VAD port, preroll ring, wake candidate flags, follow-up window, silent
  discards, in-flight guard, closing-prompt support if the relay tree has
  it.
  Accept: "Hey Rusty" from across the room answers in character; TV noise
  produces silent discards, never an error state.

M7 - Spotify + now playing
  Nothing device-side beyond NowPlaying display and testing the voice
  path; relay Spotify auth is `--spotify-login` on the Mac.
  Accept: "play <song> on Spotify" works by voice; title and artist show
  on the strip.

M8 - Optional polish
  Voice picker via 0x40..0x45, settings rows (mic gain, captions),
  Echoform-on-boot option, state-driven starfield modes.

M9 - Future
  On-device wake word (ESP-SR WakeNet) to kill whisper-per-candidate
  costs.

## Rules and pitfalls for the build session

- Read `App.cpp:1-62` for the main-loop rules; they bind EchoformApp's loop
  the same way: never perform slow work inline; workers + atomics; ticks
  over 33 ms log `[tick] SLOW`.
- Echoform code never touches `src/app/` - no AppState, no scenes, no
  SceneContext. Those are reader-OS machinery.
- Stop the serial monitor before `pio run -t upload` (port busy
  otherwise). Echoform build: `pio run -e echoform`; reader firmware
  remains the repo default (`waveshare_esp32s3_usb_msc`).
- Relay ops (from rusty-nail bench experience): long-lived relay processes
  accumulate half-open board connections after reflashes; restart the
  relay when utterances stop landing. Relay env lives in
  `~/development/rusty-nail/tools/echoform-relay/.env` (ANTHROPIC_API_KEY,
  OPENAI_API_KEY, ECHOFORM_* knobs; README there documents all of them).
  Start: `cargo run` in that directory; `--spotify-login` once for
  Spotify.
- The wire format has one source of truth: fcecho's proto.rs (the relay
  links it). The C++ port must match it byte for byte; when in doubt, add
  a golden-frame test (hex fixtures generated by the Rust side) rather
  than reasoning from memory.
- Half duplex always: never stream mic PCM while the speech ring is
  non-empty (plus tail); the shared-speaker-mic coupling that plagued the
  rusty-nail PCB is unknown territory on this board.
