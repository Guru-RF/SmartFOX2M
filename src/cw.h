#include <Arduino.h>

enum CwKeying {
  CW_KEY_TONE,   // 2M: carrier stays on, sidetone keyed on pin 15 for FM mod
  CW_KEY_OUTPUT, // 80M: carrier itself keyed via si5351 output_enable (OOK/CW)
};

void cw(bool state, int hz);
void cw_string_proc(String str, int hz, int duration);
void cwSetVolume(int pct);
void cwSetKeying(CwKeying mode);