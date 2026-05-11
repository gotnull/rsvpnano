# LLM Handoff: Mac Camera Stream Prototype

## Current State

This repo now contains two related camera-stream implementations:

1. A standalone local prototype under `examples/mac_camera_stream/`.
2. A production firmware integration exposed as `Settings -> Camera test` on the RSVP Nano device.

The Mac server is currently designed around snapshot + MJPEG HTTP:

- `GET /` status page
- `GET /snapshot.jpg` latest JPEG frame
- `GET /stream.mjpg` multipart MJPEG stream
- `GET /health` JSON health

The server now overlays a frame counter/timestamp and moving red bar into every
JPEG by default. This is a deliberate stream diagnostic. If the overlay does
not change on the device, the device is not receiving/rendering subsequent
frames. Pass `--no-overlay` to disable it after the client is proven.

The production device firmware now uses persistent `/stream.mjpg` parsing. The standalone generic ESP32 example still uses snapshot polling because it is the simpler board bring-up path.

## Files Added

Standalone prototype:

- `examples/mac_camera_stream/README.md`
- `examples/mac_camera_stream/LLM_HANDOFF.md`
- `examples/mac_camera_stream/server/camera_server.py`
- `examples/mac_camera_stream/server/requirements.txt`
- `examples/mac_camera_stream/esp32_client/platformio.ini`
- `examples/mac_camera_stream/esp32_client/src/main.cpp`

The standalone ESP32 client is separate from the main firmware. It is useful for generic ESP32 boards and for testing camera fetch/decode without the RSVP Nano app stack.

## Firmware Files Changed

- `platformio.ini`
  - Added `bodmer/JPEGDecoder@^2.0.0`.

- `src/app/AppState.h`
  - Added `AppState::CameraStream`.

- `src/app/App.h`
  - Added camera stream method declarations.
  - Added camera JPEG/frame buffers and counters.

- `src/app/App.cpp`
  - Added HTTP/Wi-Fi/JPEG includes.
  - Added compile-time camera server defaults:
    - `RSVP_CAMERA_SERVER_HOST`, default `"192.168.5.77"`
    - `RSVP_CAMERA_SERVER_PORT`, default `8080`
    - `RSVP_CAMERA_SNAPSHOT_PATH`, default `"/snapshot.jpg"`
  - Added `Settings -> Camera test`.
  - Added `enterCameraStream()`, `exitCameraStream()`, `updateCameraStream()`.
  - Added persistent MJPEG stream parsing via `/stream.mjpg`.
  - Left `fetchCameraSnapshot()` in place as a fallback/reference path.
  - Added `decodeCameraSnapshot()` using `JPEGDecoder`.
  - Added PSRAM-preferred JPEG and RGB565 frame buffers.
  - Added camera serial diagnostics for bytes, decode status, timing, heap, PSRAM, RSSI.
  - Added touch/power exit behavior for the camera screen.
  - Camera exit returns to `SettingsHome` with `Camera test` highlighted,
    matching the way fullscreen demos return to the demo picker.
  - Added a minimal `downloadModStarterPack()` placeholder because the existing Settings menu referenced it but no implementation existed; root firmware would not link without it.

- `src/display/DisplayManager.h`
  - Added `renderCameraRgb565Frame(...)`.

- `src/display/DisplayManager.cpp`
  - Added `renderCameraRgb565Frame(...)`.
  - This follows the screensaver/demo native-stripe architecture:
    - no UI virtual framebuffer;
    - aspect-fit source frame;
    - compose directly into panel-native `txBuffer_`;
    - push large DMA stripes through `drawBitmap()`.

## Architecture Notes

The production camera display path intentionally mirrors the existing high-performance screensaver/demo rendering approach.

Important existing performance facts:

- Logical display is `640 x 172`.
- Panel-native orientation is `172 x 640`.
- `txBuffer_` is DMA-capable internal RAM.
- Demo/screen saver renderers bypass `virtualFrame_` and compose directly into native stripes.
- `kMaxChunkBytes = 16 KB`; with native width `172`, this yields about 47 physical rows per transfer.

Camera rendering currently works like this:

1. Device fetches `/snapshot.jpg`.
2. Compressed JPEG is stored in `cameraJpegBuffer_`.
3. `JPEGDecoder` decodes MCU blocks.
4. MCU blocks are copied into `cameraFrameBuffer_` as normal RGB565, logical orientation.
5. `DisplayManager::renderCameraRgb565Frame()` aspect-fits that source frame onto the `640 x 172` logical display, applies a 180-degree camera correction, and writes panel-native stripes.

This is not full 60 FPS end-to-end yet. The renderer is 60 FPS-compatible, but JPEG decode, Wi-Fi throughput, server encode time, and display flush time are the limiting factors.

## Current Defaults

Mac server command:

```sh
cd examples/mac_camera_stream/server
source .venv/bin/activate
python camera_server.py --host 0.0.0.0 --port 8080 --camera-index 0 --width 320 --height 240 --fps 10 --jpeg-quality 60
```

Current server observed during setup:

- LAN IP: `192.168.5.77`
- Stream URL: `http://192.168.5.77:8080/stream.mjpg`
- Snapshot URL: `http://192.168.5.77:8080/snapshot.jpg`

Firmware default matches that IP:

```cpp
#define RSVP_CAMERA_SERVER_HOST "192.168.5.77"
```

If the Mac IP changes, add or edit a root `platformio.ini` build flag:

```ini
-DRSVP_CAMERA_SERVER_HOST=\"YOUR_MAC_LAN_IP\"
```

## Wi-Fi Assumptions

The production firmware camera test does not define SSID/password constants. It reuses the existing RSVP Nano Wi-Fi config loaded from `/wifi.json` and cached in NVS.

If `Settings -> Camera test` shows Wi-Fi failure, check:

- SD card contains `/wifi.json`;
- network entry is valid;
- Mac and device are on the same LAN/VLAN;
- macOS firewall allows inbound connections to Python;
- server is running with `--host 0.0.0.0`.

## Build And Deploy

Root firmware build:

```sh
~/.platformio/penv/bin/pio run
```

This was verified successfully after the camera integration.

Published OTA release:

```text
https://github.com/gotnull/rsvpnano/releases/tag/ota-20260511060028
```

Additional camera-stream OTA releases published during debugging:

```text
ota-20260511061048 - return/menu handling fix
ota-20260511062031 - MJPEG stream parser and 180-degree camera orientation fix
ota-20260511062323 - frame marker / stream diagnostics
ota-20260511062802 - screensaver/demo/camera dismiss touch cleanup
ota-20260511062933 - camera opening touch guard
ota-20260511063529 - suppress original menu-selection touch until release
ota-20260511063835 - camera tap exit returns to SettingsHome with Camera test highlighted
ota-20260511064626 - stream liveness fix: short MJPEG frame timeout plus snapshot fallback
ota-20260511064920 - camera touch preemption: poll touch during camera network reads
```

Latest OTA URL:

```text
https://github.com/gotnull/rsvpnano/releases/latest/download/firmware.bin
```

Publish command used:

```sh
tools/publish_release.sh --notes "Camera stream test build: Settings Camera test with snapshot JPEG decode and native-stripe display path."
```

Important: the GitHub release binary was built from the local working tree. At the time of this handoff, source changes may still be uncommitted unless the user committed them later.

## Verification Already Done

Python server:

- Created venv in `examples/mac_camera_stream/server/.venv`.
- Installed `opencv-python` and `numpy`.
- Started server successfully.
- Verified `/health` returned `ok: true`.
- Verified `/snapshot.jpg` returned HTTP `200`.

Standalone ESP32 example:

```sh
~/.platformio/penv/bin/pio run -e esp32s3
~/.platformio/penv/bin/pio run -e esp32
```

Both built successfully.

Production firmware:

```sh
~/.platformio/penv/bin/pio run
```

Built successfully for `waveshare_esp32s3_usb_msc`.

## Known Constraints

- Current production device path uses `/snapshot.jpg` polling. It is robust but not optimal for smooth video.
- The production firmware uses persistent `/stream.mjpg`; if streaming stalls, inspect serial logs for stream connect, content length, payload timeout, and decode failures.
- Current camera liveness behavior: if MJPEG frame parsing does not produce a
  frame within `kCameraStreamFrameTimeoutMs`, firmware closes the stream and
  immediately tries `/snapshot.jpg` as a fallback frame. This keeps the display
  moving and makes parser stalls visible in serial logs.
- Camera touch exit is now preemptive. `pollCameraExitTouch()` runs before each
  camera update and inside the blocking stream/body-read loops so a tap can
  abort camera I/O and return to Settings even if the frame path is stalled.
- Classic ESP32 is not a good target for high-resolution webcam streaming. ESP32-S3 with PSRAM is preferred.
- Keep server output conservative:
  - `320 x 240`
  - JPEG quality `50-60`
  - `5-10 fps`
- The camera source frame buffer is capped at `320 x 240`.
- Max compressed JPEG size is capped at `96 KB`.
- Camera buffers are allocated lazily and kept for reuse.
- Camera Test is intentionally a Settings feature: touch or power exits the
  stream and returns to `SettingsHome` with `Camera test` highlighted. This
  mirrors demos returning to their picker and avoids falling back to the reader
  after a fullscreen test view.

## Next Best Work

1. Add a Settings subpage or config file for camera host:
   - current default is compile-time.
   - better long-term location could be `/wifi.json` or `/camera.json`.

2. Add frame-stage timing:
   - HTTP read ms;
   - JPEG decode ms;
   - display render ms;
   - total frame ms.

3. Avoid full RGB565 source-frame storage if needed:
   - decode MCU blocks into a native intermediate stripe cache;
   - this is more complex but saves PSRAM and latency.

4. Commit source changes before the next release.
