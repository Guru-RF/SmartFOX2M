#include <Arduino.h>

enum LEDState {
  LED_BOOT,
  LED_IDLE,
  LED_TX,
  LED_LOBAT,
  LED_OFF_SCHED,
  LED_ERROR,
};

void ledBegin(int pin);
void ledSetState(LEDState s);
LEDState ledGetState();
void ledTick();
