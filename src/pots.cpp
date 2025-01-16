
#include "pots.h"

void ring(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Ring");
    tone(15, 425, 1000);
    delay(1000);
    tone(15, 0, 3000);
    delay(3000);
  }
}

void busy(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Busy");
    tone(15, 425, 500);
    delay(500);
    tone(15, 0, 500);
    delay(500);
  }
}

void congestion(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Congestion");
    tone(15, 425, 167);
    delay(167);
    tone(15, 0, 167);
    delay(167);
  }
}