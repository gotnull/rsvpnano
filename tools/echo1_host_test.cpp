// Host-side golden test for the ECHO1 C++ transcription (src/echoform/Echo1).
// Mirrors the fcecho proto.rs test vectors so the two codecs are pinned to
// the same bytes. Build and run on the Mac:
//   clang++ -std=c++17 -I src tools/echo1_host_test.cpp src/echoform/Echo1.cpp \
//     -o /tmp/echo1_test && /tmp/echo1_test
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

#include "echoform/Echo1.h"

using namespace echo1;

static int failures = 0;

#define CHECK(cond)                                             \
  do {                                                          \
    if (!(cond)) {                                              \
      printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);    \
      ++failures;                                               \
    }                                                           \
  } while (0)

int main() {
  // CRC-16/CCITT-FALSE known vector (proto.rs crc16_known_vector).
  CHECK(crc16(reinterpret_cast<const uint8_t *>("123456789"), 9) == 0x29B1);

  // Header layout golden frame: Hello, request 0, seq 0, flags 0, payload
  // {01, 80 05} (version 1, max_payload 1408 LE). Bytes hand-derived from
  // the documented layout; CRC from the pinned polynomial.
  {
    Hello hello{kVersion, static_cast<uint16_t>(kMaxPayload)};
    uint8_t payload[8];
    const size_t pn = hello.encodeTo(payload, sizeof(payload));
    CHECK(pn == 3);
    CHECK(payload[0] == 0x01);
    CHECK(payload[1] == 0x80);  // 1408 = 0x0580 little-endian
    CHECK(payload[2] == 0x05);
    uint8_t frame[64];
    const size_t n =
        encode(MsgType::Hello, 0, 0, 0, payload, pn, frame, sizeof(frame));
    CHECK(n == kHeaderLen + 3 + kCrcLen);
    const uint8_t expectedHeader[12] = {0x45, 0x66, 0x01, 0x01, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x03, 0x00};
    CHECK(memcmp(frame, expectedHeader, 12) == 0);
    // CRC over header+payload must verify.
    CHECK(crc16(frame, n - kCrcLen) ==
          static_cast<uint16_t>(frame[n - 2] | (frame[n - 1] << 8)));
  }

  // Round-trip every message type through the parser, split into single
  // bytes (proto.rs roundtrip_all_message_types + incremental contract).
  {
    const uint8_t types[] = {0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13,
                             0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
                             0x27, 0x28, 0x30, 0x31, 0x40, 0x41, 0x42,
                             0x43, 0x44, 0x45, 0x46, 0x47};
    for (uint8_t t : types) {
      MsgType mt;
      CHECK(msgTypeFromU8(t, mt));
      const uint8_t payload[3] = {1, 2, 3};
      uint8_t frame[64];
      const size_t n = encode(mt, 7, 9, 0, payload, 3, frame, sizeof(frame));
      CHECK(n > 0);
      Parser p;
      for (size_t i = 0; i < n; ++i) {
        p.extend(frame + i, 1);  // worst-case byte split
      }
      FrameHeader h;
      const uint8_t *body = nullptr;
      size_t bodyLen = 0;
      CHECK(p.poll(h, body, bodyLen));
      CHECK(h.msgType == mt);
      CHECK(h.requestId == 7);
      CHECK(h.sequence == 9);
      CHECK(bodyLen == 3 && memcmp(body, payload, 3) == 0);
    }
  }

  // Resync: garbage before and between frames costs nothing but counters.
  {
    uint8_t frame[64];
    const uint8_t payload[4] = {9, 9, 9, 9};
    const size_t n =
        encode(MsgType::Ping, 1, 2, 0, payload, 4, frame, sizeof(frame));
    std::vector<uint8_t> stream;
    const uint8_t junk[] = {0x00, 0x45, 0xFF, 0x66, 0x45};
    stream.insert(stream.end(), junk, junk + sizeof(junk));
    stream.insert(stream.end(), frame, frame + n);
    stream.insert(stream.end(), junk, junk + sizeof(junk));
    stream.insert(stream.end(), frame, frame + n);
    Parser p;
    p.extend(stream.data(), stream.size());
    FrameHeader h;
    const uint8_t *body = nullptr;
    size_t bodyLen = 0;
    int frames = 0;
    while (p.poll(h, body, bodyLen)) {
      ++frames;
      CHECK(h.msgType == MsgType::Ping);
    }
    CHECK(frames == 2);
  }

  // Corrupted CRC is rejected and the stream recovers.
  {
    uint8_t frame[64];
    const size_t n = encode(MsgType::Ping, 1, 0, 0, nullptr, 0, frame,
                            sizeof(frame));
    frame[n - 1] ^= 0xFF;
    uint8_t good[64];
    const size_t gn = encode(MsgType::Pong, 2, 0, 0, nullptr, 0, good,
                             sizeof(good));
    Parser p;
    p.extend(frame, n);
    p.extend(good, gn);
    FrameHeader h;
    const uint8_t *body = nullptr;
    size_t bodyLen = 0;
    CHECK(p.poll(h, body, bodyLen));
    CHECK(h.msgType == MsgType::Pong);
    CHECK(p.stats.badCrc >= 1);
  }

  // SessionConfig payload matches parse_session's expectations.
  {
    SessionConfig cfg;
    cfg.micChannel = 0;
    cfg.captions = true;
    cfg.conversationMemory = true;
    uint8_t out[64];
    const size_t n = cfg.encodeTo(out, sizeof(out));
    CHECK(n == 6);  // channel + flags + four zero-length strings
    CHECK(out[0] == 0);
    CHECK(out[1] == 0x03);
    CHECK(out[2] == 0 && out[3] == 0 && out[4] == 0 && out[5] == 0);
  }

  // RequestTracker id sequencing, gap resync, timeout.
  {
    RequestTracker t(10);
    const uint16_t id = t.begin(0);
    CHECK(id == 1);
    FrameHeader h{};
    h.requestId = id;
    h.sequence = 0;
    CHECK(t.check(h, 1) == SequenceCheck::Ok);
    h.sequence = 5;  // gap
    CHECK(t.check(h, 2) == SequenceCheck::Gap);
    h.sequence = 6;  // resynced
    CHECK(t.check(h, 3) == SequenceCheck::Ok);
    h.requestId = 99;
    CHECK(t.check(h, 4) == SequenceCheck::Stale);
    CHECK(!t.tick(10));
    CHECK(t.tick(20));  // timed out
    uint16_t activeId;
    CHECK(!t.active(activeId));
  }

  if (failures == 0) {
    printf("echo1 host test: ALL PASS\n");
    return 0;
  }
  printf("echo1 host test: %d FAILURES\n", failures);
  return 1;
}
