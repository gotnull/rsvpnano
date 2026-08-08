#pragma once

#include <stddef.h>
#include <stdint.h>

// ECHO1 wire protocol - C++ transcription of rusty-nail's
// crates/fcecho/src/proto.rs (the single source of truth; the relay links
// that crate). Byte-for-byte compatible: little-endian header, CRC-16/
// CCITT-FALSE trailer, incremental parser with single-byte resync.
// Deliberately Arduino-free so the codec golden-tests on the host.
//
// Frame layout (little-endian):
//   offset  size  field
//   0       2     magic "Ef" (0x45 0x66)
//   2       1     protocol version (1)
//   3       1     message type
//   4       2     request id
//   6       2     sequence number
//   8       2     flags
//   10      2     payload length (0..=kMaxPayload)
//   12      n     payload
//   12+n    2     CRC-16/CCITT-FALSE over bytes 0..12+n
namespace echo1 {

constexpr uint8_t kVersion = 1;
constexpr uint8_t kMinVersion = 1;
constexpr uint8_t kMagic0 = 0x45;  // 'E'
constexpr uint8_t kMagic1 = 0x66;  // 'f'
constexpr size_t kHeaderLen = 12;
constexpr size_t kCrcLen = 2;
constexpr size_t kMaxPayload = 1408;
constexpr size_t kMaxFrame = kHeaderLen + kMaxPayload + kCrcLen;

// Wire values are append-only; never renumber (proto.rs RelayMessageType).
enum class MsgType : uint8_t {
  Hello = 0x01,
  HelloAck = 0x02,
  ConfigureSession = 0x03,
  BeginUtterance = 0x10,
  AudioPcm16 = 0x11,
  EndUtterance = 0x12,
  CancelRequest = 0x13,
  TranscriptPartial = 0x20,
  TranscriptFinal = 0x21,
  ModelStatus = 0x22,
  TextDelta = 0x23,
  SpeechPcm16 = 0x24,
  EmotionHint = 0x25,
  Usage = 0x26,
  Completed = 0x27,
  Error = 0x28,
  Ping = 0x30,
  Pong = 0x31,
  ListVoices = 0x40,
  VoiceList = 0x41,
  PreviewVoice = 0x42,
  PreviewPcm16 = 0x43,
  PreviewDone = 0x44,
  CancelPreview = 0x45,
  AmbientFeatures = 0x46,
  NowPlaying = 0x47,
};

// Returns true if v is a known wire value (proto.rs from_u8).
bool msgTypeFromU8(uint8_t v, MsgType &out);

struct FrameHeader {
  uint8_t version;
  MsgType msgType;
  uint16_t requestId;
  uint16_t sequence;
  uint16_t flags;
};

// CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF). crc16("123456789")==0x29B1.
uint16_t crc16(const uint8_t *bytes, size_t len);

// Encode one frame into out (capacity outCap). Returns frame length, or 0 on
// payload-too-long / buffer-too-small (proto.rs encode).
size_t encode(MsgType msgType, uint16_t requestId, uint16_t sequence,
              uint16_t flags, const uint8_t *payload, size_t payloadLen,
              uint8_t *out, size_t outCap);

struct ParserStats {
  uint32_t framesOk = 0;
  uint32_t badMagic = 0;
  uint32_t badVersion = 0;
  uint32_t badType = 0;
  uint32_t badLength = 0;
  uint32_t badCrc = 0;
  uint32_t overflowDrops = 0;
};

// Incremental frame parser: feed bytes with extend(), drain frames with
// poll(). Never trusts lengths, resynchronises by single-byte discard.
class Parser {
 public:
  Parser() { reset(); }
  void reset();
  // Append transport bytes; returns how many were accepted.
  size_t extend(const uint8_t *chunk, size_t len);
  // Extract the next valid frame. Payload pointer borrows the internal
  // buffer until the next extend/poll call. Returns false = need more bytes.
  bool poll(FrameHeader &header, const uint8_t *&payload, size_t &payloadLen);

  ParserStats stats;

 private:
  void compact();
  void dropFront(size_t n);

  uint8_t buf_[kMaxFrame * 2];
  size_t len_ = 0;
  size_t consumed_ = 0;
};

// Version negotiation: highest version both sides speak (proto.rs negotiate).
// Returns 0 if no overlap.
uint8_t negotiate(uint8_t ours, uint8_t theirs);

// Hello / HelloAck payload.
struct Hello {
  uint8_t protocolVersion;
  uint16_t maxPayload;

  static constexpr size_t kWireLen = 3;
  size_t encodeTo(uint8_t *out, size_t cap) const;
  static bool parse(const uint8_t *payload, size_t len, Hello &out);
};

enum class ModelStatusCode : uint8_t {
  Idle = 0,
  Transcribing = 1,
  Thinking = 2,
  Synthesising = 3,
  Streaming = 4,
};
bool modelStatusFromU8(uint8_t v, ModelStatusCode &out);

// ConfigureSession payload (fcecho::config::encode_session/parse_session):
//   [0] mic_channel (0 = left, 1 = right)
//   [1] flags (bit0 captions, bit1 conversation_memory)
//   then 4 length-prefixed strings: provider, model_id, voice_id, persona_id
//   (empty string = relay default).
struct SessionConfig {
  uint8_t micChannel = 0;
  bool captions = true;
  bool conversationMemory = true;
  const char *provider = "";
  const char *modelId = "";
  const char *voiceId = "";
  const char *personaId = "";

  size_t encodeTo(uint8_t *out, size_t cap) const;
};

// Request lifecycle tracker (proto.rs RequestTracker). Times are in caller
// ticks; timeoutTicks in the same unit.
enum class SequenceCheck { Ok, Gap, Stale };

class RequestTracker {
 public:
  explicit RequestTracker(uint32_t timeoutTicks)
      : timeoutTicks_(timeoutTicks) {}

  bool active(uint16_t &id) const {
    if (!hasActive_) return false;
    id = activeId_;
    return true;
  }
  // Begin a new request; returns its id (caller must cancel any prior one).
  uint16_t begin(uint32_t now);
  void cancel() { hasActive_ = false; }
  void complete() { hasActive_ = false; }
  // Validate an inbound frame against the active request.
  SequenceCheck check(const FrameHeader &header, uint32_t now);
  // Tick; returns true exactly once when the active request times out.
  bool tick(uint32_t now);

  uint32_t timeouts = 0;
  uint32_t gaps = 0;

 private:
  uint16_t nextRequestId_ = 1;
  bool hasActive_ = false;
  uint16_t activeId_ = 0;
  uint16_t expectedSeq_ = 0;
  uint32_t lastActivity_ = 0;
  uint32_t timeoutTicks_;
};

}  // namespace echo1
