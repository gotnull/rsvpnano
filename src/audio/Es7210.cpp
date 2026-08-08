#include "audio/Es7210.h"

#include <Wire.h>

#include "board/BoardConfig.h"

namespace {

// Register map (subset) — names match Espressif's es7210_reg.h.
constexpr uint8_t kRegReset = 0x00;
constexpr uint8_t kRegClockOff = 0x01;
constexpr uint8_t kRegMainClk = 0x02;
constexpr uint8_t kRegLrckDivH = 0x04;
constexpr uint8_t kRegLrckDivL = 0x05;
constexpr uint8_t kRegPowerDown = 0x06;
constexpr uint8_t kRegOsr = 0x07;
constexpr uint8_t kRegModeConfig = 0x08;
constexpr uint8_t kRegTimeControl0 = 0x09;
constexpr uint8_t kRegTimeControl1 = 0x0A;
constexpr uint8_t kRegSdpInterface1 = 0x11;
constexpr uint8_t kRegSdpInterface2 = 0x12;
constexpr uint8_t kRegAdc34Hpf2 = 0x20;
constexpr uint8_t kRegAdc34Hpf1 = 0x21;
constexpr uint8_t kRegAdc12Hpf1 = 0x22;
constexpr uint8_t kRegAdc12Hpf2 = 0x23;
constexpr uint8_t kRegAnalog = 0x40;
constexpr uint8_t kRegMic12Bias = 0x41;
constexpr uint8_t kRegMic34Bias = 0x42;
constexpr uint8_t kRegMic1Gain = 0x43;
constexpr uint8_t kRegMic2Gain = 0x44;
constexpr uint8_t kRegMic3Gain = 0x45;
constexpr uint8_t kRegMic4Gain = 0x46;
constexpr uint8_t kRegMic1Power = 0x47;
constexpr uint8_t kRegMic2Power = 0x48;
constexpr uint8_t kRegMic3Power = 0x49;
constexpr uint8_t kRegMic4Power = 0x4A;
constexpr uint8_t kRegMic12Power = 0x4B;
constexpr uint8_t kRegMic34Power = 0x4C;

// Analog PGA gain code: 3 dB per step below 33 dB (0x0A = +30 dB, the
// reference default). At 33 dB conversational speech still reached whisper
// quiet enough to trigger its silence-hallucination ("thank you for
// watching" in Japanese); 0x0D = +36 dB, paired with 2x digital gain in
// the mic task.
constexpr uint8_t kMicGain = 0x0D;

// Captured CLOCK_OFF value after open + mic select; es7210_start() re-writes
// it to wake the selected ADC pairs.
uint8_t s_clockOffReg = 0x3F;
bool s_started = false;

// ---- Bit-banged I2C -------------------------------------------------------
// The ES7210 datasheet specifies CDATA hold time from CCLK falling as MAX
// 900 ns (Figure 4). The ESP32 hardware I2C holds SDA for microseconds at
// our bus speeds, violating that on every bit — which is why register
// writes landed with random bits dropped and readbacks were garbage. These
// routines drive the shared bus pins directly with sub-microsecond data
// hold. Config-time only; the hardware driver is restored afterwards.

void bbSda(bool high) {
  if (high) {
    pinMode(BoardConfig::PIN_I2C_SDA, INPUT_PULLUP);  // release, pulled up
  } else {
    pinMode(BoardConfig::PIN_I2C_SDA, OUTPUT);
    digitalWrite(BoardConfig::PIN_I2C_SDA, LOW);
  }
}

void bbScl(bool high) {
  if (high) {
    pinMode(BoardConfig::PIN_I2C_SCL, INPUT_PULLUP);
  } else {
    pinMode(BoardConfig::PIN_I2C_SCL, OUTPUT);
    digitalWrite(BoardConfig::PIN_I2C_SCL, LOW);
  }
}

void bbHalf() { delayMicroseconds(3); }  // ~150 kHz with margin

void bbStart() {
  bbSda(true);
  bbScl(true);
  bbHalf();
  bbSda(false);  // SDA falls while SCL high = START
  bbHalf();
  bbScl(false);
  bbHalf();
}

void bbStop() {
  bbSda(false);
  bbHalf();
  bbScl(true);
  bbHalf();
  bbSda(true);  // SDA rises while SCL high = STOP
  bbHalf();
}

// Clock out one bit: set SDA immediately after SCL falls (sub-us hold).
void bbWriteBit(bool bit) {
  bbSda(bit);
  bbHalf();
  bbScl(true);
  bbHalf();
  bbScl(false);
  // SDA updated at the top of the next bit — immediately after this edge.
}

bool bbReadBit() {
  bbSda(true);  // release for slave
  bbHalf();
  bbScl(true);
  bbHalf();
  const bool bit = digitalRead(BoardConfig::PIN_I2C_SDA);
  bbScl(false);
  return bit;
}

// Returns true if the slave ACKed.
bool bbWriteByte(uint8_t byte) {
  for (int i = 7; i >= 0; --i) {
    bbWriteBit((byte >> i) & 1);
  }
  return !bbReadBit();  // ACK = SDA low
}

uint8_t bbReadByte(bool ack) {
  uint8_t value = 0;
  for (int i = 7; i >= 0; --i) {
    value = (value << 1) | (bbReadBit() ? 1 : 0);
  }
  bbWriteBit(!ack);
  return value;
}

// Take the bus pins from the hardware controller / give them back.
void bbAcquireBus() {
  bbSda(true);
  bbScl(true);
  delayMicroseconds(10);
}

void bbReleaseBus() {
  Wire1.begin(BoardConfig::PIN_I2C_SDA, BoardConfig::PIN_I2C_SCL);
  Wire1.setClock(300000);
}

bool writeReg(uint8_t reg, uint8_t value) {
  bbStart();
  bool ok = bbWriteByte(static_cast<uint8_t>(BoardConfig::ES7210_I2C_ADDR << 1));
  ok = ok && bbWriteByte(reg);
  ok = ok && bbWriteByte(value);
  bbStop();
  return ok;
}

bool readReg(uint8_t reg, uint8_t &value) {
  bbStart();
  bool ok = bbWriteByte(static_cast<uint8_t>(BoardConfig::ES7210_I2C_ADDR << 1));
  ok = ok && bbWriteByte(reg);
  bbStart();  // repeated start
  ok = ok && bbWriteByte(static_cast<uint8_t>((BoardConfig::ES7210_I2C_ADDR << 1) | 1));
  if (ok) {
    value = bbReadByte(false);
  }
  bbStop();
  return ok;
}

// Scoped ownership of the bus pins for bit-banged access.
class ScopedBitBangBus {
 public:
  ScopedBitBangBus() { bbAcquireBus(); }
  ~ScopedBitBangBus() { bbReleaseBus(); }
};

}  // namespace

namespace Es7210 {

void dumpRegisters();

bool detect() {
  bbStart();
  const bool acked =
      bbWriteByte(static_cast<uint8_t>(BoardConfig::ES7210_I2C_ADDR << 1));
  bbStop();
  return acked;
}

bool startCapture() {
  if (s_started) return true;
  ScopedBitBangBus busGuard;
  if (!detect()) {
    Serial.println("[es7210] not detected on I2C bus 1");
    return false;
  }

  // Faithful reference open(): full reset then release. The mid-recording
  // mute probe proved writes DO land (readbacks lie — never RMW on this
  // part), yet the clock config behaves unwritten; the reference driver's
  // 0xFF/0x41 reset pulse is what re-latches the clock tree, so keep it
  // exactly. Absolute register values throughout.
  bool ok = true;
  ok &= writeReg(kRegReset, 0xFF);  // full reset
  delay(2);
  ok &= writeReg(kRegReset, 0x41);  // release, normal working mode
  delay(2);
  ok &= writeReg(kRegClockOff, 0x3F);
  ok &= writeReg(kRegTimeControl0, 0x30);  // chip state cycle
  ok &= writeReg(kRegTimeControl1, 0x30);  // power-up state cycle
  ok &= writeReg(kRegAdc12Hpf2, 0x2A);     // HPF quick setup
  ok &= writeReg(kRegAdc12Hpf1, 0x0A);
  ok &= writeReg(kRegAdc34Hpf2, 0x0A);
  ok &= writeReg(kRegAdc34Hpf1, 0x2A);
  // Documented default 0x10 with MS_MODE (bit0) = 0 for slave; earlier
  // absolute-zero write clobbered LRCK_RATE_MODE.
  ok &= writeReg(kRegModeConfig, 0x10);
  ok &= writeReg(kRegAnalog, 0x43);   // analog on, VDDA 3.3 V, VMID 5k
  ok &= writeReg(kRegMic12Bias, 0x70);  // 2.87 V bias
  ok &= writeReg(kRegMic34Bias, 0x70);
  ok &= writeReg(kRegOsr, 0x20);
  // Reference value for 256x-fs MCLK: adc_div=1, dll on, doubler on. The
  // earlier APLL/LRCK-divider experiments were compensating for corrupted
  // writes (hold-time violation, since fixed by bit-banging) — reverted.
  ok &= writeReg(kRegMainClk, 0xC1);
  ok &= writeReg(kRegSdpInterface1, 0x60);  // 16-bit data
  // TDM one-line mode — the configuration xiaozhi (proven on this board)
  // uses: all four ADCs multiplexed on SDOUT1 as 4x16-bit slots. Plain
  // stereo I2S output never produced full-rate data from this part here.
  ok &= writeReg(kRegSdpInterface2, 0x02);

  // Mic path, absolute values, all four ADCs powered (TDM needs the full
  // set). Physical MEMS mics are MIC1/MIC2; MIC3 is the AEC speaker
  // loopback, MIC4 unused — gains set accordingly.
  ok &= writeReg(kRegMic12Power, 0x00);
  ok &= writeReg(kRegMic34Power, 0x00);
  ok &= writeReg(kRegMic1Gain, 0x10 | kMicGain);
  ok &= writeReg(kRegMic2Gain, 0x10 | kMicGain);
  ok &= writeReg(kRegMic3Gain, 0x10);  // reference channel, 0 dB
  ok &= writeReg(kRegMic4Gain, 0x10);

  // Clocks on for all four ADC paths: 0x3F minus the MIC1/2 mask (0x0B)
  // and MIC3/4 mask (0x15), as the reference driver computes them.
  s_clockOffReg = 0x20;
  ok &= writeReg(kRegClockOff, s_clockOffReg);
  ok &= writeReg(kRegPowerDown, 0x00);
  ok &= writeReg(kRegMic1Power, 0x08);
  ok &= writeReg(kRegMic2Power, 0x08);
  ok &= writeReg(kRegMic3Power, 0x08);
  ok &= writeReg(kRegMic4Power, 0x08);
  ok &= writeReg(kRegAnalog, 0x43);
  // Reference driver's start pulse: digital reset strobe, then run.
  ok &= writeReg(kRegReset, 0x71);
  delay(1);
  ok &= writeReg(kRegReset, 0x41);

  if (!ok) {
    Serial.println("[es7210] register write failed during bring-up");
    return false;
  }
  s_started = true;
  Serial.println("[es7210] capture started (MIC1+MIC2, 33 dB, slave I2S)");
  dumpRegisters();
  return true;
}

void dumpRegisters() {
  // Bench diagnostic: read back the whole map so we can see whether the
  // bring-up writes actually stuck (a chip held in reset reads back 0x00s
  // or bus noise).
  String line = "[es7210] regs:";
  for (uint8_t reg = 0x00; reg <= 0x4C; ++reg) {
    uint8_t value = 0;
    if (!readReg(reg, value)) {
      line += " rd-fail@" + String(reg, HEX);
      break;
    }
    char buf[12];
    snprintf(buf, sizeof(buf), " %02X=%02X", reg, value);
    line += buf;
  }
  Serial.println(line);
}

bool debugWriteReg(uint8_t reg, uint8_t value) {
  return writeReg(reg, value);
}

void probeRegisterReadback() {
  constexpr uint8_t kScratchReg = 0x09;  // TIME_CONTROL0 — safe to poke
  const uint8_t patterns[] = {0x30, 0x55, 0xAA, 0x20};
  for (uint8_t pattern : patterns) {
    Wire1.beginTransmission(BoardConfig::ES7210_I2C_ADDR);
    Wire1.write(kScratchReg);
    Wire1.write(pattern);
    const uint8_t writeResult = Wire1.endTransmission();

    // Read A: repeated start (no stop between address write and read).
    uint8_t readRepStart = 0xEE;
    Wire1.beginTransmission(BoardConfig::ES7210_I2C_ADDR);
    Wire1.write(kScratchReg);
    const uint8_t addrResultA = Wire1.endTransmission(false);
    if (Wire1.requestFrom(static_cast<int>(BoardConfig::ES7210_I2C_ADDR), 1) == 1) {
      readRepStart = static_cast<uint8_t>(Wire1.read());
    }

    // Read B: full stop, then read.
    uint8_t readStopped = 0xEE;
    Wire1.beginTransmission(BoardConfig::ES7210_I2C_ADDR);
    Wire1.write(kScratchReg);
    const uint8_t addrResultB = Wire1.endTransmission(true);
    if (Wire1.requestFrom(static_cast<int>(BoardConfig::ES7210_I2C_ADDR), 1) == 1) {
      readStopped = static_cast<uint8_t>(Wire1.read());
    }

    Serial.printf(
        "[es7210] probe reg09: wrote 0x%02X (wr=%u) repstart=0x%02X (a=%u) "
        "stopped=0x%02X (b=%u)\n",
        pattern, writeResult, readRepStart, addrResultA, readStopped,
        addrResultB);
  }
}

void stopCapture() {
  if (!s_started) return;
  ScopedBitBangBus busGuard;
  // es7210_stop()
  writeReg(kRegMic1Power, 0xFF);
  writeReg(kRegMic2Power, 0xFF);
  writeReg(kRegMic3Power, 0xFF);
  writeReg(kRegMic4Power, 0xFF);
  writeReg(kRegMic12Power, 0xFF);
  writeReg(kRegMic34Power, 0xFF);
  writeReg(kRegAnalog, 0xC0);
  writeReg(kRegClockOff, 0x7F);
  writeReg(kRegPowerDown, 0x07);
  s_started = false;
  Serial.println("[es7210] capture stopped");
}

}  // namespace Es7210
