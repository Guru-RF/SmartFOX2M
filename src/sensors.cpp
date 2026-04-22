#include "sensors.h"

// Fixed hardware: VSYS sense on GPIO28 through a 100k/100k divider (2:1).
static const int batAdcPin = A2;
static const float batDiv = 2.0f;

void sensorsBegin() {
  analogReadResolution(12);
  // ADC pin + mux need a few reads to settle — otherwise the very first
  // call from the battery watchdog returns garbage.
  for (int i = 0; i < 8; i++) {
    (void)analogRead(batAdcPin);
    delay(1);
  }
}

float readBattery() {
  // Throw away a few samples in case the mux was on a different input.
  for (int i = 0; i < 3; i++) {
    (void)analogRead(batAdcPin);
    delay(1);
  }
  uint32_t acc = 0;
  const int N = 16;
  for (int i = 0; i < N; i++) {
    acc += analogRead(batAdcPin);
    delay(1);
  }
  float raw = (float)acc / N;
  return raw * 3.3f / 4095.0f * batDiv;
}

float readTemp() { return analogReadTemp(); }
