#include "FS.h"
#include "Wire.h"
#include "cw.h"
#include "music.h"
#include "pots.h"
#include "pwm.h"
#include "si5351.h"
#include <Arduino.h>
#include <FatFS.h>
#include <FatFSUSB.h>

struct Config {
  String fmFreq;
  String fmOffset;
  String cwSpeed;
  String cwFreq;
  String cwMessage;
  String payload;
} config;

bool yamlParse(const char *yaml, Config &cfg) {
  String line;
  String payloadBlock = "";
  bool inPayload = false;

  while (*yaml) {
    line = "";
    while (*yaml && *yaml != '\n')
      line += *yaml++;
    if (*yaml == '\n')
      yaml++;

    line.trim();
    if (line.startsWith("#") || line.length() == 0)
      continue;

    if (inPayload) {
      if (line.startsWith("#"))
        continue;
      payloadBlock += line + "\n";
      continue;
    }

    int colonIndex = line.indexOf(':');
    if (colonIndex > 0) {
      String key = line.substring(0, colonIndex);
      String value = line.substring(colonIndex + 1);
      key.trim();
      value.trim();

      if (value == "|") {
        if (key == "payload")
          inPayload = true;
        continue;
      }

      value.replace("\"", "");
      if (value.startsWith("\"") && value.endsWith("\"")) {
        value = value.substring(1, value.length() - 1);
      }

      if (key == "fmFreq")
        cfg.fmFreq = value;
      else if (key == "fmOffset")
        cfg.fmOffset = value;
      else if (key == "cwSpeed")
        cfg.cwSpeed = value;
      else if (key == "cwFreq")
        cfg.cwFreq = value;
      else if (key == "cwMessage")
        cfg.cwMessage = value;
      else if (key == "payload")
        cfg.payload = value;
    }
  }

  if (payloadBlock.length() > 0) {
    cfg.payload = payloadBlock;
  }

  return true;
}

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
  driveConnected = true;
  return !inPrinting;
}

void cwmsg() { cw_string_proc(cw_message, cw_freq, cw_speed); }
void cw0() { cw(false, cw_freq); }
void cw1() { cw(true, cw_freq); }
void nosignal() { si5351.output_enable(SI5351_CLK0, 0); }
void signal() { si5351.output_enable(SI5351_CLK0, 1); }

void processPayload(const char *payload) {
  Serial.println("Processing payload...");
  char buffer[1024];
  strncpy(buffer, payload, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';
  char *token = strtok(buffer, "\n");

  while (token != nullptr) {
    while (*token == ' ')
      token++;
    if (*token == '#' || strlen(token) == 0) {
      token = strtok(nullptr, "\n");
      continue;
    }

    char *commentPos = strchr(token, '#');
    if (commentPos)
      *commentPos = '\0';

    String cmd = String(token);
    cmd.trim();
    if (cmd.length() == 0) {
      token = strtok(nullptr, "\n");
      continue;
    }

    if (cmd.startsWith("wait ")) {
      int seconds = cmd.substring(5).toInt();
      Serial.print("Wait: ");
      Serial.print(seconds);
      Serial.println(" seconds");
      delay(seconds * 1000);
    } else {
      Serial.print("Function triggered: ");
      Serial.println(cmd);

      if (cmd == "nokia")
        nokia();
      else if (cmd == "doom")
        doom();
      else if (cmd == "supermario")
        supermario();
      else if (cmd == "pwm")
        playPWM();
      else if (cmd == "pacman")
        pacman();
      else if (cmd == "babyelephantwalk")
        babyelephantwalk();
      else if (cmd == "ring")
        ring(5);
      else if (cmd == "busy")
        busy(5);
      else if (cmd == "congestion")
        congestion(5);
      else if (cmd == "cw0")
        cw0();
      else if (cmd == "cw1")
        cw1();
      else if (cmd == "cwmsg")
        cwmsg();
      else if (cmd == "nosignal")
        nosignal();
      else if (cmd == "signal")
        signal();
      else {
        Serial.print("Unknown function: ");
        Serial.println(cmd);
      }
    }

    token = strtok(nullptr, "\n");
  }
}

void setup() {
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);

  delay(2000);
  if (!FatFS.begin()) {
    Serial.println("FatFS initialization failed!");
    while (1)
      delay(1);
  }
  fatfs::f_setlabel("SMARTFOX");
  Serial.println("FatFS initialized.");

  const char *filePath = "/config.yaml";

  if (FatFS.exists(filePath)) {
    Serial.println("File exists: " + String(filePath));
    File file = FatFS.open(filePath, "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    String yamlContent;
    while (file.available())
      yamlContent += (char)file.read();
    file.close();

    // Trim leading/trailing whitespace just in case
    yamlContent.trim();

    // Check if it's a firmware update trigger
    if (yamlContent == "firmwareupdate") {
      Serial.println("Firmware update requested.");

      // Attempt to remove the config file
      if (FatFS.remove(filePath)) {
        Serial.println("Config file removed successfully.");
      } else {
        Serial.println("Failed to remove config file.");
      }

      delay(500); // Let the serial message flush
      Serial.println("Rebooting into UF2 bootloader...");
      delay(500);
      reset_usb_boot(0, 0); // Enter UF2 bootloader mode
      while (true)
        ; // Just in case
    }

    if (!yamlParse(yamlContent.c_str(), config)) {
      Serial.println("Failed to parse YAML config. Rewriting default config.");
      FatFS.remove(filePath);
    } else {
      goto configLoaded;
    }
  }

  {
    File file = FatFS.open(filePath, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    String defaultConfig =
        "# Frequency in MHz\n"
        "fmFreq: \"145.450\"\n"
        "# Offset in Hz\n"
        "fmOffset: \"30000\"\n"
        "# CW speed in WPM\n"
        "cwSpeed: \"75\"\n"
        "# CW tone frequency in Hz\n"
        "cwFreq: \"750\"\n"
        "# Morse message to transmit\n"
        "cwMessage: \"VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS A "
        "NAGOYA MINI VERTICAL\"\n"
        "# Sequence of actions (multiline block, each line is a step)\n"
        "payload: |\n"
        "  # Enable signal output\n"
        "  signal\n"
        "  # Wait 5 seconds\n"
        "  wait 5\n"
        "  # PWM - plays a short audio sample\n"
        "  pwm\n"
        "  wait 1\n"
        "  # PSTN ring tone\n"
        "  ring\n"
        "  wait 1\n"
        "  # PSTN busy tone\n"
        "  busy\n"
        "  wait 1\n"
        "  # PSTN congestion tone\n"
        "  congestion\n"
        "  wait 1\n"
        "  # Nokia ringtone\n"
        "  nokia\n"
        "  wait 1\n"
        "  # Pacman tune\n"
        "  pacman\n"
        "  wait 1\n"
        "  # Doom tune\n"
        "  doom\n"
        "  wait 1\n"
        "  # Super Mario Bros tune\n"
        "  supermario\n"
        "  wait 1\n"
        "  # Baby Elephant Walk tune\n"
        "  babyelephantwalk\n"
        "  wait 1\n"
        "  # CW: nothing (tone off)\n"
        "  cw0\n"
        "  wait 2\n"
        "  # CW: keydown (carrier)\n"
        "  cw1\n"
        "  wait 1\n"
        "  # CW: play programmed message\n"
        "  cwmsg\n"
        "  # Disable signal output (no carrier)\n"
        "  nosignal\n"
        "  wait 20\n";

    file.print(defaultConfig);
    file.close();
    yamlParse(defaultConfig.c_str(), config);
  }

configLoaded:
  FatFSUSB.onUnplug(unplug);
  FatFSUSB.onPlug(plug);
  FatFSUSB.driveReady(mountable);
  FatFSUSB.begin();

  Serial.println("FatFSUSB started.");
  Serial.println("Connect drive via USB to modify config !");

  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Wire1.setSDA(26);
  Wire1.setSCL(27);
  delay(10);

  int32_t fmOffset = config.fmOffset.toInt();
  bool i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_0PF, 250000000, fmOffset);
  if (!i2c_found)
    Serial.println("Device not found on I2C bus!");

  si5351.reset();
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLB);

  double freqMHz = config.fmFreq.toFloat();
  uint64_t freqHz = (uint64_t)(freqMHz * 1e8);

  Serial.print("Frequency in Hz: ");
  Serial.println(freqHz);

  si5351.pll_reset(SI5351_PLLB);
  si5351.set_vcxo(PLLB_FREQ, 50);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.set_freq(freqHz, SI5351_CLK0);
  si5351.output_enable(SI5351_CLK0, 1);

  for (int i = 1; i <= 7; i++)
    si5351.set_clock_pwr((si5351_clock)i, 0);

  cw_message = config.cwMessage.c_str();
  payload = config.payload.c_str();
  cw_freq = config.cwFreq.toInt();
  cw_speed = config.cwSpeed.toInt();

  Serial.println("SmartFOX2M booted !");
  digitalWrite(11, LOW);
}

void loop() { processPayload(payload); }
