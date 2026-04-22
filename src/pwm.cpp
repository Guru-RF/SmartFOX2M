#include "pwm.h"
#include <FatFS.h>

PWMAudio pwm(15);

struct WAVFmt {
  uint32_t sampleRate;
  uint16_t bitsPerSample;
  uint16_t channels;
  uint32_t dataSize;
  uint32_t dataOffset;
};

static bool parseWAV(File &f, WAVFmt &fmt) {
  char riff[4];
  uint32_t sz;
  char wave[4];
  if (f.read((uint8_t *)riff, 4) != 4 || memcmp(riff, "RIFF", 4) != 0)
    return false;
  if (f.read((uint8_t *)&sz, 4) != 4)
    return false;
  if (f.read((uint8_t *)wave, 4) != 4 || memcmp(wave, "WAVE", 4) != 0)
    return false;

  bool haveFmt = false;
  while (f.available()) {
    char id[4];
    uint32_t chunkSize;
    if (f.read((uint8_t *)id, 4) != 4)
      return false;
    if (f.read((uint8_t *)&chunkSize, 4) != 4)
      return false;

    if (memcmp(id, "fmt ", 4) == 0) {
      uint8_t buf[16];
      uint32_t toRead = chunkSize < 16 ? chunkSize : 16;
      if (f.read(buf, toRead) != (int)toRead)
        return false;
      fmt.channels = (uint16_t)(buf[2] | (buf[3] << 8));
      fmt.sampleRate = (uint32_t)(buf[4] | (buf[5] << 8) | (buf[6] << 16) |
                                  ((uint32_t)buf[7] << 24));
      fmt.bitsPerSample = (uint16_t)(buf[14] | (buf[15] << 8));
      if (chunkSize > 16)
        f.seek(f.position() + (chunkSize - 16));
      haveFmt = true;
    } else if (memcmp(id, "data", 4) == 0) {
      if (!haveFmt)
        return false;
      fmt.dataSize = chunkSize;
      fmt.dataOffset = f.position();
      return true;
    } else {
      f.seek(f.position() + chunkSize);
    }
  }
  return false;
}

void playWAV(const char *path) {
  File f = FatFS.open(path, "r");
  if (!f) {
    Serial.print("WAV not found: ");
    Serial.println(path);
    return;
  }

  WAVFmt fmt;
  if (!parseWAV(f, fmt)) {
    Serial.println("WAV parse failed");
    f.close();
    return;
  }
  if (fmt.bitsPerSample != 16 || fmt.channels != 1) {
    Serial.print("Unsupported WAV (need 16-bit mono), got ");
    Serial.print(fmt.bitsPerSample);
    Serial.print("-bit ");
    Serial.print(fmt.channels);
    Serial.println("ch");
    f.close();
    return;
  }

  f.seek(fmt.dataOffset);
  pwm.setPin(15);
  pwm.begin(fmt.sampleRate);

  int16_t buf[256];
  uint32_t remaining = fmt.dataSize;
  while (remaining >= 2) {
    uint32_t want = remaining < sizeof(buf) ? remaining : sizeof(buf);
    int got = f.read((uint8_t *)buf, want);
    if (got <= 0)
      break;
    int samples = got / 2;
    for (int i = 0; i < samples; i++) {
      while (pwm.availableForWrite() == 0)
        yield();
      pwm.write(buf[i]);
    }
    remaining -= got;
  }

  while (pwm.availableForWrite() < 16)
    yield();
  pwm.end();
  audioIdle();
  f.close();
}

static void audioSetup() {
  analogWriteRange(1024);
  analogWriteResolution(10);
}

void audioIdle() {
  audioSetup();
  analogWriteFreq(100000);
  analogWrite(15, 512);
}

void audioTone(int hz) {
  if (hz <= 0) {
    audioIdle();
    return;
  }
  audioSetup();
  analogWriteFreq((uint32_t)hz);
  analogWrite(15, 512);
}

void audioTone(int hz, int duty1024) {
  if (hz <= 0 || duty1024 <= 0) {
    audioIdle();
    return;
  }
  if (duty1024 > 1024)
    duty1024 = 1024;
  audioSetup();
  analogWriteFreq((uint32_t)hz);
  analogWrite(15, duty1024);
}

void playPWM() { playWAV("/sample.wav"); }
