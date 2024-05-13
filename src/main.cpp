#include "Wire.h"
#include "si5351.h"
#include <Arduino.h>

Si5351 si5351;

// #define PLLB_FREQ 87270000000ULL
#define PLLB_FREQ 86865000000ULL

void setup() {
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  // reset the SI chip
  digitalWrite(3, LOW);
  delay(2000);
  Serial.println("SmartFOX booted !");
  // start the SI chip
  digitalWrite(3, HIGH);
  delay(1);

  Wire1.setSDA(26);
  Wire1.setSCL(27);

  bool i2c_found;

  // Start serial and initialize the Si5351
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_0PF, 0, -220);
  if (!i2c_found) {
    Serial.println("Device not found on I2C bus!");
  }
  si5351.reset();
  delay(200);
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLB);
  si5351.set_vcxo(PLLB_FREQ, 31); // pull weight
  // si5351.set_freq_manual(14545000000ULL, PLLB_FREQ, SI5351_CLK0);
  si5351.set_freq_manual(14477500000ULL, PLLB_FREQ, SI5351_CLK0);
}

void loop() {
  si5351.output_enable(SI5351_CLK0, 1);
  for (int i = 0; i < 5; i++) {
    Serial.println("Ring");
    tone(15, 425, 1000);
    delay(1000);
    tone(15, 0, 3000);
    delay(3000);
  }
  for (int i = 0; i < 5; i++) {
    Serial.println("Busy");
    tone(15, 425, 500);
    delay(500);
    tone(15, 0, 500);
    delay(500);
  }
  // 425/167,0/167
  for (int i = 0; i < 5; i++) {
    Serial.println("Busy");
    tone(15, 425, 167);
    delay(167);
    tone(15, 0, 167);
    delay(167);
  }
  si5351.output_enable(SI5351_CLK0, 0);

  delay(20000);
}