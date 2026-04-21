#include "scheduler.h"

static ScheduleConfig sc;
static void (*setFreqFn)(uint64_t) = nullptr;

static unsigned long bootMs = 0;
static long wallSecAtBoot = -1;
static int winStart = -1, winEnd = -1;

static unsigned long lastIDms = 0;
static unsigned long lastFreqSwitchMs = 0;
static int freqIdx = 0;
static String freqs[8];
static int freqCount = 0;

static long parseHHMM(const String &s) {
  int colon = s.indexOf(':');
  if (colon < 0)
    return -1;
  int h = s.substring(0, colon).toInt();
  int m = s.substring(colon + 1).toInt();
  if (h < 0 || h > 23 || m < 0 || m > 59)
    return -1;
  return h * 3600 + m * 60;
}

static long parseDateTimeToSec(const String &s) {
  int sp = s.indexOf(' ');
  if (sp < 0)
    return -1;
  String time = s.substring(sp + 1);
  int c1 = time.indexOf(':');
  int c2 = time.indexOf(':', c1 + 1);
  if (c1 < 0 || c2 < 0)
    return -1;
  int h = time.substring(0, c1).toInt();
  int m = time.substring(c1 + 1, c2).toInt();
  int sec = time.substring(c2 + 1).toInt();
  if (h < 0 || h > 23 || m < 0 || m > 59 || sec < 0 || sec > 59)
    return -1;
  return h * 3600L + m * 60L + sec;
}

static void splitFreqs(const String &csv) {
  freqCount = 0;
  int start = 0;
  while (start < (int)csv.length() && freqCount < 8) {
    int comma = csv.indexOf(',', start);
    if (comma < 0)
      comma = csv.length();
    String piece = csv.substring(start, comma);
    piece.trim();
    if (piece.length() > 0)
      freqs[freqCount++] = piece;
    start = comma + 1;
  }
}

void schedulerBegin(const ScheduleConfig &cfg, void (*fn)(uint64_t)) {
  sc = cfg;
  setFreqFn = fn;
  bootMs = millis();
  lastIDms = bootMs;
  lastFreqSwitchMs = bootMs;

  if (sc.currentTime.length() > 0)
    wallSecAtBoot = parseDateTimeToSec(sc.currentTime);

  if (sc.runWindow.length() > 0) {
    int dash = sc.runWindow.indexOf('-');
    if (dash > 0) {
      winStart = parseHHMM(sc.runWindow.substring(0, dash));
      winEnd = parseHHMM(sc.runWindow.substring(dash + 1));
    }
  }

  splitFreqs(sc.freqList);
}

long schedulerWallSecOfDay() {
  if (wallSecAtBoot < 0)
    return -1;
  unsigned long elapsed = (millis() - bootMs) / 1000;
  long t = (wallSecAtBoot + (long)elapsed) % 86400L;
  return t;
}

static bool inRunWindow() {
  if (winStart < 0 || winEnd < 0)
    return true;
  long t = schedulerWallSecOfDay();
  if (t < 0)
    return true;
  if (winStart <= winEnd)
    return t >= winStart && t < winEnd;
  return t >= winStart || t < winEnd;
}

static bool inArdfSlot() {
  if (sc.ardfSlot <= 0)
    return true;
  long cycleSec = (long)sc.ardfCycleMin * 60L;
  if (cycleSec <= 0)
    return true;

  long ref;
  long w = schedulerWallSecOfDay();
  if (w >= 0)
    ref = w;
  else
    ref = (long)((millis() - bootMs) / 1000);

  long inCycle = ref % cycleSec;
  long slotIdx = inCycle / 60L;
  return slotIdx == (long)(sc.ardfSlot - 1);
}

unsigned long schedulerSlotEndsInMs() {
  if (sc.ardfSlot <= 0)
    return 0xFFFFFFFFUL;
  long ref;
  long w = schedulerWallSecOfDay();
  if (w >= 0)
    ref = w;
  else
    ref = (long)((millis() - bootMs) / 1000);
  long inCycle = ref % ((long)sc.ardfCycleMin * 60L);
  long slotStart = ((long)(sc.ardfSlot - 1)) * 60L;
  long secLeft = (slotStart + 60L) - inCycle;
  if (secLeft < 0)
    return 0;
  return (unsigned long)secLeft * 1000UL;
}

bool schedulerShouldTx() { return inRunWindow() && inArdfSlot(); }

bool schedulerIDDue() {
  if (sc.idIntervalMin <= 0 || sc.idCallsign.length() == 0)
    return false;
  return (millis() - lastIDms) >= (unsigned long)sc.idIntervalMin * 60UL * 1000UL;
}

void schedulerNoteIDSent() { lastIDms = millis(); }

void schedulerForceNextFreq() {
  if (freqCount < 2 || !setFreqFn)
    return;
  freqIdx = (freqIdx + 1) % freqCount;
  double mhz = freqs[freqIdx].toFloat();
  uint64_t hz = (uint64_t)(mhz * 1e8);
  setFreqFn(hz);
  lastFreqSwitchMs = millis();
  Serial.print("Freq -> ");
  Serial.print(freqs[freqIdx]);
  Serial.println(" MHz");
}

void schedulerTick() {
  if (sc.freqCycleSec > 0 && freqCount >= 2) {
    if (millis() - lastFreqSwitchMs >= (unsigned long)sc.freqCycleSec * 1000UL)
      schedulerForceNextFreq();
  }
}

String schedulerCurrentFreqMHz() {
  if (freqCount > 0)
    return freqs[freqIdx];
  return String();
}
