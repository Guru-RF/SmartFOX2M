#include "sensors.h"

static int batAdcPin = A2;
static float batDiv = 3.0f;

void sensorsBegin(int pin, float divider) {
  batAdcPin = pin;
  batDiv = divider;
  analogReadResolution(12);
}

float readBattery() {
  uint32_t acc = 0;
  const int N = 8;
  for (int i = 0; i < N; i++)
    acc += analogRead(batAdcPin);
  float raw = (float)acc / N;
  return raw * 3.3f / 4095.0f * batDiv;
}

float readTemp() { return analogReadTemp(); }
