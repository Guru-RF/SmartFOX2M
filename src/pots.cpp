#include "pots.h"
#include "pwm.h"

void ring(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Ring");
    audioTone(425);
    delay(1000);
    audioIdle();
    delay(3000);
  }
}

void busy(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Busy");
    audioTone(425);
    delay(500);
    audioIdle();
    delay(500);
  }
}

void congestion(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Congestion");
    audioTone(425);
    delay(167);
    audioIdle();
    delay(167);
  }
}
