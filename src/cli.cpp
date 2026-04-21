#include "cli.h"
#include "cw.h"
#include "payload.h"
#include "pots.h"
#include "scheduler.h"
#include "sensors.h"
#include "si5351.h"
#include <FatFS.h>

extern Si5351 si5351;
extern struct Config {
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

static String buf;

static void help() {
  Serial.println(F("CLI commands:"));
  Serial.println(F("  help                 - this list"));
  Serial.println(F("  ls                   - list files on the drive"));
  Serial.println(F("  bat                  - print battery voltage"));
  Serial.println(F("  temp                 - print RP2040 temperature"));
  Serial.println(F("  freq <MHz>           - retune CLK0"));
  Serial.println(F("  play <file>          - play .wav or .rtttl from drive"));
  Serial.println(F("  random <*.ext>       - play a random matching file"));
  Serial.println(F("  tone <hz> <ms>       - emit a tone"));
  Serial.println(F("  pots <ring|busy|congestion>  (runs ~5s)"));
  Serial.println(F("  cw <text>            - send arbitrary CW text"));
  Serial.println(F("  cwmsg [1|2|3]        - send a CW message slot"));
  Serial.println(F("  msg <text>           - set cwMessage1 (RAM only)"));
  Serial.println(F("  signal | nosignal    - toggle CLK0 output"));
  Serial.println(F("  id                   - send callsign ID now"));
  Serial.println(F("  slot <0..5>          - change ARDF slot live (0=off)"));
  Serial.println(F("  status               - print state summary"));
  Serial.println(F("  reboot               - software reset"));
}

static void status() {
  Serial.print(F("freq:    "));
  Serial.print(config.fmFreq);
  Serial.println(F(" MHz"));
  Serial.print(F("ardf:    slot="));
  Serial.print(config.ardfSlot);
  Serial.print(F(" cycleMin="));
  Serial.println(config.ardfCycleMin);
  Serial.print(F("id:      "));
  Serial.print(config.idCallsign);
  Serial.print(F(" every "));
  Serial.print(config.idIntervalMin);
  Serial.println(F(" min"));
  Serial.print(F("window:  "));
  Serial.println(config.runWindow.length() ? config.runWindow : "always");
  Serial.print(F("wallSec: "));
  Serial.println(schedulerWallSecOfDay());
  Serial.print(F("shouldTx:"));
  Serial.println(schedulerShouldTx() ? F(" yes") : F(" no"));
  Serial.print(F("bat:     "));
  Serial.print(readBattery(), 2);
  Serial.println(F(" V"));
  Serial.print(F("temp:    "));
  Serial.print(readTemp(), 1);
  Serial.println(F(" C"));
}

static void exec(String line) {
  line.trim();
  if (line.length() == 0)
    return;

  int sp = line.indexOf(' ');
  String head = sp < 0 ? line : line.substring(0, sp);
  String arg = sp < 0 ? String() : line.substring(sp + 1);
  arg.trim();

  if (head == "help" || head == "?") {
    help();
  } else if (head == "ls") {
    Dir d = FatFS.openDir("/");
    uint32_t count = 0, total = 0;
    while (d.next()) {
      String name = d.fileName();
      uint32_t sz = d.fileSize();
      total += sz;
      count++;
      Serial.print("  ");
      if (sz < 1024)
        Serial.printf("%6u B  ", (unsigned)sz);
      else if (sz < 1024 * 1024)
        Serial.printf("%6.1f KB ", sz / 1024.0f);
      else
        Serial.printf("%6.2f MB ", sz / 1048576.0f);
      Serial.println(name);
    }
    Serial.printf("%lu file%s, %.1f KB total\n", (unsigned long)count,
                  count == 1 ? "" : "s", total / 1024.0f);
  } else if (head == "bat") {
    Serial.print(readBattery(), 3);
    Serial.println(F(" V"));
  } else if (head == "temp") {
    Serial.print(readTemp(), 2);
    Serial.println(F(" C"));
  } else if (head == "freq") {
    double mhz = arg.toFloat();
    if (mhz > 0) {
      config.fmFreq = arg;
      uint64_t hz = (uint64_t)(mhz * 1e8);
      si5351.set_freq(hz, SI5351_CLK0);
      Serial.print(F("freq -> "));
      Serial.print(mhz, 4);
      Serial.println(F(" MHz"));
    }
  } else if (head == "play") {
    radioSignal();
    String mini = "play " + arg;
    runPayload(mini.c_str());
    radioNoSignal();
  } else if (head == "random") {
    radioSignal();
    String mini = "random " + arg;
    runPayload(mini.c_str());
    radioNoSignal();
  } else if (head == "tone") {
    radioSignal();
    String mini = "tone " + arg;
    runPayload(mini.c_str());
    radioNoSignal();
  } else if (head == "pots") {
    String sub = arg;
    sub.trim();
    if (sub == "ring" || sub == "busy" || sub == "congestion") {
      radioSignal();
      if (sub == "ring")
        ring(3);
      else if (sub == "busy")
        busy(5);
      else
        congestion(15);
      radioNoSignal();
    } else
      Serial.println(F("pots: ring | busy | congestion"));
  } else if (head == "cw") {
    if (arg.length()) {
      radioSignal();
      cw_string_proc(arg.c_str(), config.cwFreq.toInt(),
                     config.cwSpeed.toInt());
      radioNoSignal();
    }
  } else if (head == "cwmsg") {
    int slot = arg.length() ? arg.toInt() : 1;
    radioCwMsg(slot);
  } else if (head == "msg") {
    config.cwMessage1 = arg;
    Serial.print(F("cwMessage1 -> "));
    Serial.println(arg);
  } else if (head == "signal") {
    radioSignal();
  } else if (head == "nosignal") {
    radioNoSignal();
  } else if (head == "id") {
    runPayload("id");
  } else if (head == "slot") {
    config.ardfSlot = arg.toInt();
    Serial.print(F("ardfSlot -> "));
    Serial.println(config.ardfSlot);
    Serial.println(F("(restart for full re-arm)"));
  } else if (head == "status") {
    status();
  } else if (head == "reboot") {
    Serial.println(F("rebooting..."));
    delay(100);
    rp2040.reboot();
  } else {
    Serial.print(F("? "));
    Serial.println(line);
  }
}

void cliBegin() {
  buf.reserve(128);
  Serial.println(F("CLI ready. Type 'help'."));
}

void cliTick() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (buf.length() > 0) {
        String line = buf;
        buf = "";
        Serial.println();
        exec(line);
      }
    } else if (c == 0x7f || c == 0x08) {
      if (buf.length() > 0) {
        buf.remove(buf.length() - 1);
        Serial.print("\b \b");
      }
    } else if (c >= 0x20 && c < 0x7f) {
      if (buf.length() < 200) {
        buf += c;
        Serial.write(c);
      }
    }
  }
}
