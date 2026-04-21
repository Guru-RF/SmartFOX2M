#include <Arduino.h>

void runPayload(const char *payload);
void payloadWaitMs(unsigned long ms);
void radioSignal();
void radioNoSignal();
void radioCwMsg(int slot);
void radioCw0();
void radioCw1();
