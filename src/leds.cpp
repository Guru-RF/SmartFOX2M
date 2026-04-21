#include "leds.h"

static int ledPin = -1;
static LEDState state = LED_BOOT;
static unsigned long lastChange = 0;
static int phase = 0;

static void apply(bool on) {
  if (ledPin >= 0)
    digitalWrite(ledPin, on ? HIGH : LOW);
}

void ledBegin(int pin) {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);
  apply(true);
  state = LED_BOOT;
  lastChange = millis();
  phase = 0;
}

void ledSetState(LEDState s) {
  if (s != state) {
    state = s;
    phase = 0;
    lastChange = millis();
  }
}

LEDState ledGetState() { return state; }

void ledTick() {
  unsigned long now = millis();
  unsigned long dt = now - lastChange;

  switch (state) {
  case LED_BOOT: {
    if (dt >= 100) {
      phase ^= 1;
      apply(phase);
      lastChange = now;
    }
    break;
  }
  case LED_IDLE: {
    static const unsigned long pat[] = {50, 1950};
    if (dt >= pat[phase]) {
      phase = (phase + 1) % 2;
      apply(phase == 0);
      lastChange = now;
    }
    break;
  }
  case LED_TX:
    apply(true);
    break;
  case LED_LOBAT: {
    static const unsigned long pat[] = {80, 120, 80, 1700};
    if (dt >= pat[phase]) {
      phase = (phase + 1) % 4;
      apply(phase == 0 || phase == 2);
      lastChange = now;
    }
    break;
  }
  case LED_OFF_SCHED: {
    static const unsigned long pat[] = {30, 2970};
    if (dt >= pat[phase]) {
      phase = (phase + 1) % 2;
      apply(phase == 0);
      lastChange = now;
    }
    break;
  }
  case LED_ERROR: {
    static const unsigned long pat[] = {150, 150, 150, 150, 150, 600,
                                        450, 150, 450, 150, 450, 600,
                                        150, 150, 150, 150, 150, 1200};
    if (dt >= pat[phase]) {
      phase = (phase + 1) % 18;
      apply(phase % 2 == 0);
      lastChange = now;
    }
    break;
  }
  }
}
