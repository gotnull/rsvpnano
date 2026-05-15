# Session Notes — Architecture Refactor, Module Player, Camera Stream, Boot Cleanup

This doc captures everything learned and changed across one long session covering:

- Demoscene screensaver upgrades
- libxmp MOD/XM/S3M player with audio task
- Modules picker + tracker player UI
- Scene-based architecture (Milestone 1)
- MJPG camera streaming refactor
- Boot behavior overhaul (lazy library, always-menu)
- OTA reliability fix (GitHub API path)
- Multiple critical bug fixes

It's a working reference — read sections as needed when resuming work.

---

## 1. Screensaver — Demoscene Upgrade

### Architecture

`src/screensaver/` is table-driven. `Screensaver.h/.cpp` is the orchestrator only — every extension axis lives in a sibling header pair so adding shapes / morph styles / star modes / rotation timelines is a one- or two-line edit. No heap, no virtual dispatch, all tables in flash.

| File | Owns |
| --- | --- |
| `Screensaver.{h,cpp}` | public API, per-frame orchestration, shape storage, palette accessors |
| `ShapeRegistry.{h,cpp}` | 15 named shape generators + `ShapeGenerator` descriptor + `ShapeRng` |
| `Timeline.{h,cpp}` | `ShapeKeyframe` table, `EaseCurve`, `MorphStyle`, `sampleTimeline()`, `applyMorphStyle()`, `resolvePoint()` |
| `RotationAnimator.{h,cpp}` | `RotationSegment` table + frame-stepped Euler accumulator + Tait-Bryan ZYX matrix |
| `StarfieldModes.{h,cpp}` | `StarfieldModeDef` strategy table for `Forward3D` / `Parallax` / `Vortex` / `Twinkle` / `Rain`, plus `StarFieldRng` wrapper |

`Screensaver::tick()` is now ≈30 lines: advance frame counter, detect shape transition, advance rotation animator, update stars via strategy, sample shape timeline + resolve points, project + brightness, hand off to `sortPoints()`.

### How to add things

- **New shape**: write `void genFoo(ShapeBuffer&, ShapeRng&)` in `ShapeRegistry.cpp`, append `{"foo", genFoo}` to `kRegistry`, bump `Screensaver::kShapeCount`. `static_assert` will catch a mismatch.
- **New keyframe / re-order**: edit `kTimeline[]` in `Timeline.cpp`. Per-keyframe hold/morph frames are independent so one shape can dwell longer than the rest.
- **New morph style**: extend `MorphStyle`, add a branch in `applyMorphStyle()`. Per-point seed + index are already plumbed so explode/spiral/dissolve styles can be deterministic-but-varied.
- **New rotation timeline**: edit `kSegments[]` in `RotationAnimator.cpp`.
- **New star mode**: add an enum value to `Screensaver::StarMode` (above `kCount`), write `initX`/`updateX` in `StarfieldModes.cpp`, append `{"x", initX, updateX}` to `kModes`. `static_assert` ties the table size to the enum.

### Shapes (15 total)

1. Cube grid (6³)
2. Sphere (Fibonacci spiral, uniform)
3. Torus
4. Single-helix tube
5. Double helix (two tube strands)
6. Random cloud
7. Wave-plane ripple
8. Lissajous blob
9. Octahedron edges
10. Trefoil knot
11. Möbius strip
12. Hyperboloid of one sheet
13. Hyperbolic-paraboloid saddle
14. Pyramid edges
15. Icosahedron edges

**Stacking rule** (shapes 2..14): each generator writes `N_distinct < 216` positions and stacks `216/N_distinct` consecutive indices on each so the morph keeps a full 216-wide target while the visible nearest-neighbour distance stays ≥ 0.30 model units (~18 px center-to-center at `kFocal=180`, `kCameraZ=3`). That guarantees ≥ half-a-ball-diameter of empty space at the bumped 16/19 px radius. Cube and sphere are already sparse enough at full 216-distinct.

### Animation

- Tait-Bryan ZYX rotation matrix (`RotationAnimator`)
- 7-segment rotation timeline — per-axis rates change every 220–320 frames (1840 frames ≈ 31 s per rotation cycle)
- Shape timeline: 360 hold + 120 morph per keyframe → 15 × 480 = 7200 frames ≈ 120 s full cycle
- Subtle Y-bias bob (10 s sine)
- Five star modes — `rerollStarMode()` picks a new mode at every shape transition

### Rendering (`renderScreensaverFrame` in `DisplayManager.cpp`)

- Native-stripe path: composes directly into panel-native `txBuffer_` (DMA-capable internal RAM ~16 KB / ~47 rows), pushes via `drawBitmap` per stripe
- 3-tone shaded ball: dim outer rim → bright core shifted toward "light" by `radius/4` → small white specular at offset `radius/3`
- Z-brightness depth cue: every RGB channel scaled by `p.brightness` (255 near → 90 far)
- Focal 180, camera Z=3.0, radius scaler 16 (cap 19)
- 150 stars with random per-star tint (8-color palette) and size (1 / 3 / 5 px discs)
- `SCREENSAVER_PROFILING=1` emits per-frame `[saver] tick=… compose=… spi=… total=… us fps=…`

### Memory

- 15 shapes × 216 points × 12 bytes ≈ 38 KB shape table (compile-time, not heap)
- 150 stars × ~24 bytes ≈ 3.6 KB
- Renderer sprite buffer (`Sprite[366]`) is `static` inside `renderScreensaverFrame` so it doesn't ride the ~8 KB Arduino loop stack
- All other tables (shape registry, timeline, rotation segments, star modes) are `constexpr` arrays in flash

---

## 2. MOD / XM / S3M / IT Player

### libxmp-lite

Vendored into `lib/libxmp-lite/` (4.6.0). 30+ C files copied from the official lite subset:

- `src/*.c` — virtual, format, period, player, read_event, misc, dataio, lfo, scan, control, filter, effects, mixer, mix_all, load_helpers, load, filetype, hio, smix, memio, win32, md5
- `src/loaders/*.c` — common, itsex, sample, xm_load, mod_load, s3m_load, it_load
- `include/libxmp-lite/xmp.h` — public API
- `library.json` declares `LIBXMP_CORE_PLAYER`, suppresses three GCC warnings

### `ModPlayer` (`src/audio/ModPlayer.h/.cpp`)

- FreeRTOS task pinned to core 0, stack 6 KB, priority 5
- Render loop: `xmp_play_buffer` → volume × duck scale → `i2s_write` (1024 stereo samples per chunk @ 44.1 kHz)
- File buffered into PSRAM via `heap_caps_malloc(MALLOC_CAP_SPIRAM)` with internal-RAM fallback
- Reconfigures I²S to 44.1 kHz stereo on play; restores to 16 kHz mono in the task cleanup
- `getNowPlaying()` returns title, format, BPM, speed, position, row, channel volumes (0..64 per channel, up to 32 channels) — used by the tracker player UI
- Notification duck: `setDuckPercent(30)` while a notification tone plays, restored to 100 after

### Critical: `stop()` is async

The previous design did synchronous libxmp release + heap free + i2s clock restore on the calling thread, which was inside the screensaver dismiss touch handler. That blocked the main loop for hundreds of ms.

Now `stop()` only sets `stopRequested_` and notifies the task. The task runs the cleanup branch on its next iteration. The main thread is never blocked.

`playFile()` calls `waitForStop()` before installing a new context so a half-released libxmp state can't be observed.

### Modules picker

`MenuScreen::ModulesPicker` (`openModulesPicker`, `renderModulesPicker`, `selectModulesPickerItem`):

- Lists `/mods/*.{mod,xm,s3m,it}` via `StorageManager::listModuleNames()`
- "Back" row + placeholder "(no modules — drop …)" if empty
- Tap → `enterModulePlayback(path, nowMs)`

### Tracker player view (`AppState::ModulePlaying`)

`renderModulePlayerFrame` in `DisplayManager.cpp`:

- Top headline: module title (marquee if wide) in focus color
- Sub-line meta: filename + format + BPM + SPD + POS + ROW
- Channel bars: rainbow palette, per-channel volume scaled to bottom half. Each frame decays bars by `kBarDecay=6` and bumps to current libxmp volume — reads as a level meter, not raw volume.
- Auto-returns to picker when track ends (libxmp `xmp_play_buffer` returns < 0)

### Auto-shuffle

`setState` watches transitions:

- Entering Screensaver or DemoPlaying with `demoMusicMode_ != 0` → `startRandomModule()`
- Leaving those states → `modPlayer_.stop()` (async)

`startRandomModule()` picks via `esp_random()` from `listModuleNames()`. `Demo music: Off / Shuffle / Picked` in Settings.

### Notification ducking

`playNotificationTone()` calls `modPlayer_.setDuckPercent(30)` before the tone, `setDuckPercent(100)` after. Since RTTTL/WAV playback is synchronous, the duck window is exactly the tone duration.

### MOD starter pack downloader

`downloadModStarterPack()` is currently a stub (just renders a status). A `mods-pack-v1` release was created on `gotnull/rsvpnano` with five tracker tunes (Gaffeltruck.mod, ode2ptk.mod, test.xm, reverse_xm.xm, s3m_sample_porta.s3m), so the wiring can be completed next time without more hosting work.

---

## 3. Scene Architecture — Milestone 1

### Why

`App::update()` had grown into a thousand-line god method doing scene logic, input, render, audio transitions, SD work, network/OTA, book loading, notification polling, `delay()` calls, blocking I/O, ad-hoc state branches. Adding features kept introducing lifecycle side-effects and blocking paths.

The replacement is incremental: introduce Scene/SceneManager/EventBus, migrate one scene (Screensaver) as proof, leave the rest under a `LegacyScene` catch-all that still defers to the old `App::update()` path. Next migrations land one scene at a time.

### Scene interface (`src/app/scene/Scene.h`)

```cpp
class Scene {
 public:
  virtual ~Scene() = default;
  virtual const char* name() const = 0;
  virtual void enter(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual void tick(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual void exit(SceneContext& ctx, uint32_t nowMs) = 0;
  virtual bool onTouch(SceneContext& ctx, const TouchEvent&, uint32_t nowMs) = 0;
};
```

Rules:

- `enter()` does setup, no `delay()`, no blocking I/O
- `tick()` advances animation + renders, must not block
- `exit()` posts cleanup to background tasks rather than running it synchronously
- `onTouch()` returns true if consumed; false falls through to legacy

### `SceneContext` (`src/app/scene/SceneContext.h`)

Bundle of subsystem refs (`DisplayManager`, `Screensaver`, `ModPlayer`, `EventBus`) plus a per-call output slot (`requestedExit` + reason). SceneManager resets the output before each scene call. Crucially does NOT hand scenes a back-pointer to App.

Has an explicit constructor (`SceneContext(DisplayManager&, Screensaver&, ModPlayer&, EventBus&)`) because the project's C++14-ish build doesn't allow aggregate-init for structs with default-initialized members.

### `SceneManager` (`src/app/scene/SceneManager.h/.cpp`)

- Static 8-slot registry indexed by `SceneId`
- `registerScene(id, scenePtr)` at boot
- `begin(ctx, nowMs)` activates `SceneId::Legacy` (no-op)
- `requestTransition(next, reason, nowMs)` is fire-and-forget; the transition runs in `processPendingTransition()` which is called at the top of `tick()` and at the tail of `dispatchTouch()` — never mid-handler
- `tick()` is unconditional — even when the active scene is Legacy. This was a critical bug: an earlier version gated `tick()` on `active() == Screensaver` which created a chicken-and-egg where the pending Legacy→Screensaver transition could never run because `tick()` was never called. Legacy::tick is a no-op so unconditional cost is zero.
- `dispatchTouch()` returns false for Legacy, letting App's existing handler run untouched
- Every transition logs: `[scene] exit X` / `[scene] transition from=X to=Y reason=…` / `[scene] enter Y`
- Publishes `EventType::SceneChanged` after each transition

### `ScreensaverScene` (`src/app/scene/ScreensaverScene.h/.cpp`)

Owns:

- `screensaver_.begin()` exactly once per activation (in `enter()`)
- 16 ms frame cap + `renderScreensaverFrame` (in `tick()`)
- `ctx.requestExit("touch")` on any touch phase (in `onTouch()`)
- Publishes `ScreensaverEntered` / `ScreensaverExited` on enter/exit

Does NOT own:

- Idle detection (App still drives that — it needs cross-state visibility)
- AppState bookkeeping (App keeps `AppState::Screensaver` as the coarse marker until the rest of the state machine migrates)
- Audio cleanup (ModPlayer's task does that on core 0)

### `LegacyScene` (`src/app/scene/LegacyScene.h`)

No-op fence around the unmigrated `App::update()` path. Activated whenever a real scene exits.

### Event bus (`src/app/event/EventBus.h/.cpp`)

- 16-slot bounded static array — no heap
- Synchronous publish (handlers run inline)
- `Event` struct: type + 2 generic `uint32_t` slots + timestamp
- Events declared: TouchTap, SceneChanged, ScreensaverEntered, ScreensaverExited, ModCleanupStarted, ModCleanupFinished, OtaAttempt, OtaResult
- Subscribers register at boot with a static trampoline + `this` as userdata

### App wiring

Members added (order matters — `SceneContext` refs precede it):

```
EventBus events_;
LegacyScene legacyScene_;
ScreensaverScene screensaverScene_;
SceneContext sceneCtx_;
SceneManager sceneManager_;
```

App's constructor builds `sceneCtx_{display_, screensaver_, modPlayer_, events_}`. `begin()` registers scenes, subscribes to `ScreensaverExited`, calls `sceneManager_.begin()`.

`update()`:

- Idle trigger calls `sceneManager_.requestTransition(Screensaver, "idle", nowMs)` AND `setState(AppState::Screensaver)` for legacy compat
- `sceneManager_.tick(nowMs)` runs unconditionally; trackStage records "saver" only when active scene is Screensaver

`handleTouch()`:

- Screensaver dismiss branch routes to `sceneManager_.dispatchTouch(ev, nowMs)`
- App's `onScreensaverExited` event handler restores the legacy AppState via `setState(screensaverPreviousState_)`

### Next migrations (recommended order)

1. **Menu** (biggest payoff) — main menu + settings + all pickers
2. **Reader** — Paused + Playing together; pulls reader-refresh out of `App::update()`
3. **ModPlayer** — straight port of existing renderModulePlayerFrame
4. **OTA / download / network** — move to a single background Wi-Fi task with a queue
5. **Camera / DemoPlayer** — last; already partially scene-shaped

---

## 4. MJPG Camera Stream

### What works

- Native-stripe renderer (`renderCameraRgb565Frame`) — aspect-fits 320×240 source into 640×172 panel, 180° rotation, compose direct into native stripes, push via `drawBitmap`
- Byte-buffered multipart header parser (`parseCameraMjpegHeaders`) — 320-byte stack buffer, scans for `\r\n\r\n` terminator and case-insensitive `Content-Length`; zero String allocations per frame
- Stale-frame drain (`drainStaleCameraFrames`) — after each body read, peek TCP buffer; if more `--frame` boundaries are queued, skip headers + body bytes so the decoded frame is the most recent one
- Snapshot fallback — if stream parse fails, fetch `/snapshot.jpg` once to keep the display moving and let the next iteration reconnect the stream
- Touch preemption — `pollCameraExitTouch` runs before each update and inside body-read loops; exit aborts mid-read

### Known bug — drain desync (NOT FIXED)

If `drainStaleCameraFrames` runs out of buffered TCP bytes mid-body, it returns leaving partial body in the stream. Next `parseCameraMjpegHeaders` reads those JPEG bytes as headers → 320-byte buffer fills with garbage → `[mjpg-hdr] header buffer overflow`. Recovery is via snapshot fallback + stream reconnect (~300 ms cost).

Cycle observed: ~5–10 OK frames → desync → 1 snapshot fallback → reconnect.

**Fix** (write next session): in `drainStaleCameraFrames`, if `available() <= 0` mid-body, `closeCameraMjpegStream()` and return. The next iteration's `readCameraMjpegFrame()` will see closed stream and call `openCameraMjpegStream()` cleanly. No body bytes ever leak into the header parser.

```cpp
while (remaining > 0) {
  const int avail = cameraStreamClient_.available();
  if (avail <= 0) {
    Serial.printf("[mjpg-drain] desync mid-body (%d/%d), closing stream\n",
                  contentLength - remaining, contentLength);
    closeCameraMjpegStream();
    return;
  }
  // ...read body
}
```

### Known issue — single-thread, no pipelining

All camera work runs on the main thread:

- HTTP read: 100–200 ms / frame
- JPEG decode + RGB565 copy: 50–100 ms
- Native-stripe project + drawBitmap: 80–120 ms

Total: ~200–400 ms per frame ⇒ ~3–5 fps. The same 320×240 stream rendered by a browser is smooth because decode + render are pipelined; on the device they're serial.

**Architecturally correct fix** (next session work):

Dedicated `CameraStreamTask` pinned to core 0:

- Producer task: open stream once, loop { parse header → read body → decode JPEG → write RGB565 to back buffer → atomic swap front/back → signal "new frame" → repeat }
- 2× RGB565 buffers in PSRAM (300 KB total — fine)
- Main thread `updateCameraStream()` becomes: peek latest front buffer → `renderCameraRgb565Frame` → done
- Touch dismiss signals task to stop, waits briefly, exits

This decouples decode rate from render rate and overlaps the two. Realistic ceiling jumps from ~5 fps to ~15–20 fps. 60 fps would need either a hardware JPEG decoder (ESP32-S3 has none built-in) or much smaller frames.

**Further optimization** (later still): decode MCU blocks directly into the panel-native stripe buffer, eliminating the 150 KB intermediate RGB565 buffer. This is non-trivial because MCUs decode in source raster order while panel stripes are in rotated panel order — you'd need to buffer per-stripe and flush when complete.

### Server side (`examples/mac_camera_stream/server/camera_server.py`)

- Python OpenCV → multipart/x-mixed-replace HTTP stream
- Frame counter / timestamp / red bar overlay (toggle with `--no-overlay`)
- Default 320×240 @ 10 fps quality 60

### Build flags

```cpp
#define RSVP_CAMERA_SERVER_HOST  "192.168.5.77"
#define RSVP_CAMERA_SERVER_PORT  8080
#define RSVP_CAMERA_SNAPSHOT_PATH "/snapshot.jpg"
#define RSVP_CAMERA_STREAM_PATH   "/stream.mjpg"
```

Override via `-DRSVP_CAMERA_SERVER_HOST=\"YOUR_IP\"` in root `platformio.ini`.

---

## 5. Boot Behavior

### Final state

1. Boot does NOT call `storage_.listBooks()` and `storage_.begin()` itself no longer triggers `refreshBookPaths()` — library scan is fully lazy.
2. Boot ALWAYS lands on `AppState::Menu` with `menuScreen_ = Main`, regardless of saved app state.
3. Book restore is meta-only: `prepareSavedBookMeta()` derives the "Resume from" label from the saved file's basename without scanning the library. Full content load is deferred until the user actually taps Resume.
4. The lazy scan fires from three places:
   - `App::openBookPicker()` — first time the user opens it this session
   - `App::openAuthorPicker()` — same
   - `App::ensureCurrentBookLoaded()` — when Resume is tapped, before content load

### Why this matters

A 142-book library took ~24 s to scan (collectBookPaths + header read + epub stem walk). Previously this ran on every boot. After the refactor the user lands in the menu in ~1 s; the 24 s cost is paid only on first picker open or Resume tap.

### Files changed

- `src/storage/StorageManager.cpp:1100-1117` — removed `refreshBookPaths()` from `begin()`
- `src/app/App.cpp:524-528` — removed `listBooks()` from `begin()`
- `src/app/App.cpp:598-614` — boot path always meta-only restore
- `src/app/App.cpp:1102-1112` — boot state always Menu
- `src/app/App.cpp:openBookPicker`, `openAuthorPicker`, `ensureCurrentBookLoaded` — lazy scan calls

---

## 6. OTA Reliability

### Problem

`https://github.com/<owner>/<repo>/releases/latest/download/firmware.bin` is a redirect chain:

1. github.com → 302 to release-specific URL
2. → 302 to `objects.githubusercontent.com` via fastly
3. → 200 with the binary

Arduino's HTTPClient + WiFiClientSecure does not reliably preserve the secure session across the cross-host hop. The user reported persistent `HTTP -1` (HTTPC_ERROR_CONNECTION_REFUSED) failures. Made worse by reduced free heap (scene infra + audio task have eaten into TLS handshake budget).

### Fix (`src/network/OtaManager.cpp:155-228` + `resolveGithubLatestAssetUrl`)

1. Detect `/releases/latest/download/` URL pattern.
2. Hit `https://api.github.com/repos/<owner>/<repo>/releases/latest` instead. Returns single-response JSON, no redirect, no cross-host hop.
3. String-search the JSON for the asset's `"name":"firmware.bin"` then the next `"browser_download_url":"…"` — that's the direct signed `objects.githubusercontent.com` URL.
4. Fetch firmware from that resolved URL.
5. If the API call fails for any reason (DNS, TLS, parse), fall through to the legacy `/releases/latest/download/` URL with the existing 3-attempt retry loop.

User-Agent `rsvp-nano-ota/1.0` required — GitHub API 403s without one.

### Verification log

```
[ota] api https://api.github.com/repos/gotnull/rsvpnano/releases/latest → HTTP 200
[ota] resolved asset URL via API: https://github.com/gotnull/rsvpnano/releases/download/ota-…/firmware.bin
[ota] attempt 1/3 → HTTP 200
```

Works consistently after this change.

### `setBufferSizes` note

I tried `WiFiClientSecure::setBufferSizes(4096, 1024)` to reduce TLS handshake heap. This Arduino-ESP32 version doesn't expose it. Removed. The API-resolve path makes it less necessary anyway.

---

## 7. Critical Bug Fixes

A list of subtle bugs found and fixed in this session — each one was a visible user-pain point.

### 7.1 `updateState()` bouncing non-Reader states to Paused

`App::updateState()` runs every iteration and falls through to `setState(Paused)` for any state not in its allowlist. The allowlist was `{Menu, Sleeping, Screensaver, DemoPlaying}` — missing `CameraStream`, `ModulePlaying`, `UsbTransfer`. So Camera test rendered ONE frame then bounced to the reader; ModulePlaying ended instantly on entry.

**Fix** (`App.cpp:1120-1130`): added the three missing states.

### 7.2 Menu chevron column truncating every row

`renderMenu` computed `chevronColumnX = kCompactMenuX + maxItemTextWidth + spaceWidth*2`. With 100+ MOD filenames, one very long item pushed `chevronColumnX` off-screen, making `maxWidth` for ALL rows negative → every row got ellipsised to `…`.

**Fix** (`DisplayManager.cpp:renderMenu`): cap `chevronColumnX` at `virtualWidth - chevronWidth - kFooterMarginX - 4`, and redefine `maxWidth` as `chevronColumnX - kCompactMenuX - spaceWidth` so it stays strictly positive.

### 7.3 Modules picker exits to main menu on up/down swipe

`App::renderMenu()` is the central re-render router after `moveMenuSelection`. Its switch was missing `MenuScreen::ModulesPicker` → fell through to `renderMainMenu()`. Underlying state was correct; only the redraw was wrong.

**Fix** (`App.cpp:5210-5235`): added the `ModulesPicker` branch to the router. Also added a defensive `[render-bug]` log if `renderMainMenu` is ever called with a non-Main `menuScreen_`.

### 7.4 SceneManager gate chicken-and-egg

`App::update()` initially called `sceneManager_.tick()` only when `sceneManager_.active() == Screensaver`. But `active_` only flips to `Screensaver` inside `tick()` when it processes the pending transition. So the pending Legacy→Screensaver never ran.

**Fix** (`App.cpp:712-723`): call `sceneManager_.tick()` unconditionally. Legacy::tick is a no-op so it costs nothing.

### 7.5 ModPlayer.stop() blocking the touch handler

The old `stop()` waited synchronously for the audio task to drain (~200 ms) + ran libxmp release + heap free + i2s clock restore on the calling thread. Called from `setState()`, which is called from the screensaver dismiss touch handler. Result: multi-second touch-unresponsive window after dismiss.

**Fix** (`ModPlayer.cpp`): `stop()` only sets `stopRequested_` and notifies the task. The task itself runs all cleanup (xmp_end_player + xmp_release_module + xmp_free_context + heap_caps_free + i2s_set_clk back to 16 kHz mono). `playFile()` calls a new `waitForStop()` before installing the new context.

### 7.6 Boot auto-resuming a book the user wasn't on

Saved `AppState` from a previous session could push the device straight into Paused (reader screen) on boot, even when the user was in Settings/Menu before the restart.

**Fix** (`App.cpp:1102-1112`): boot now always lands in Menu, ignoring saved app state. The user taps Resume to load the book.

### 7.7 Screensaver music starting on entry persisted into Menu

If `Demo music` was Shuffle and a track was playing during Screensaver, dismissing returned to Menu with the track still playing. `setState` had a transition hook to stop on leaving music states, but the synchronous `modPlayer_.stop()` blocked the touch handler (see 7.5).

**Fix**: async stop from 7.5 made this clean.

---

## 8. Outstanding Work

In rough priority order:

1. **Camera drain desync close** — 10-line fix, removes the recurring `[mjpg-hdr] header buffer overflow` → snapshot-fallback → reconnect cycle. Code already drafted in section 4.
2. **Camera task** — biggest perf win. Move HTTP fetch + JPEG decode to a FreeRTOS task on core 0; double-buffered RGB565 swap; main thread just renders the latest. Estimated ~15–20 fps vs current ~5 fps.
3. **Library index NVS cache** — first picker open still costs ~24 s for 142 books. Caching the scan result in NVS makes subsequent sessions instant. Invalidate on SD remount.
4. **Migrate Menu scene** — pulls the biggest chunk out of `App::update()`. Routes touches and renders.
5. **Migrate Reader scene** (Paused + Playing) — collapses the periodic reader-refresh out of `App::update()`.
6. **Migrate ModPlayer scene** — straight port of `renderModulePlayerFrame`.
7. **OTA / download / notifications task** — single Wi-Fi task with a queue; scenes subscribe to status events.
8. **Migrate Camera / DemoPlayer scenes** — last; already partially scene-shaped.
9. **Replace snapshot fallback** when the camera task lands — the task self-heals on stream errors, so the inline fallback isn't needed.
10. **MCU-direct rendering for camera** — skip the 150 KB intermediate RGB565 buffer. Complex (rotation + aspect-fit + MCU raster order). Defer until after the task split lands.
11. **MOD starter pack downloader** — implementation that actually fetches the `mods-pack-v1` release artifacts on demand.
12. **Final OTA tag for the camera task work** — currently `ota-20260511082536` is the latest functional binary the user has running; the Stage 1 fix + camera task should ship as one cohesive build.

---

## 9. File Inventory

### New files (uncommitted)

```
src/app/event/Events.h
src/app/event/EventBus.h
src/app/event/EventBus.cpp
src/app/scene/Scene.h
src/app/scene/SceneId.h
src/app/scene/SceneContext.h
src/app/scene/SceneManager.h
src/app/scene/SceneManager.cpp
src/app/scene/LegacyScene.h
src/app/scene/ScreensaverScene.h
src/app/scene/ScreensaverScene.cpp
src/audio/ModPlayer.h
src/audio/ModPlayer.cpp
lib/libxmp-lite/                            (vendored library, ~30 .c/.h files)
docs/SESSION_NOTES.md                       (this doc)
```

### Modified files (uncommitted)

```
src/app/App.h
src/app/App.cpp
src/app/AppState.h
src/display/DisplayManager.h
src/display/DisplayManager.cpp
src/storage/StorageManager.h
src/storage/StorageManager.cpp
src/network/OtaManager.h
src/network/OtaManager.cpp
src/screensaver/Screensaver.h
src/screensaver/Screensaver.cpp
examples/mac_camera_stream/README.md
README.md                                   (pinout reference, rendering arch section)
platformio.ini                              (SCREENSAVER_PROFILING=1)
```

### Deleted

```
examples/mac_camera_stream/LLM_HANDOFF.md   (contents folded into README)
```

---

## 10. OTA Release Timeline

| Tag | What's in it |
|---|---|
| `ota-20260510103154` | Per-stage timing instrumentation |
| `ota-20260510124207` | Demo music + module picker |
| `ota-20260510124833` | Async modPlayer.stop |
| `ota-20260510125643` | 3D ball fix (rim/core/specular) |
| `ota-20260510130859` | Larger balls + Z-brightness |
| `ota-20260510132915` | Stage 1 MOD player |
| `ota-20260510133408` | Stage 2 picker + auto-shuffle |
| `ota-20260511060028` | Camera test added |
| `ota-20260511071127` | Modules picker fixes (chevron + tracker UI) |
| `ota-20260511072832` | MJPG refactor (byte parser + drain) |
| `ota-20260511073209` | Modules picker render-router fix |
| `ota-20260511074441` | Extensive logging |
| `ota-20260511075109` | Async stop, boot-to-menu, OTA retry |
| `ota-20260511080518` | Scene Architecture Milestone 1 |
| `ota-20260511081541` | SceneManager gate fix |
| `ota-20260511082022` | Library scan deferred from boot |
| `ota-20260511082536` | OTA via GitHub API |
| `ota-20260511083015` | storage_.begin internal scan kill |
| `ota-20260511083317` | updateState allowlist (CameraStream / ModulePlaying / UsbTransfer) |

Latest functional OTA the user pulled: `ota-20260511082536`. The `83015` and `83317` builds are published but may not have been pulled.

The `mods-pack-v1` release contains five tracker tunes for the starter pack downloader (still stubbed).

---

## 11. Resume Tips for Next Session

- Commit the working tree before touching anything new (it's all uncommitted right now).
- First task: the drain desync close in `drainStaleCameraFrames` — small, well-understood, high-impact.
- Then the camera task — biggest user-visible win on the most-recent complaint.
- Avoid further App.cpp expansion; new functionality should land as a scene class behind the existing infra.
- The serial monitor command is in tools/loop history — `cat /dev/cu.usbmodem*` with stty + an awk/grep filter limited to specific tags. Wide filters trip the rate-limit on this monitoring harness; keep them tight.
- OTA path is reliable now — `tools/publish_release.sh --skip-build` (after a successful `pio run`) on the dev machine, device pulls via Settings → OTA update.
- For USB flash: stop the serial monitor first or the port will be busy.
