# Mac Camera Stream to ESP32

Local prototype for streaming a macOS MacBook camera to an ESP32/ESP32-S3 over Wi-Fi/LAN.

The Python server captures webcam frames with OpenCV and serves:

- `/` status page
- `/snapshot.jpg` latest JPEG frame
- `/stream.mjpg` multipart MJPEG stream
- `/health` JSON health endpoint

The ESP32 example starts with snapshot polling because it is easier to debug and more robust on small boards than a long-lived MJPEG parser. The server still exposes MJPEG first so you can test latency in a browser and later replace snapshot polling with stream parsing.

## Layout

```text
examples/mac_camera_stream/
  server/
    camera_server.py
    requirements.txt
  esp32_client/
    platformio.ini
    src/main.cpp
```

This is intentionally separate from the main RSVP Nano firmware.

## macOS Server Setup

From this directory:

```sh
cd examples/mac_camera_stream/server
python3.11 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
python camera_server.py --host 0.0.0.0 --port 8080 --camera-index 0 --width 320 --height 240 --fps 10 --jpeg-quality 60
```

On first run, macOS may ask for camera permission. Allow Terminal, your IDE, or the Python executable that launched the server. If you previously denied access, open:

```text
System Settings -> Privacy & Security -> Camera
```

Then enable the app you are using to run Python.

Find your Mac LAN IP:

```sh
ipconfig getifaddr en0
```

If you are on Ethernet, try:

```sh
ipconfig getifaddr en1
```

Test from the Mac browser:

```text
http://127.0.0.1:8080/stream.mjpg
http://127.0.0.1:8080/snapshot.jpg
http://127.0.0.1:8080/health
```

Test from another device on the same Wi-Fi:

```text
http://YOUR_MAC_LAN_IP:8080/stream.mjpg
```

If another device cannot connect, check macOS firewall settings and confirm both devices are on the same LAN/VLAN.

## Server CLI

```sh
python camera_server.py \
  --host 0.0.0.0 \
  --port 8080 \
  --camera-index 0 \
  --width 320 \
  --height 240 \
  --fps 10 \
  --jpeg-quality 60
```

Defaults:

- host: `0.0.0.0`
- port: `8080`
- camera index: `0`
- width: `320`
- height: `240`
- fps: `10`
- JPEG quality: `60`

The capture loop runs in a background thread. HTTP clients receive the latest encoded JPEG without blocking camera capture.

## ESP32 Client Setup

The main RSVP Nano firmware also has a production-side test entry at:

```text
Settings -> Camera test
```

It uses the normal device Wi-Fi configuration from `/wifi.json` and defaults to:

```text
http://192.168.5.77:8080/snapshot.jpg
```

To build for a different Mac IP, add a build flag in the root `platformio.ini`:

```ini
-DRSVP_CAMERA_SERVER_HOST=\"YOUR_MAC_LAN_IP\"
```

The standalone client below is still useful when testing generic ESP32 boards outside the RSVP Nano firmware.

Edit the constants at the top of `esp32_client/src/main.cpp`:

```cpp
static const char *WIFI_SSID = "YOUR_WIFI_SSID";
static const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
static const char *SERVER_HOST = "192.168.1.100";
static const uint16_t SERVER_PORT = 8080;
static const char *SNAPSHOT_PATH = "/snapshot.jpg";
```

Use your Mac LAN IP for `SERVER_HOST`. Do not use `localhost` from the ESP32.

Build and flash:

```sh
cd examples/mac_camera_stream/esp32_client
pio run -e esp32s3 -t upload
pio device monitor -b 115200
```

For a classic ESP32 dev board:

```sh
pio run -e esp32 -t upload
pio device monitor -b 115200
```

The client logs:

- Wi-Fi status and IP
- server URL
- HTTP status and content length
- bytes received per frame
- JPEG dimensions and MCU size
- decode success/failure
- approximate FPS
- heap and PSRAM before/after decode

## Display Hook

The ESP32 example decodes JPEG frames with `JPEGDecoder` and calls:

```cpp
static void renderJpegBlock(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t *pixels)
```

This is the integration point for a display library. For TFT_eSPI-style libraries it usually becomes:

```cpp
tft.pushImage(x, y, w, h, pixels);
```

The default implementation is a stub and serial diagnostics path, so frame reception and JPEG decode can be verified before wiring a display.

## Memory And Performance Constraints

Start conservative:

- `320x240`
- JPEG quality `50-60`
- `5-10 fps`
- snapshot polling before MJPEG parsing

ESP32-S3 with PSRAM is strongly preferred for smoother display work. Classic ESP32 can receive and decode small JPEGs, but high-resolution webcam streaming is not realistic. Large JPEGs require a full compressed frame buffer plus decoder working memory, and a real display driver may need additional DMA buffers.

If frames drop or decoding fails:

- lower width and height, for example `240x160` or `160x120`
- lower server FPS
- lower JPEG quality
- raise `FRAME_INTERVAL_MS` on the ESP32
- keep `MAX_JPEG_BYTES` conservative
- confirm PSRAM is detected on ESP32-S3
- watch the serial heap logs

The ESP32 client sets HTTP timeouts and does not block forever waiting for broken reads. On Wi-Fi or server failures it retries and prints diagnostics.

## Verification Flow

1. Start the Mac server:

   ```sh
   cd examples/mac_camera_stream/server
   source .venv/bin/activate
   python camera_server.py --host 0.0.0.0 --port 8080 --width 320 --height 240 --fps 10 --jpeg-quality 60
   ```

2. Open a browser on the Mac:

   ```text
   http://127.0.0.1:8080/stream.mjpg
   ```

3. Open a browser from another device on the same LAN:

   ```text
   http://YOUR_MAC_LAN_IP:8080/stream.mjpg
   ```

4. Edit ESP32 Wi-Fi and server constants.

5. Flash the ESP32 and open serial monitor.

6. Confirm logs show:

   ```text
   [wifi] connected
   [http] status=200
   [frame] bytes=... decode=ok
   [stats] ok=... approx_fps=...
   ```

7. Wire `renderJpegBlock()` to your display driver when reception and decode are stable.

## Troubleshooting

`could not open camera index 0`

Check camera permissions and try `--camera-index 1`.

Browser works on Mac but not from ESP32

Use the Mac LAN IP, check firewall prompts, and confirm both devices are on the same Wi-Fi network.

ESP32 logs invalid content length

Make sure `SNAPSHOT_PATH` is `/snapshot.jpg`, not `/stream.mjpg`.

JPEG decode fails

Lower resolution and quality first. Confirm the server URL in serial logs opens as a JPEG in your browser.

Low FPS

The bottleneck may be Wi-Fi, JPEG decode, display push speed, or server encode speed. Compare browser MJPEG smoothness with ESP32 serial `elapsed` and `approx_fps` logs.
