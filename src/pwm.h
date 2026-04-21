#include <Arduino.h>
#include <PWMAudio.h>

extern PWMAudio pwm;

void playPWM();
void playWAV(const char *path);
