#!/usr/bin/env python3
"""Low-latency local webcam server for ESP32 MJPEG/snapshot clients."""

from __future__ import annotations

import argparse
import json
import logging
import signal
import socket
import threading
import time
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Optional

import cv2


LOGGER = logging.getLogger("camera_server")


class CameraState:
    def __init__(
        self,
        camera_index: int,
        width: int,
        height: int,
        fps: int,
        jpeg_quality: int,
        overlay: bool,
    ):
        self.camera_index = camera_index
        self.width = width
        self.height = height
        self.fps = fps
        self.jpeg_quality = jpeg_quality
        self.overlay = overlay
        self.latest_jpeg: Optional[bytes] = None
        self.latest_frame_ts = 0.0
        self.frames_captured = 0
        self.capture_errors = 0
        self.last_error = ""
        self.opened = False
        self._condition = threading.Condition()
        self._stop = threading.Event()
        self._thread: Optional[threading.Thread] = None

    def start(self) -> None:
        self._thread = threading.Thread(target=self._capture_loop, name="camera-capture", daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._stop.set()
        with self._condition:
            self._condition.notify_all()
        if self._thread is not None:
            self._thread.join(timeout=3)

    def wait_for_frame(self, last_ts: float, timeout: float = 2.0) -> tuple[Optional[bytes], float]:
        deadline = time.monotonic() + timeout
        with self._condition:
            while not self._stop.is_set():
                if self.latest_jpeg is not None and self.latest_frame_ts != last_ts:
                    return self.latest_jpeg, self.latest_frame_ts
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    break
                self._condition.wait(timeout=remaining)
            return self.latest_jpeg, self.latest_frame_ts

    def snapshot(self) -> tuple[Optional[bytes], float]:
        with self._condition:
            return self.latest_jpeg, self.latest_frame_ts

    def health(self) -> dict:
        age = None
        if self.latest_frame_ts:
            age = max(0.0, time.time() - self.latest_frame_ts)
        return {
            "ok": self.opened and self.latest_jpeg is not None,
            "camera_index": self.camera_index,
            "width": self.width,
            "height": self.height,
            "target_fps": self.fps,
            "jpeg_quality": self.jpeg_quality,
            "overlay": self.overlay,
            "frames_captured": self.frames_captured,
            "capture_errors": self.capture_errors,
            "latest_frame_age_seconds": age,
            "last_error": self.last_error,
        }

    def _capture_loop(self) -> None:
        cap = cv2.VideoCapture(self.camera_index)
        try:
            if not cap.isOpened():
                self.last_error = f"could not open camera index {self.camera_index}"
                LOGGER.error(self.last_error)
                return

            self.opened = True
            cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.width)
            cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.height)
            cap.set(cv2.CAP_PROP_FPS, self.fps)

            encode_params = [int(cv2.IMWRITE_JPEG_QUALITY), self.jpeg_quality]
            frame_interval = 1.0 / max(1, self.fps)
            next_frame_at = time.monotonic()

            while not self._stop.is_set():
                now = time.monotonic()
                if now < next_frame_at:
                    time.sleep(min(0.01, next_frame_at - now))
                    continue
                next_frame_at = now + frame_interval

                ok, frame = cap.read()
                if not ok or frame is None:
                    self.capture_errors += 1
                    self.last_error = "camera read failed"
                    LOGGER.warning(self.last_error)
                    time.sleep(0.25)
                    continue

                if self.width > 0 and self.height > 0:
                    frame = cv2.resize(frame, (self.width, self.height), interpolation=cv2.INTER_AREA)

                if self.overlay:
                    label = f"frame {self.frames_captured + 1}  {time.strftime('%H:%M:%S')}"
                    cv2.rectangle(frame, (4, 4), (185, 30), (0, 0, 0), thickness=-1)
                    cv2.putText(
                        frame,
                        label,
                        (10, 23),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.5,
                        (0, 255, 0),
                        1,
                        cv2.LINE_AA,
                    )
                    phase = (self.frames_captured * 7) % max(1, self.width)
                    cv2.rectangle(frame, (phase, self.height - 10), (min(self.width - 1, phase + 18), self.height - 2), (0, 0, 255), thickness=-1)

                ok, encoded = cv2.imencode(".jpg", frame, encode_params)
                if not ok:
                    self.capture_errors += 1
                    self.last_error = "jpeg encode failed"
                    LOGGER.warning(self.last_error)
                    continue

                with self._condition:
                    self.latest_jpeg = encoded.tobytes()
                    self.latest_frame_ts = time.time()
                    self.frames_captured += 1
                    self.last_error = ""
                    self._condition.notify_all()
        finally:
            self.opened = False
            cap.release()


class CameraRequestHandler(BaseHTTPRequestHandler):
    server_version = "MacCameraMJPEG/1.0"

    def log_message(self, fmt: str, *args: object) -> None:
        LOGGER.info("%s - %s", self.client_address[0], fmt % args)

    @property
    def camera(self) -> CameraState:
        return self.server.camera_state  # type: ignore[attr-defined]

    def do_GET(self) -> None:
        if self.path == "/" or self.path.startswith("/?"):
            self._send_index()
        elif self.path == "/health":
            self._send_json(self.camera.health())
        elif self.path == "/snapshot.jpg":
            self._send_snapshot()
        elif self.path == "/stream.mjpg":
            self._send_stream()
        else:
            self.send_error(HTTPStatus.NOT_FOUND, "not found")

    def _send_index(self) -> None:
        body = f"""<!doctype html>
<html>
<head><title>Mac Camera Stream</title></head>
<body>
  <h1>Mac Camera Stream</h1>
  <p>Status: {"camera open" if self.camera.opened else "camera not open"}</p>
  <ul>
    <li><a href="/snapshot.jpg">snapshot.jpg</a></li>
    <li><a href="/stream.mjpg">stream.mjpg</a></li>
    <li><a href="/health">health</a></li>
  </ul>
  <img src="/stream.mjpg" width="{self.camera.width}" height="{self.camera.height}">
</body>
</html>
"""
        data = body.encode("utf-8")
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def _send_json(self, payload: dict) -> None:
        data = json.dumps(payload, indent=2).encode("utf-8")
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "application/json")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def _send_snapshot(self) -> None:
        jpeg, _ = self.camera.wait_for_frame(0.0, timeout=2.0)
        if jpeg is None:
            self.send_error(HTTPStatus.SERVICE_UNAVAILABLE, "no camera frame available")
            return
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "image/jpeg")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(jpeg)))
        self.end_headers()
        self.wfile.write(jpeg)

    def _send_stream(self) -> None:
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "multipart/x-mixed-replace; boundary=frame")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Connection", "close")
        self.end_headers()

        last_ts = 0.0
        while True:
            jpeg, last_ts = self.camera.wait_for_frame(last_ts, timeout=5.0)
            if jpeg is None:
                continue
            try:
                self.wfile.write(b"--frame\r\n")
                self.wfile.write(b"Content-Type: image/jpeg\r\n")
                self.wfile.write(f"Content-Length: {len(jpeg)}\r\n\r\n".encode("ascii"))
                self.wfile.write(jpeg)
                self.wfile.write(b"\r\n")
                self.wfile.flush()
            except (BrokenPipeError, ConnectionResetError, TimeoutError, OSError):
                LOGGER.info("MJPEG client disconnected: %s", self.client_address[0])
                break


class CameraHTTPServer(ThreadingHTTPServer):
    daemon_threads = True
    allow_reuse_address = True

    def __init__(self, server_address: tuple[str, int], handler_cls: type[BaseHTTPRequestHandler], camera_state: CameraState):
        super().__init__(server_address, handler_cls)
        self.camera_state = camera_state


def local_ip_hint() -> str:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(("8.8.8.8", 80))
            return s.getsockname()[0]
    except OSError:
        return "127.0.0.1"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Serve Mac webcam frames over local HTTP MJPEG.")
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=8080)
    parser.add_argument("--camera-index", type=int, default=0)
    parser.add_argument("--width", type=int, default=320)
    parser.add_argument("--height", type=int, default=240)
    parser.add_argument("--fps", type=int, default=10)
    parser.add_argument("--jpeg-quality", type=int, default=60)
    parser.add_argument(
        "--no-overlay",
        action="store_true",
        help="Disable the frame counter/timestamp overlay burned into each JPEG.",
    )
    return parser.parse_args()


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
    args = parse_args()
    quality = max(1, min(95, args.jpeg_quality))

    camera = CameraState(
        args.camera_index,
        args.width,
        args.height,
        args.fps,
        quality,
        overlay=not args.no_overlay,
    )
    camera.start()

    server = CameraHTTPServer((args.host, args.port), CameraRequestHandler, camera)
    stop_event = threading.Event()

    def handle_signal(signum: int, _frame: object) -> None:
        LOGGER.info("signal %s received, shutting down", signum)
        stop_event.set()
        server.shutdown()

    signal.signal(signal.SIGINT, handle_signal)
    signal.signal(signal.SIGTERM, handle_signal)

    ip = local_ip_hint()
    LOGGER.info(
        "serving camera index=%d size=%dx%d fps=%d jpeg_quality=%d overlay=%s",
        args.camera_index,
        args.width,
        args.height,
        args.fps,
        quality,
        "on" if camera.overlay else "off",
    )
    LOGGER.info("local URL: http://127.0.0.1:%d/stream.mjpg", args.port)
    LOGGER.info("LAN URL hint: http://%s:%d/stream.mjpg", ip, args.port)

    try:
        server.serve_forever(poll_interval=0.25)
    finally:
        server.server_close()
        camera.stop()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
