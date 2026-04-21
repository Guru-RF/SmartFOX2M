#include "FS.h"
#include "Wire.h"
#include "cli.h"
#include "cw.h"
#include "leds.h"
#include "payload.h"
#include "pots.h"
#include "pwm.h"
#include "rtttl.h"
#include "scheduler.h"
#include "sensors.h"
#include "si5351.h"
#include <Arduino.h>
#include <FatFS.h>
#include <FatFSUSB.h>

struct Config {
  String fmFreq;
  String fmFreqList;
  String fmOffset;
  String cwSpeed;
  String cwFreq;
  String cwMessage;
  String cwMessage1;
  String cwMessage2;
  String cwMessage3;
  String idCallsign;
  String payload;
  int ardfSlot;
  int ardfCycleMin;
  int idIntervalMin;
  int freqCycleSec;
  String runWindow;
  String currentTime;
  float batLowVolts;
  float batCutoffVolts;
  bool sleepBetweenSlots;
  int batAdcPin;
  float batDivider;
} config;

Si5351 si5351;
#define PLLB_FREQ 86865000000ULL
#define LED_PIN 11

volatile bool updated = false;
volatile bool driveConnected = false;
volatile bool inPrinting = false;
static bool batCutoff = false;

static void setDefaults(Config &c) {
  c.fmFreq = "145.450";
  c.fmFreqList = "";
  c.fmOffset = "30000";
  c.cwSpeed = "75";
  c.cwFreq = "750";
  c.cwMessage = "";
  c.cwMessage1 = "VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS A "
                 "NAGOYA MINI VERTICAL";
  c.cwMessage2 = "";
  c.cwMessage3 = "";
  c.idCallsign = "ON6URE";
  c.payload = "";
  c.ardfSlot = 0;
  c.ardfCycleMin = 5;
  c.idIntervalMin = 10;
  c.freqCycleSec = 0;
  c.runWindow = "";
  c.currentTime = "";
  c.batLowVolts = 6.5f;
  c.batCutoffVolts = 6.0f;
  c.sleepBetweenSlots = true;
  c.batAdcPin = 28;
  c.batDivider = 3.0f;
}

static bool yamlParse(const char *yaml, Config &cfg) {
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

      {
        bool inQuote = false;
        int cut = -1;
        for (int i = 0; i < (int)value.length(); i++) {
          char c = value.charAt(i);
          if (c == '"')
            inQuote = !inQuote;
          else if (c == '#' && !inQuote) {
            cut = i;
            break;
          }
        }
        if (cut >= 0)
          value = value.substring(0, cut);
        value.trim();
      }

      value.replace("\"", "");
      value.trim();

      if (key == "fmFreq") cfg.fmFreq = value;
      else if (key == "fmFreqList") cfg.fmFreqList = value;
      else if (key == "fmOffset") cfg.fmOffset = value;
      else if (key == "cwSpeed") cfg.cwSpeed = value;
      else if (key == "cwFreq") cfg.cwFreq = value;
      else if (key == "cwMessage") cfg.cwMessage = value;
      else if (key == "cwMessage1") cfg.cwMessage1 = value;
      else if (key == "cwMessage2") cfg.cwMessage2 = value;
      else if (key == "cwMessage3") cfg.cwMessage3 = value;
      else if (key == "idCallsign") cfg.idCallsign = value;
      else if (key == "ardfSlot") cfg.ardfSlot = value.toInt();
      else if (key == "ardfCycleMin") cfg.ardfCycleMin = value.toInt();
      else if (key == "idIntervalMin") cfg.idIntervalMin = value.toInt();
      else if (key == "freqCycleSec") cfg.freqCycleSec = value.toInt();
      else if (key == "runWindow") cfg.runWindow = value;
      else if (key == "currentTime") cfg.currentTime = value;
      else if (key == "batLowVolts") cfg.batLowVolts = value.toFloat();
      else if (key == "batCutoffVolts") cfg.batCutoffVolts = value.toFloat();
      else if (key == "sleepBetweenSlots")
        cfg.sleepBetweenSlots = (value == "true" || value == "1");
      else if (key == "batAdcPin") cfg.batAdcPin = value.toInt();
      else if (key == "batDivider") cfg.batDivider = value.toFloat();
      else if (key == "payload") cfg.payload = value;
    }
  }

  if (payloadBlock.length() > 0)
    cfg.payload = payloadBlock;

  if (cfg.cwMessage1.length() == 0 && cfg.cwMessage.length() > 0)
    cfg.cwMessage1 = cfg.cwMessage;

  return true;
}

static void unplug(uint32_t i) {
  (void)i;
  driveConnected = false;
  updated = true;
  rp2040.reboot();
}

static void plug(uint32_t i) {
  (void)i;
  driveConnected = true;
  FatFS.end();
}

static bool mountable(uint32_t i) {
  (void)i;
  driveConnected = true;
  return !inPrinting;
}

static void setRadioFreq(uint64_t hz) {
  si5351.set_freq(hz, SI5351_CLK0);
}

static bool rmRecursive(const String &path) {
  Dir d = FatFS.openDir(path.c_str());
  while (d.next()) {
    String name = d.fileName();
    String full = path;
    if (!full.endsWith("/"))
      full += "/";
    full += name;
    if (d.isDirectory())
      rmRecursive(full);
    else
      FatFS.remove(full.c_str());
  }
  if (path != "/")
    return FatFS.rmdir(path.c_str());
  return true;
}

static void cleanMacMetadata() {
  int removed = 0;
  Dir d = FatFS.openDir("/");
  while (d.next()) {
    String name = d.fileName();
    String full = "/" + name;
    bool isMeta = name.startsWith("._") || name == ".DS_Store" ||
                  name == ".Spotlight-V100" || name == ".Trashes" ||
                  name == ".fseventsd" || name == ".TemporaryItems" ||
                  name == ".apdisk";
    if (!isMeta)
      continue;
    if (d.isDirectory()) {
      if (rmRecursive(full))
        removed++;
    } else {
      if (FatFS.remove(full.c_str()))
        removed++;
    }
  }
  if (removed > 0) {
    Serial.print("Cleaned ");
    Serial.print(removed);
    Serial.println(" macOS metadata entries.");
  }
}

static void cwSpeakRaw(const String &s) {
  bool tx = true;
  si5351.output_enable(SI5351_CLK0, 1);
  ledSetState(LED_TX);
  cw_string_proc(s.c_str(), config.cwFreq.toInt(), config.cwSpeed.toInt());
  (void)tx;
}

static void batteryWatchdog() {
  static unsigned long lastCheck = 0;
  static bool lobatAnnounced = false;
  if (batCutoff)
    return;
  unsigned long now = millis();
  if (now - lastCheck < 30000UL && lastCheck != 0)
    return;
  lastCheck = now;

  if (config.batLowVolts <= 0 && config.batCutoffVolts <= 0)
    return;
  float v = readBattery();
  if (config.batCutoffVolts > 0 && v < config.batCutoffVolts) {
    Serial.print("BAT cutoff: ");
    Serial.print(v, 2);
    Serial.println(" V — shutting down RF.");
    radioNoSignal();
    if (!driveConnected)
      cwSpeakRaw("LOBAT CUT");
    si5351.set_clock_pwr(SI5351_CLK0, 0);
    ledSetState(LED_ERROR);
    batCutoff = true;
    return;
  }
  if (config.batLowVolts > 0 && v < config.batLowVolts) {
    if (!lobatAnnounced) {
      Serial.print("BAT low: ");
      Serial.print(v, 2);
      Serial.println(" V");
      if (!driveConnected)
        cwSpeakRaw("LOBAT");
      lobatAnnounced = true;
    }
    ledSetState(LED_LOBAT);
  } else {
    lobatAnnounced = false;
  }
}

static const char *defaultYaml() {
  return
      "# RF\n"
      "fmFreq: \"145.450\"\n"
      "# comma list e.g. \"145.450,145.550\" enables rotation; empty = no rotation\n"
      "fmFreqList: \"\"\n"
      "fmOffset: \"30000\"\n"
      "# 0 = no rotation, else seconds per freq in fmFreqList\n"
      "freqCycleSec: 0\n"
      "\n"
      "# CW\n"
      "cwSpeed: \"75\"\n"
      "cwFreq: \"750\"\n"
      "cwMessage1: \"VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS A NAGOYA MINI VERTICAL\"\n"
      "cwMessage2: \"\"\n"
      "cwMessage3: \"\"\n"
      "\n"
      "# Legal callsign ID watchdog (0 = disabled)\n"
      "idCallsign: \"ON6URE\"\n"
      "idIntervalMin: 10\n"
      "\n"
      "# ARDF: 0 = continuous TX, 1..5 = which slot in the cycle\n"
      "ardfSlot: 0\n"
      "ardfCycleMin: 5\n"
      "\n"
      "# Schedule (RTC seeded once at boot from currentTime)\n"
      "# runWindow empty = always, or e.g. \"09:00-17:00\"\n"
      "runWindow: \"\"\n"
      "# currentTime \"YYYY-MM-DD HH:MM:SS\" seeds the wall clock\n"
      "currentTime: \"\"\n"
      "\n"
      "# Battery (VSYS via ADC) - defaults: GPIO28 = ADC2, 3:1 divider\n"
      "batLowVolts: 6.5\n"
      "batCutoffVolts: 6.0\n"
      "batAdcPin: 28\n"
      "batDivider: 3.0\n"
      "sleepBetweenSlots: true\n"
      "\n"
      "# Payload commands:\n"
      "#   signal | nosignal | wait <s>\n"
      "#   play <file.wav|file.rtttl> | random *.wav | random *.rtttl\n"
      "#   tone <hz> <ms> | homing <s> | freq <MHz>\n"
      "#   repeat <n> ... end\n"
      "#   cwmsg | cwmsg2 | cwmsg3 | cw0 | cw1 | id | temp | bat\n"
      "#   ring | busy | congestion | pwm\n"
      "payload: |\n"
      "  signal\n"
      "  cwmsg\n"
      "  wait 1\n"
      "  homing 8\n"
      "  wait 1\n"
      "  random *.rtttl\n"
      "  wait 1\n"
      "  random *.wav\n"
      "  wait 1\n"
      "  cw0\n"
      "  wait 2\n"
      "  cw1\n"
      "  wait 1\n"
      "  cwmsg\n"
      "  nosignal\n"
      "  wait 20\n";
}

void setup() {
  ledBegin(LED_PIN);
  setDefaults(config);

  delay(2000);
  Serial.println("[boot] starting");
  bool fsOk = FatFS.begin();
  if (!fsOk) {
    Serial.println("FatFS mount failed — formatting (5s to abort via reset)...");
    Serial.flush();
    delay(5000);
    if (FatFS.format() && FatFS.begin()) {
      Serial.println("FatFS formatted & mounted.");
      fsOk = true;
    } else {
      Serial.println("FatFS unrecoverable.");
      ledSetState(LED_ERROR);
    }
  } else {
    Serial.println("FatFS mounted.");
  }
  if (fsOk) {
    fatfs::f_setlabel("SMARTFOX");
    cleanMacMetadata();
  }

  const char *filePath = "/config.txt";

  bool loaded = false;
  bool exists = fsOk && FatFS.exists(filePath);
  Serial.print("config.txt exists: ");
  Serial.println(exists ? "yes" : "no");

  if (exists) {
    File file = FatFS.open(filePath, "r");
    if (file) {
      String yamlContent;
      while (file.available())
        yamlContent += (char)file.read();
      file.close();
      yamlContent.trim();
      Serial.print("config bytes: ");
      Serial.println(yamlContent.length());

      if (yamlContent == "firmwareupdate") {
        Serial.println("Firmware update requested.");
        if (FatFS.remove(filePath))
          Serial.println("Config file removed.");
        delay(500);
        Serial.println("Rebooting into UF2 bootloader...");
        delay(500);
        reset_usb_boot(0, 0);
        while (true)
          ;
      }

      if (yamlContent.length() > 0)
        yamlParse(yamlContent.c_str(), config);
    } else {
      Serial.println("Could not open existing config.txt for read.");
    }
  }

  if (fsOk && config.payload.length() == 0) {
    Serial.println("Config missing/empty — writing default.");
    Serial.flush();
    if (exists) {
      Serial.println(" removing stale file...");
      Serial.flush();
      FatFS.remove(filePath);
    }
    Serial.println(" opening for write...");
    Serial.flush();
    File file = FatFS.open(filePath, "w");
    if (!file) {
      Serial.println("ERR: cannot open config.txt for write");
      Serial.flush();
    } else {
      Serial.println(" open ok, writing in chunks...");
      Serial.flush();
      const char *def = defaultYaml();
      size_t defLen = strlen(def);
      Serial.print(" default size: ");
      Serial.println(defLen);
      Serial.flush();

      size_t total = 0;
      const size_t CHUNK = 128;
      bool writeOk = true;
      while (total < defLen) {
        size_t want = defLen - total;
        if (want > CHUNK)
          want = CHUNK;
        size_t w = file.write((const uint8_t *)def + total, want);
        if (w == 0 || w == (size_t)-1) {
          Serial.print(" chunk write failed at offset ");
          Serial.println(total);
          Serial.flush();
          writeOk = false;
          break;
        }
        total += w;
        file.flush();
      }
      Serial.print(" total written: ");
      Serial.println(total);
      Serial.flush();
      file.close();
      Serial.println(" file.close done");
      Serial.flush();
      if (writeOk) {
        yamlParse(def, config);
        Serial.println(" yamlParse done");
        Serial.flush();
        loaded = true;
      }
    }
  } else {
    loaded = true;
  }

  if (!loaded) {
    Serial.println("FATAL: no config and could not write default.");
    ledSetState(LED_ERROR);
  }

  Serial.println("[boot] starting FatFSUSB");
  Serial.flush();
  delay(200);
  FatFSUSB.onUnplug(unplug);
  FatFSUSB.onPlug(plug);
  FatFSUSB.driveReady(mountable);
  FatFSUSB.begin();
  delay(500);
  Serial.println("[boot] FatFSUSB up");

  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Wire1.setSDA(26);
  Wire1.setSCL(27);
  delay(10);

  Serial.println("[boot] si5351 init");
  int32_t fmOffset = config.fmOffset.toInt();
  bool i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_0PF, 250000000, fmOffset);
  if (!i2c_found)
    Serial.println("Si5351 not found on I2C bus!");

  si5351.reset();
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLB);
  si5351.pll_reset(SI5351_PLLB);
  si5351.set_vcxo(PLLB_FREQ, 50);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);

  double freqMHz = config.fmFreq.toFloat();
  uint64_t freqHz = (uint64_t)(freqMHz * 1e8);
  si5351.set_freq(freqHz, SI5351_CLK0);
  si5351.output_enable(SI5351_CLK0, 1);
  for (int i = 1; i <= 7; i++)
    si5351.set_clock_pwr((si5351_clock)i, 0);

  Serial.println("[boot] sensors");
  sensorsBegin(config.batAdcPin, config.batDivider);

  Serial.println("[boot] scheduler");
  ScheduleConfig sc;
  sc.ardfSlot = config.ardfSlot;
  sc.ardfCycleMin = config.ardfCycleMin > 0 ? config.ardfCycleMin : 5;
  sc.idIntervalMin = config.idIntervalMin;
  sc.idCallsign = config.idCallsign;
  sc.runWindow = config.runWindow;
  sc.freqList = config.fmFreqList;
  sc.freqCycleSec = config.freqCycleSec;
  sc.currentTime = config.currentTime;
  schedulerBegin(sc, setRadioFreq);

  randomSeed(micros());

  Serial.println("[boot] cli");
  cliBegin();
  ledSetState(LED_IDLE);
  Serial.println("[boot] done — SmartFOX2M running");
}

void loop() {
  static bool wasDriveConnected = false;
  static bool wasOutOfSlot = false;

  ledTick();
  cliTick();
  schedulerTick();
  batteryWatchdog();

  if (batCutoff) {
    delay(200);
    return;
  }

  if (driveConnected) {
    if (!wasDriveConnected) {
      radioNoSignal();
      wasDriveConnected = true;
    }
    delay(50);
    return;
  }
  wasDriveConnected = false;

  if (!schedulerShouldTx()) {
    if (!wasOutOfSlot) {
      if (ledGetState() != LED_LOBAT && ledGetState() != LED_ERROR)
        ledSetState(LED_OFF_SCHED);
      radioNoSignal();
      wasOutOfSlot = true;
    }
    if (config.sleepBetweenSlots) {
      unsigned long s = schedulerSlotEndsInMs();
      unsigned long d = s > 5000 ? 1000 : 200;
      delay(d);
    } else {
      delay(200);
    }
    return;
  }
  wasOutOfSlot = false;

  if (ledGetState() == LED_OFF_SCHED)
    ledSetState(LED_IDLE);

  runPayload(config.payload.c_str());
}
