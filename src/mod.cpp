#include "mod.h"
#include "pwm.h"
#include <FatFS.h>

extern "C" {
#include "micromod.h"
}

#define MOD_SAMPLE_RATE 22050
#define MOD_BLOCK 128

void playMOD(const char *path) {
  File f = FatFS.open(path, "r");
  if (!f) {
    Serial.print("MOD not found: ");
    Serial.println(path);
    return;
  }

  size_t fsize = f.size();
  if (fsize < 1084) {
    Serial.println("MOD too small");
    f.close();
    return;
  }

  signed char header[1084];
  if (f.read((uint8_t *)header, 1084) != 1084) {
    Serial.println("MOD header read failed");
    f.close();
    return;
  }

  long modLen = micromod_calculate_mod_file_len(header);
  if (modLen <= 0) {
    Serial.println("Not a recognised MOD");
    f.close();
    return;
  }
  if ((size_t)modLen > fsize)
    modLen = fsize;

  signed char *buf = (signed char *)malloc(modLen);
  if (!buf) {
    Serial.print("MOD: out of memory (needs ");
    Serial.print(modLen);
    Serial.println(" bytes)");
    f.close();
    return;
  }

  memcpy(buf, header, 1084);
  if (modLen > 1084) {
    f.seek(1084);
    f.read((uint8_t *)buf + 1084, modLen - 1084);
  }
  f.close();

  if (micromod_initialise(buf, MOD_SAMPLE_RATE) < 0) {
    Serial.println("MOD init failed");
    free(buf);
    return;
  }
  micromod_set_gain(256);

  char songName[24];
  micromod_get_string(0, songName);
  Serial.print("MOD: ");
  Serial.print(songName);
  Serial.print(" (");
  Serial.print(modLen);
  Serial.println(" bytes)");

  long totalSamples = micromod_calculate_song_duration();

  pwm.setPin(15);
  pwm.begin(MOD_SAMPLE_RATE);

  short stereoBuf[MOD_BLOCK * 2];
  long played = 0;
  while (played < totalSamples) {
    long toRender = MOD_BLOCK;
    if (played + toRender > totalSamples)
      toRender = totalSamples - played;
    memset(stereoBuf, 0, (size_t)toRender * 2 * sizeof(short));
    micromod_get_audio(stereoBuf, toRender);
    for (long i = 0; i < toRender; i++) {
      int mono = (int)stereoBuf[i * 2] + (int)stereoBuf[i * 2 + 1];
      if (mono > 32767)
        mono = 32767;
      else if (mono < -32768)
        mono = -32768;
      while (pwm.availableForWrite() == 0)
        yield();
      pwm.write((int16_t)mono);
    }
    played += toRender;
  }

  while (pwm.availableForWrite() < 16)
    yield();
  pwm.end();
  audioIdle();
  free(buf);
}
