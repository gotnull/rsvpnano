#include "echoform/Echo1.h"

#include <string.h>

namespace echo1 {

bool msgTypeFromU8(uint8_t v, MsgType &out) {
  switch (v) {
    case 0x01: case 0x02: case 0x03:
    case 0x10: case 0x11: case 0x12: case 0x13:
    case 0x20: case 0x21: case 0x22: case 0x23: case 0x24:
    case 0x25: case 0x26: case 0x27: case 0x28:
    case 0x30: case 0x31:
    case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45:
    case 0x46: case 0x47:
      out = static_cast<MsgType>(v);
      return true;
    default:
      return false;
  }
}

uint16_t crc16(const uint8_t *bytes, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; ++i) {
    crc ^= static_cast<uint16_t>(bytes[i]) << 8;
    for (int b = 0; b < 8; ++b) {
      crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                           : static_cast<uint16_t>(crc << 1);
    }
  }
  return crc;
}

namespace {

void putLe16(uint8_t *p, uint16_t v) {
  p[0] = static_cast<uint8_t>(v & 0xFF);
  p[1] = static_cast<uint8_t>(v >> 8);
}

uint16_t getLe16(const uint8_t *p) {
  return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
}

}  // namespace

size_t encode(MsgType msgType, uint16_t requestId, uint16_t sequence,
              uint16_t flags, const uint8_t *payload, size_t payloadLen,
              uint8_t *out, size_t outCap) {
  if (payloadLen > kMaxPayload) return 0;
  const size_t total = kHeaderLen + payloadLen + kCrcLen;
  if (outCap < total) return 0;
  out[0] = kMagic0;
  out[1] = kMagic1;
  out[2] = kVersion;
  out[3] = static_cast<uint8_t>(msgType);
  putLe16(out + 4, requestId);
  putLe16(out + 6, sequence);
  putLe16(out + 8, flags);
  putLe16(out + 10, static_cast<uint16_t>(payloadLen));
  if (payloadLen > 0) {
    memcpy(out + kHeaderLen, payload, payloadLen);
  }
  const uint16_t crc = crc16(out, kHeaderLen + payloadLen);
  putLe16(out + kHeaderLen + payloadLen, crc);
  return total;
}

void Parser::reset() {
  len_ = 0;
  consumed_ = 0;
  stats = ParserStats{};
}

void Parser::compact() {
  if (consumed_ > 0) {
    memmove(buf_, buf_ + consumed_, len_ - consumed_);
    len_ -= consumed_;
    consumed_ = 0;
  }
}

void Parser::dropFront(size_t n) {
  consumed_ = 0;
  memmove(buf_, buf_ + n, len_ - n);
  len_ -= n;
}

size_t Parser::extend(const uint8_t *chunk, size_t len) {
  compact();
  const size_t space = sizeof(buf_) - len_;
  const size_t n = len < space ? len : space;
  memcpy(buf_ + len_, chunk, n);
  len_ += n;
  if (n < len) {
    stats.overflowDrops += static_cast<uint32_t>(len - n);
  }
  return n;
}

bool Parser::poll(FrameHeader &header, const uint8_t *&payload,
                  size_t &payloadLen) {
  compact();
  for (;;) {
    if (len_ < kHeaderLen) {
      // Drop obvious non-magic prefixes so garbage cannot linger.
      while (len_ >= 2 && (buf_[0] != kMagic0 || buf_[1] != kMagic1)) {
        ++stats.badMagic;
        dropFront(1);
      }
      return false;
    }
    if (buf_[0] != kMagic0 || buf_[1] != kMagic1) {
      ++stats.badMagic;
      dropFront(1);
      continue;
    }
    const uint8_t version = buf_[2];
    if (version < kMinVersion || version > kVersion) {
      ++stats.badVersion;
      dropFront(1);
      continue;
    }
    MsgType msgType;
    if (!msgTypeFromU8(buf_[3], msgType)) {
      ++stats.badType;
      dropFront(1);
      continue;
    }
    const size_t bodyLen = getLe16(buf_ + 10);
    if (bodyLen > kMaxPayload) {
      ++stats.badLength;
      dropFront(1);
      continue;
    }
    const size_t total = kHeaderLen + bodyLen + kCrcLen;
    if (len_ < total) {
      return false;  // wait for more bytes
    }
    const uint16_t claimed = getLe16(buf_ + total - 2);
    if (crc16(buf_, total - kCrcLen) != claimed) {
      ++stats.badCrc;
      dropFront(1);
      continue;
    }
    header.version = version;
    header.msgType = msgType;
    header.requestId = getLe16(buf_ + 4);
    header.sequence = getLe16(buf_ + 6);
    header.flags = getLe16(buf_ + 8);
    ++stats.framesOk;
    consumed_ = total;
    payload = buf_ + kHeaderLen;
    payloadLen = bodyLen;
    return true;
  }
}

uint8_t negotiate(uint8_t ours, uint8_t theirs) {
  const uint8_t v = ours < theirs ? ours : theirs;
  return v >= kMinVersion ? v : 0;
}

size_t Hello::encodeTo(uint8_t *out, size_t cap) const {
  if (cap < kWireLen) return 0;
  out[0] = protocolVersion;
  putLe16(out + 1, maxPayload);
  return kWireLen;
}

bool Hello::parse(const uint8_t *payload, size_t len, Hello &out) {
  if (len < kWireLen) return false;
  out.protocolVersion = payload[0];
  out.maxPayload = getLe16(payload + 1);
  return true;
}

bool modelStatusFromU8(uint8_t v, ModelStatusCode &out) {
  if (v > 4) return false;
  out = static_cast<ModelStatusCode>(v);
  return true;
}

size_t SessionConfig::encodeTo(uint8_t *out, size_t cap) const {
  const char *strings[4] = {provider, modelId, voiceId, personaId};
  size_t need = 2;
  size_t lens[4];
  for (int i = 0; i < 4; ++i) {
    lens[i] = strings[i] ? strlen(strings[i]) : 0;
    if (lens[i] > 255) lens[i] = 255;
    need += 1 + lens[i];
  }
  if (cap < need) return 0;
  out[0] = micChannel;
  out[1] = static_cast<uint8_t>((captions ? 1 : 0) |
                                (conversationMemory ? 2 : 0));
  size_t at = 2;
  for (int i = 0; i < 4; ++i) {
    out[at++] = static_cast<uint8_t>(lens[i]);
    if (lens[i] > 0) {
      memcpy(out + at, strings[i], lens[i]);
      at += lens[i];
    }
  }
  return at;
}

uint16_t RequestTracker::begin(uint32_t now) {
  const uint16_t id = nextRequestId_;
  nextRequestId_ = static_cast<uint16_t>(nextRequestId_ + 1);
  if (nextRequestId_ == 0) nextRequestId_ = 1;
  hasActive_ = true;
  activeId_ = id;
  expectedSeq_ = 0;
  lastActivity_ = now;
  return id;
}

SequenceCheck RequestTracker::check(const FrameHeader &header, uint32_t now) {
  if (!hasActive_ || header.requestId != activeId_) {
    return SequenceCheck::Stale;
  }
  lastActivity_ = now;
  if (header.sequence == expectedSeq_) {
    expectedSeq_ = static_cast<uint16_t>(expectedSeq_ + 1);
    return SequenceCheck::Ok;
  }
  ++gaps;
  // Resynchronise onto the stream rather than wedging.
  expectedSeq_ = static_cast<uint16_t>(header.sequence + 1);
  return SequenceCheck::Gap;
}

bool RequestTracker::tick(uint32_t now) {
  if (hasActive_ && (now - lastActivity_) > timeoutTicks_) {
    hasActive_ = false;
    ++timeouts;
    return true;
  }
  return false;
}

}  // namespace echo1
