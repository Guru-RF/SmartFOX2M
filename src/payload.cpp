#include "payload.h"
#include "band.h"
#include "cw.h"
#include "leds.h"
#include "mod.h"
#include "pots.h"
#include "pwm.h"
#include "rtttl.h"
#include "scheduler.h"
#include "sensors.h"
#include "si5351.h"
#include <FatFS.h>
#include <vector>

extern Si5351 si5351;
extern struct Config {
  String fmFreq;
  String fmFreqList;
  String fmOffset;
  int cwWPM;
  String cwFreq;
  int cwVolume;
  int modVolume;
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
} config;

void radioSignal() {
  si5351.output_enable(SI5351_CLK0, 1);
  ledSetState(LED_TX);
}

void radioNoSignal() {
  si5351.output_enable(SI5351_CLK0, 0);
  ledSetState(LED_IDLE);
}

static int cwDitMs() {
  int wpm = config.cwWPM > 0 ? config.cwWPM : 24;
  return 1200 / wpm;
}

void radioCwMsg(int slot) {
  String msg;
  switch (slot) {
  case 1: msg = config.cwMessage1; break;
  case 2: msg = config.cwMessage2; break;
  case 3: msg = config.cwMessage3; break;
  default: msg = config.cwMessage1.length() ? config.cwMessage1 : config.cwMessage;
  }
  if (msg.length() == 0)
    return;
  cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
  radioSignal();
  cw_string_proc(msg.c_str(), config.cwFreq.toInt(), cwDitMs());
}

void radioCw0() {
  cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
  radioSignal();
  cw(false, config.cwFreq.toInt());
}

void radioCw1() {
  cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
  radioSignal();
  cw(true, config.cwFreq.toInt());
}

static void sendCwId() {
  if (config.idCallsign.length() == 0)
    return;
  Serial.print("CW-ID: ");
  Serial.println(config.idCallsign);
  bool wasTx = (ledGetState() == LED_TX);
  cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
  radioSignal();
  cw_string_proc(config.idCallsign.c_str(), config.cwFreq.toInt(), cwDitMs());
  if (!wasTx)
    radioNoSignal();
  schedulerNoteIDSent();
}

void payloadWaitMs(unsigned long ms) {
  unsigned long target = millis() + ms;
  while ((long)(target - millis()) > 0) {
    delay(20);
    schedulerTick();
    ledTick();
    if (schedulerIDDue())
      sendCwId();
    if (!schedulerShouldTx())
      break;
  }
}

static void doTone(int hz, int ms) {
  audioTone(hz);
  payloadWaitMs(ms);
  audioIdle();
}

static void doHoming(int sec) {
  unsigned long end = millis() + (unsigned long)sec * 1000UL;
  bool hi = false;
  while ((long)(end - millis()) > 0 && schedulerShouldTx()) {
    audioTone(hi ? 1100 : 700);
    delay(180);
    hi = !hi;
    schedulerTick();
    ledTick();
  }
  audioIdle();
}

static void cwSpeak(const String &s) {
  bool wasTx = (ledGetState() == LED_TX);
  cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
  radioSignal();
  cw_string_proc(s.c_str(), config.cwFreq.toInt(), cwDitMs());
  if (!wasTx)
    radioNoSignal();
}

static void doTemp() {
  float t = readTemp();
  char buf[24];
  snprintf(buf, sizeof(buf), "T %d C", (int)(t + 0.5f));
  Serial.print("Temp: ");
  Serial.println(buf);
  cwSpeak(String(buf));
}

static void doBat() {
  float v = readBattery();
  char buf[24];
  int whole = (int)v;
  int dec = (int)((v - whole) * 10.0f + 0.5f);
  snprintf(buf, sizeof(buf), "B %d.%d V", whole, dec);
  Serial.print("Bat: ");
  Serial.println(buf);
  cwSpeak(String(buf));
}

static String pickRandomMatching(const String &suffixLower) {
  int count = 0;
  Dir d = FatFS.openDir("/");
  while (d.next()) {
    String n = d.fileName();
    String low = n;
    low.toLowerCase();
    if (low.endsWith(suffixLower))
      count++;
  }
  if (count == 0)
    return String();
  long pick = random(count);
  int i = 0;
  Dir d2 = FatFS.openDir("/");
  while (d2.next()) {
    String n = d2.fileName();
    String low = n;
    low.toLowerCase();
    if (low.endsWith(suffixLower)) {
      if (i == pick)
        return n;
      i++;
    }
  }
  return String();
}

static void doPlay(const String &nameRaw) {
  String name = nameRaw;
  name.trim();
  String path = name.startsWith("/") ? name : "/" + name;
  String low = path;
  low.toLowerCase();
  Serial.print("Play: ");
  Serial.println(path);
  if (low.endsWith(".wav"))
    playWAV(path.c_str());
  else if (low.endsWith(".rtttl") || low.endsWith(".rtx"))
    playRTTTL(path.c_str());
  else if (low.endsWith(".mod"))
    playMOD(path.c_str());
  else
    Serial.println("Unknown file extension (use .wav, .rtttl, or .mod)");
}

static void doRandom(const String &patternRaw) {
  String pat = patternRaw;
  pat.trim();
  pat.toLowerCase();
  String suffix;
  int dot = pat.lastIndexOf('.');
  if (dot < 0) {
    Serial.println("random: need pattern like *.wav or *.rtttl");
    return;
  }
  suffix = pat.substring(dot);
  String picked = pickRandomMatching(suffix);
  if (picked.length() == 0) {
    Serial.print("random: no matches for ");
    Serial.println(suffix);
    return;
  }
  Serial.print("random -> ");
  Serial.println(picked);
  doPlay(picked);
}

static void doFreq(const String &mhzStr) {
  double mhz = mhzStr.toFloat();
  if (mhz <= 0)
    return;
  uint64_t hz = (uint64_t)(mhz * 1e8);
  si5351.set_freq(hz, SI5351_CLK0);
  Serial.print("Freq -> ");
  Serial.print(mhz, 4);
  Serial.println(" MHz");
}

static std::vector<String> tokenize(const char *payload) {
  std::vector<String> out;
  String cur;
  while (*payload) {
    if (*payload == '\n') {
      out.push_back(cur);
      cur = "";
    } else {
      cur += *payload;
    }
    payload++;
  }
  if (cur.length() > 0)
    out.push_back(cur);

  std::vector<String> clean;
  for (auto &line : out) {
    int hash = line.indexOf('#');
    if (hash >= 0)
      line = line.substring(0, hash);
    line.trim();
    if (line.length() > 0)
      clean.push_back(line);
  }
  return clean;
}

void runPayload(const char *payload) {
  if (!payload || !*payload)
    return;
  std::vector<String> lines = tokenize(payload);
  if (lines.empty())
    return;

  struct Frame {
    int returnPc;
    int remaining;
  };
  std::vector<Frame> stack;

  int pc = 0;
  while (pc < (int)lines.size()) {
    schedulerTick();
    ledTick();
    if (!schedulerShouldTx()) {
      radioNoSignal();
      return;
    }
    if (schedulerIDDue())
      sendCwId();

    String &cmd = lines[pc];

    // Skip audio-playback and signal-gating commands on 80M (HF CW): no PWM
    // audio chain, and cw() already keys the carrier directly.
    bool is80 = (bandDetected() == BAND_80M);
    bool audioCmd = cmd.startsWith("play ") || cmd.startsWith("random ") ||
                    cmd.startsWith("tone ") || cmd.startsWith("homing ") ||
                    cmd == "pwm" || cmd == "ring" || cmd == "busy" ||
                    cmd == "congestion" || cmd == "signal" || cmd == "nosignal";
    if (is80 && audioCmd) {
      Serial.print("[80M] skipping audio/signal cmd: ");
      Serial.println(cmd);
      pc++;
      continue;
    }

    if (cmd.startsWith("wait ")) {
      payloadWaitMs((unsigned long)cmd.substring(5).toInt() * 1000UL);
    } else if (cmd.startsWith("play ")) {
      doPlay(cmd.substring(5));
    } else if (cmd.startsWith("random ")) {
      doRandom(cmd.substring(7));
    } else if (cmd.startsWith("tone ")) {
      String rest = cmd.substring(5);
      int sp = rest.indexOf(' ');
      int hz = rest.substring(0, sp < 0 ? rest.length() : sp).toInt();
      int ms = sp < 0 ? 200 : rest.substring(sp + 1).toInt();
      doTone(hz, ms);
    } else if (cmd.startsWith("homing ")) {
      doHoming(cmd.substring(7).toInt());
    } else if (cmd.startsWith("freq ")) {
      doFreq(cmd.substring(5));
    } else if (cmd.startsWith("repeat ")) {
      int n = cmd.substring(7).toInt();
      if (n > 1)
        stack.push_back({pc, n - 1});
    } else if (cmd == "end") {
      if (!stack.empty()) {
        Frame &top = stack.back();
        if (top.remaining > 0) {
          top.remaining--;
          pc = top.returnPc;
        } else {
          stack.pop_back();
        }
      }
    } else if (cmd == "temp") {
      doTemp();
    } else if (cmd == "bat") {
      doBat();
    } else if (cmd == "pwm") {
      playPWM();
    } else if (cmd == "ring") {
      ring(5);
      audioIdle();
    } else if (cmd == "busy") {
      busy(5);
      audioIdle();
    } else if (cmd == "congestion") {
      congestion(5);
      audioIdle();
    } else if (cmd == "cw0") {
      radioCw0();
    } else if (cmd == "cw1") {
      radioCw1();
    } else if (cmd == "cwmsg" || cmd == "cwmsg1") {
      radioCwMsg(1);
    } else if (cmd == "cwmsg2") {
      radioCwMsg(2);
    } else if (cmd == "cwmsg3") {
      radioCwMsg(3);
    } else if (cmd == "signal") {
      radioSignal();
    } else if (cmd == "nosignal") {
      radioNoSignal();
    } else if (cmd == "id") {
      sendCwId();
    } else {
      Serial.print("Unknown command: ");
      Serial.println(cmd);
    }

    pc++;
  }
}
