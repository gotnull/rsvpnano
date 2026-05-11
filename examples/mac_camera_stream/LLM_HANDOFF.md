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

The device firmware currently uses snapshot polling, not persistent MJPEG parsing. This was deliberate: snapshot polling is the reliable first working path for ESP32, and the display side is already structured so a later MJPEG parser can feed the same decode/render path.

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
  - Added `fetchCameraSnapshot()` with content-length validation and read timeout.
  - Added `decodeCameraSnapshot()` using `JPEGDecoder`.
  - Added PSRAM-preferred JPEG and RGB565 frame buffers.
  - Added camera serial diagnostics for bytes, decode status, timing, heap, PSRAM, RSSI.
  - Added touch/power exit behavior for the camera screen.
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
5. `DisplayManager::renderCameraRgb565Frame()` aspect-fits that source frame onto the `640 x 172` logical display and writes panel-native stripes.

This is not full 60 FPS end-to-end yet. The renderer is 60 FPS-compatible, but snapshot polling, HTTP setup/teardown, JPEG decode, Wi-Fi, and server encode time are the limiting factors.

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
- For smoother video, the next implementation step is a persistent `/stream.mjpg` parser on-device.
- Classic ESP32 is not a good target for high-resolution webcam streaming. ESP32-S3 with PSRAM is preferred.
- Keep server output conservative:
  - `320 x 240`
  - JPEG quality `50-60`
  - `5-10 fps`
- The camera source frame buffer is capped at `320 x 240`.
- Max compressed JPEG size is capped at `96 KB`.
- Camera buffers are allocated lazily and kept for reuse.
- Touch or power exits the camera stream and returns to Settings.

## Next Best Work

1. Add on-device persistent MJPEG parsing:
   - connect once to `/stream.mjpg`;
   - scan multipart boundaries;
   - parse `Content-Length`;
   - read JPEG payload with timeouts;
   - feed existing `decodeCameraSnapshot()` / `renderCameraRgb565Frame()`.

2. Add a Settings subpage or config file for camera host:
   - current default is compile-time.
   - better long-term location could be `/wifi.json` or `/camera.json`.

3. Add frame-stage timing:
   - HTTP read ms;
   - JPEG decode ms;
   - display render ms;
   - total frame ms.

4. Avoid full RGB565 source-frame storage if needed:
   - decode MCU blocks into a native intermediate stripe cache;
   - this is more complex but saves PSRAM and latency.

5. Commit source changes before the next release.

