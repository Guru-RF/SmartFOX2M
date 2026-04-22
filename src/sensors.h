#include <Arduino.h>

// Hardware is fixed: VSYS sensed on GPIO28 (ADC2) through a 100k/100k
// resistor divider (ratio 2:1). No need to expose either as config.
void sensorsBegin();
float readBattery();
float readTemp();
