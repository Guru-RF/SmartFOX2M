#include "pwm.h"

// The sample pointers
const int16_t *start = (const int16_t *)out_raw;
const int16_t *p = start;

unsigned int count = 0;
PWMAudio pwm(15);

void cb() {
  while (pwm.availableForWrite()) {
    pwm.write(*p++);
    count += 2;
    if (count >= sizeof(out_raw)) {
      count = 0;
      p = start;
      pwm.end();
      break;
    }
  }
}

void playPWM() {
  pwm.setPin(15);
  pwm.onTransmit(cb);
  pwm.begin(44100);
}