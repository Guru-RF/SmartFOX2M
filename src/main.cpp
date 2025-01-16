#include "FS.h"
#include "Wire.h"
#include "cw.h"
#include "music.h"
#include "pots.h"
#include "si5351.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FatFS.h>
#include <FatFSUSB.h>

Si5351 si5351;
#define PLLB_FREQ 86865000000ULL

volatile bool updated = false;
volatile bool driveConnected = false;
volatile bool inPrinting = false;
const char *cw_message;
const char *payload;
volatile int cw_speed;
volatile int cw_freq;

void unplug(uint32_t i) {
  (void)i;
  driveConnected = false;
  updated = true;
  rp2040.reboot();
}

void plug(uint32_t i) {
  (void)i;
  driveConnected = true;
  FatFS.end();
}

bool mountable(uint32_t i) {
  (void)i;
  return !inPrinting;
}

void cwmsg() { cw_string_proc(cw_message, cw_freq, cw_speed); }

void cw0() { cw(false, cw_freq); }

void cw1() { cw(true, cw_freq); }

void nosignal() { si5351.output_enable(SI5351_CLK0, 0); }
void signal() { si5351.output_enable(SI5351_CLK0, 1); }

void processPayload(const char *payload) {
  Serial.println("Processing payload...");

  char buffer[256];
  strncpy(buffer, payload, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, ",");
  while (token != nullptr) {
    char *endPtr;
    long delayValue = strtol(token, &endPtr, 10);

    if (*endPtr == '\0') {
      Serial.print("Delay: ");
      Serial.print(delayValue);
      Serial.println(" seconds");
      delay(delayValue * 1000);
    } else {
      Serial.print("Function triggered: ");
      Serial.println(token);

      if (strcmp(token, "nokia") == 0)
        nokia();
      else if (strcmp(token, "doom") == 0)
        doom();
      else if (strcmp(token, "supermario") == 0)
        supermario();
      else if (strcmp(token, "pacman") == 0)
        pacman();
      else if (strcmp(token, "babyelephantwalk") == 0)
        babyelephantwalk();
      else if (strcmp(token, "ring") == 0)
        ring(5);
      else if (strcmp(token, "busy") == 0)
        busy(5);
      else if (strcmp(token, "congestion") == 0)
        congestion(5);
      else if (strcmp(token, "cw0") == 0)
        cw0();
      else if (strcmp(token, "cw1") == 0)
        cw1();
      else if (strcmp(token, "cwmsg") == 0)
        cwmsg();
      else if (strcmp(token, "nosignal") == 0)
        nosignal();
      else if (strcmp(token, "signal") == 0)
        signal();
      else {
        Serial.print("Unknown function: ");
        Serial.println(token);
      }
    }

    token = strtok(nullptr, ",");
  }
}

void setup() {
  JsonDocument doc;
  delay(2000);

  if (!FatFS.begin()) {
    Serial.println("FatFS initialization failed!");
    while (1) {
      delay(1);
    }
  }
  Serial.println("FatFS initialization done.");

  const char *filePath = "/config.json";

  if (FatFS.exists(filePath)) {
    Serial.println("File exists: " + String(filePath));

    File file = FatFS.open(filePath, "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
      Serial.print("Failed to parse config file: ");
      Serial.println(error.c_str());
      return;
    }
  } else {
    Serial.println("File does not exist: " + String(filePath));
    doc["fmFreq"] = "145.450";
    doc["fmOffset"] = "30000";
    doc["cwSpeed"] = "75";
    doc["cwFreq"] = "750";
    doc["cwMessage"] = "VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS "
                       "A NAGOYA MINI VERTICAL ";
    doc["payload"] =
        "signal,5,ring,1,busy,1,congestion,1,nokia,1,pacman,1,doom,1,"
        "supermario,1,babyelephantwalk,1,cw0,2,cw1,1,cwmsg,nosignal,20";

    File file = FatFS.open(filePath, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    if (serializeJson(doc, file) == 0) {
      Serial.println("Failed to write to file");
    } else {
      Serial.println("Config saved successfully");
    }

    file.close();
  }

  FatFSUSB.onUnplug(unplug);
  FatFSUSB.onPlug(plug);
  FatFSUSB.driveReady(mountable);
  FatFSUSB.begin();

  Serial.println("FatFSUSB started.");
  Serial.println("Connect drive via USB to modify config !");

  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);

  // reset the SI chip
  digitalWrite(3, LOW);
  // start the SI chip
  digitalWrite(3, HIGH);

  Wire1.setSDA(26);
  Wire1.setSCL(27);
  delay(10);

  bool i2c_found;

  const char *offsetStr = doc["fmOffset"];
  int32_t fmOffset = atoi(offsetStr);

  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_0PF, 250000000, fmOffset);
  if (!i2c_found) {
    Serial.println("Device not found on I2C bus!");
  }

  si5351.reset();
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLB);

  const char *freqStr = doc["fmFreq"];
  double freqMHz = atof(freqStr);
  uint64_t freqHz = (uint64_t)(freqMHz * 1e8);

  Serial.print("Frequency in Hz: ");
  Serial.println(freqHz);

  si5351.pll_reset(SI5351_PLLB);

  si5351.set_vcxo(PLLB_FREQ, 50);

  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.set_freq(freqHz, SI5351_CLK0);

  si5351.output_enable(SI5351_CLK0, 1);
  si5351.set_clock_pwr(SI5351_CLK1, 0);
  si5351.set_clock_pwr(SI5351_CLK2, 0);
  si5351.set_clock_pwr(SI5351_CLK3, 0);
  si5351.set_clock_pwr(SI5351_CLK4, 0);
  si5351.set_clock_pwr(SI5351_CLK5, 0);
  si5351.set_clock_pwr(SI5351_CLK6, 0);
  si5351.set_clock_pwr(SI5351_CLK7, 0);

  cw_message = doc["cwMessage"];
  payload = doc["payload"];
  const char *cwFreq = doc["cwFreq"];
  cw_freq = atoi(cwFreq);
  const char *cwSpeed = doc["cwSpeed"];
  cw_speed = atoi(cwSpeed);

  Serial.println("SmartFOX2M booted !");
}

void loop() { processPayload(payload); }