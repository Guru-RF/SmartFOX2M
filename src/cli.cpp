#include "cli.h"
#include "band.h"
#include "cw.h"
#include "payload.h"
#include "pots.h"
#include "pwm.h"
#include "scheduler.h"
#include "sensors.h"
#include "si5351.h"
#include <FatFS.h>

extern Si5351 si5351;
extern struct Config {
  String fmFreq;
  String fmFreqList;
  String fmOffset;
  int cwWPM;
  String cwFreq;
  int cwVolume;
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

static String buf;

static long long ymdhmsToEpochLocal(int Y, int M, int D, int h, int m, int s) {
  int y = Y;
  if (M <= 2)
    y -= 1;
  long long era = (y >= 0 ? y : y - 399) / 400;
  unsigned yoe = (unsigned)(y - era * 400);
  unsigned doy = (153u * (unsigned)(M + (M > 2 ? -3 : 9)) + 2) / 5 + (unsigned)D - 1;
  unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long long days = era * 146097LL + (long long)doe - 719468LL;
  return days * 86400LL + (long long)h * 3600LL + (long long)m * 60LL + s;
}

// Parse EPOCH / ISO8601 / RFC 822/1036/1123/2822/3339 / "YYYY-MM-DD HH:MM:SS"
static long long parseAnyTimestampToEpoch(const String &raw) {
  String s = raw;
  s.trim();
  if (s.length() == 0)
    return -1LL;

  bool allDigits = s.length() >= 9;
  for (int i = 0; i < (int)s.length() && allDigits; i++) {
    if (!isDigit(s[i]))
      allDigits = false;
  }
  if (allDigits) {
    long long v = 0;
    for (int i = 0; i < (int)s.length(); i++)
      v = v * 10 + (s[i] - '0');
    return v;
  }

  int hmsPos = -1;
  int H = 0, M = 0, S = 0;
  for (int i = 0; i + 8 <= (int)s.length(); i++) {
    if (isDigit(s[i]) && isDigit(s[i + 1]) && s[i + 2] == ':' &&
        isDigit(s[i + 3]) && isDigit(s[i + 4]) && s[i + 5] == ':' &&
        isDigit(s[i + 6]) && isDigit(s[i + 7])) {
      if (i == 0 || !isDigit(s[i - 1])) {
        H = (s[i] - '0') * 10 + (s[i + 1] - '0');
        M = (s[i + 3] - '0') * 10 + (s[i + 4] - '0');
        S = (s[i + 6] - '0') * 10 + (s[i + 7] - '0');
        hmsPos = i;
        break;
      }
    }
  }
  if (hmsPos < 0 || H > 23 || M > 59 || S > 59)
    return -1LL;

  int Y = -1, Mo = -1, D = -1;
  String before = s.substring(0, hmsPos);

  for (int i = 0; i + 10 <= (int)before.length(); i++) {
    if (isDigit(before[i]) && isDigit(before[i + 1]) && isDigit(before[i + 2]) &&
        isDigit(before[i + 3]) && before[i + 4] == '-' &&
        isDigit(before[i + 5]) && isDigit(before[i + 6]) && before[i + 7] == '-' &&
        isDigit(before[i + 8]) && isDigit(before[i + 9])) {
      Y = (before[i] - '0') * 1000 + (before[i + 1] - '0') * 100 +
          (before[i + 2] - '0') * 10 + (before[i + 3] - '0');
      Mo = (before[i + 5] - '0') * 10 + (before[i + 6] - '0');
      D = (before[i + 8] - '0') * 10 + (before[i + 9] - '0');
      break;
    }
  }

  if (Y < 0) {
    static const char *const months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    String beforeL = before;
    beforeL.toLowerCase();
    int mIdx = -1;
    int monStart = -1;
    for (int mi = 0; mi < 12; mi++) {
      String low = String(months[mi]);
      low.toLowerCase();
      int pos = beforeL.indexOf(low);
      if (pos >= 0) {
        mIdx = mi;
        monStart = pos;
        break;
      }
    }
    if (mIdx >= 0) {
      Mo = mIdx + 1;
      String preM = before.substring(0, monStart);
      int end = preM.length() - 1;
      while (end >= 0 && !isDigit(preM[end]))
        end--;
      int start = end;
      while (start > 0 && isDigit(preM[start - 1]))
        start--;
      if (end >= start && start >= 0)
        D = preM.substring(start, end + 1).toInt();
      String postM = before.substring(monStart + 3);
      int ys = 0;
      while (ys < (int)postM.length() && !isDigit(postM[ys]))
        ys++;
      int ye = ys;
      while (ye < (int)postM.length() && isDigit(postM[ye]))
        ye++;
      if (ye > ys) {
        String yearStr = postM.substring(ys, ye);
        Y = yearStr.toInt();
        if (yearStr.length() == 2)
          Y += (Y >= 70) ? 1900 : 2000;
      }
    }
  }

  if (Y < 1970 || Mo < 1 || Mo > 12 || D < 1 || D > 31)
    return -1LL;

  long long epoch = ymdhmsToEpochLocal(Y, Mo, D, H, M, S);

  String tail = s.substring(hmsPos + 8);
  tail.trim();
  if (tail.length() > 0 && tail[0] != 'Z') {
    String zone = tail;
    int gmt = zone.indexOf("GMT");
    int utc = zone.indexOf("UTC");
    int start = gmt >= 0 ? gmt + 3 : (utc >= 0 ? utc + 3 : 0);
    zone = zone.substring(start);
    zone.trim();
    if (zone.length() > 0 && (zone[0] == '+' || zone[0] == '-')) {
      String off = zone.substring(1);
      int oh = 0, om = 0;
      if (off.length() >= 5 && off[2] == ':') {
        oh = off.substring(0, 2).toInt();
        om = off.substring(3, 5).toInt();
      } else if (off.length() >= 4) {
        oh = off.substring(0, 2).toInt();
        om = off.substring(2, 4).toInt();
      } else if (off.length() >= 2) {
        oh = off.substring(0, 2).toInt();
      }
      long long offSec = (long long)oh * 3600LL + (long long)om * 60LL;
      if (zone[0] == '+')
        epoch -= offSec;
      else
        epoch += offSec;
    }
  }

  return epoch;
}

static String formatEpoch(long long epoch) {
  if (epoch < 0)
    return String("----/--/-- --:--:-- UTC");
  long long secOfDay = epoch % 86400LL;
  long long days = epoch / 86400LL;
  if (secOfDay < 0) {
    secOfDay += 86400LL;
    days -= 1;
  }
  int h = (int)(secOfDay / 3600);
  int m = (int)((secOfDay / 60) % 60);
  int s = (int)(secOfDay % 60);

  long long z = days + 719468LL;
  long long era = (z >= 0 ? z : z - 146096) / 146097;
  unsigned doe = (unsigned)(z - era * 146097LL);
  unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  long long y = (long long)yoe + era * 400LL;
  unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  unsigned mp = (5 * doy + 2) / 153;
  unsigned d = doy - (153 * mp + 2) / 5 + 1;
  unsigned mo = mp + (mp < 10 ? 3 : -9);
  if (mo <= 2)
    y += 1;

  char buf[32];
  snprintf(buf, sizeof(buf), "%04ld-%02u-%02u %02d:%02d:%02d UTC",
           (long)y, mo, d, h, m, s);
  return String(buf);
}

static void help() {
  bool is80 = (bandDetected() == BAND_80M);
  Serial.print(F("CLI commands ("));
  Serial.print(is80 ? F("80M") : F("2M"));
  Serial.println(F(" mode):"));
  Serial.println(F("  help                 - this list"));
  Serial.println(F("  ls                   - list files on the drive"));
  Serial.println(F("  bat                  - print battery voltage"));
  Serial.println(F("  temp                 - print RP2040 temperature"));
  Serial.println(F("  freq [MHz]           - print or retune CLK0"));
  if (!is80) {
    Serial.println(F("  play <file>          - play .wav / .rtttl / .mod from drive"));
    Serial.println(F("  random <*.ext>       - play a random matching file"));
    Serial.println(F("  tone <hz> <ms>       - emit a tone"));
    Serial.println(F("  pots <ring|busy|congestion>  (runs ~5s)"));
  }
  Serial.println(F("  cw <text>            - send arbitrary CW text"));
  Serial.println(F("  cwmsg [1|2|3]        - send a CW message slot"));
  Serial.println(F("  msg <1|2|3> <text>   - set cwMessageN (RAM only, no echo)"));
  if (!is80)
    Serial.println(F("  signal | nosignal    - toggle CLK0 output"));
  Serial.println(F("  id                   - send callsign ID now"));
  Serial.println(F("  slot <0..5>          - change ARDF slot live (0=off)"));
  Serial.println(F("  time                 - print current UTC wall clock"));
  Serial.println(F("  settime <timestamp>  - set the wall clock"));
  Serial.println(F("                         accepts EPOCH | ISO8601 | RFC 822/1036/1123/2822/3339"));
  Serial.println(F("  payload              - run the configured payload once"));
  Serial.println(F("  status               - print state summary"));
  Serial.println(F("  formatdisk           - WIPE the USB drive and reboot"));
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
  Serial.print(F("wallUTC: "));
  Serial.println(formatEpoch(schedulerWallEpoch()));
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

  // On 80M (HF CW) hardware there is no PWM audio path, so these commands
  // have no effect. Refuse them loudly so users aren't surprised.
  if (bandDetected() == BAND_80M) {
    if (head == "play" || head == "random" || head == "tone" ||
        head == "pots" || head == "signal" || head == "nosignal") {
      Serial.print(F("not available on 80M: "));
      Serial.println(head);
      return;
    }
  }

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
    if (arg.length() == 0) {
      Serial.print(config.fmFreq);
      Serial.println(F(" MHz"));
    } else {
      double mhz = arg.toFloat();
      if (mhz > 0) {
        config.fmFreq = arg;
        uint64_t hz = (uint64_t)(mhz * 1e8);
        si5351.set_freq(hz, SI5351_CLK0);
        Serial.print(F("freq -> "));
        Serial.print(mhz, 4);
        Serial.println(F(" MHz"));
      } else {
        Serial.println(F("freq: bad value"));
      }
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
      audioIdle();
      radioNoSignal();
    } else
      Serial.println(F("pots: ring | busy | congestion"));
  } else if (head == "cw") {
    if (arg.length()) {
      cwSetVolume(config.cwVolume > 0 ? config.cwVolume : 100);
      radioSignal();
      int wpm = config.cwWPM > 0 ? config.cwWPM : 24;
      cw_string_proc(arg.c_str(), config.cwFreq.toInt(), 1200 / wpm);
      radioNoSignal();
      Serial.println();
    }
  } else if (head == "cwmsg") {
    int slot = arg.length() ? arg.toInt() : 1;
    radioCwMsg(slot);
    Serial.println();
  } else if (head == "msg") {
    // msg <slot>          → print current value of cwMessageN
    // msg <slot> <text>   → store new value (RAM only), no echo
    int sp2 = arg.indexOf(' ');
    int slot = (sp2 < 0 ? arg : arg.substring(0, sp2)).toInt();
    if (slot < 1 || slot > 3) {
      Serial.println(F("usage: msg <1|2|3> [text]"));
    } else if (sp2 < 0) {
      String *cur =
          slot == 1 ? &config.cwMessage1
                    : (slot == 2 ? &config.cwMessage2 : &config.cwMessage3);
      Serial.print(F("cwMessage"));
      Serial.print(slot);
      Serial.print(F(": "));
      Serial.println(*cur);
    } else {
      String text = arg.substring(sp2 + 1);
      if (slot == 1) config.cwMessage1 = text;
      else if (slot == 2) config.cwMessage2 = text;
      else config.cwMessage3 = text;
      Serial.print(F("cwMessage"));
      Serial.print(slot);
      Serial.print(F(" updated ("));
      Serial.print(text.length());
      Serial.println(F(" chars, RAM only)"));
    }
  } else if (head == "signal") {
    radioSignal();
  } else if (head == "nosignal") {
    radioNoSignal();
  } else if (head == "id") {
    runPayload("id");
    Serial.println();
  } else if (head == "time") {
    Serial.println(formatEpoch(schedulerWallEpoch()));
  } else if (head == "settime") {
    if (arg.length() == 0) {
      Serial.println(F("settime: need a timestamp (EPOCH / ISO8601 / RFC 822/1036/1123/2822/3339)"));
    } else {
      long long epoch = parseAnyTimestampToEpoch(arg);
      if (epoch < 0) {
        Serial.println(F("settime: could not parse timestamp"));
      } else {
        schedulerSetWallEpoch(epoch);
        Serial.print(F("wall clock set: "));
        Serial.println(formatEpoch(epoch));
      }
    }
  } else if (head == "payload") {
    Serial.println(F("running payload..."));
    runPayload(config.payload.c_str());
    radioNoSignal();
    Serial.println(F("payload done"));
  } else if (head == "slot") {
    config.ardfSlot = arg.toInt();
    Serial.print(F("ardfSlot -> "));
    Serial.println(config.ardfSlot);
    Serial.println(F("(restart for full re-arm)"));
  } else if (head == "status") {
    status();
  } else if (head == "formatdisk") {
    Serial.println(F("formatdisk: wiping filesystem in 3s..."));
    Serial.flush();
    delay(3000);
    FatFS.end();
    if (FatFS.format())
      Serial.println(F("format ok, rebooting..."));
    else
      Serial.println(F("format FAILED, rebooting anyway..."));
    Serial.flush();
    delay(200);
    rp2040.reboot();
  } else if (head == "reboot") {
    Serial.println(F("rebooting..."));
    delay(100);
    rp2040.reboot();
  } else if (head == "force80") {
    // Hidden: simulate 80M hardware by creating /force80.flag, wiping
    // config.txt and rebooting so the 80M default config is written.
    Serial.println(F("force80: writing /force80.flag, clearing config, rebooting..."));
    bandWriteForceFlag();
    FatFS.remove("/config.txt");
    delay(200);
    rp2040.reboot();
  } else if (head == "unforce80" || head == "force2m") {
    // Hidden: remove force flag, wipe config, reboot → defaults back to GP17.
    Serial.println(F("unforce80: removing /force80.flag, clearing config, rebooting..."));
    bandRemoveForceFlag();
    FatFS.remove("/config.txt");
    delay(200);
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
