#include "scheduler.h"

static ScheduleConfig sc;
static void (*setFreqFn)(uint64_t) = nullptr;

static unsigned long bootMs = 0;
static long long epochAtBoot = -1;
static int winStart = -1, winEnd = -1;

static long long ymdhmsToEpoch(int Y, int M, int D, int h, int m, int s) {
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

static long long parseConfigTimeToEpoch(const String &s) {
  if (s.length() < 19)
    return -1LL;
  int Y = s.substring(0, 4).toInt();
  int Mo = s.substring(5, 7).toInt();
  int D = s.substring(8, 10).toInt();
  int h = s.substring(11, 13).toInt();
  int m = s.substring(14, 16).toInt();
  int sec = s.substring(17, 19).toInt();
  if (Y < 1970 || Mo < 1 || Mo > 12 || D < 1 || D > 31)
    return -1LL;
  if (h > 23 || m > 59 || sec > 59)
    return -1LL;
  return ymdhmsToEpoch(Y, Mo, D, h, m, sec);
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
    epochAtBoot = parseConfigTimeToEpoch(sc.currentTime);

  if (sc.runWindow.length() > 0) {
    int dash = sc.runWindow.indexOf('-');
    if (dash > 0) {
      winStart = parseHHMM(sc.runWindow.substring(0, dash));
      winEnd = parseHHMM(sc.runWindow.substring(dash + 1));
    }
  }

  splitFreqs(sc.freqList);
}

long long schedulerWallEpoch() {
  if (epochAtBoot < 0)
    return -1LL;
  unsigned long elapsed = (millis() - bootMs) / 1000;
  return epochAtBoot + (long long)elapsed;
}

long schedulerWallSecOfDay() {
  long long ep = schedulerWallEpoch();
  if (ep < 0)
    return -1;
  long t = (long)(ep % 86400LL);
  if (t < 0)
    t += 86400;
  return t;
}

void schedulerSetWallEpoch(long long ep) {
  epochAtBoot = ep;
  bootMs = millis();
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

  long long ref;
  long long ep = schedulerWallEpoch();
  if (ep >= 0)
    ref = ep;
  else
    ref = (long long)((millis() - bootMs) / 1000);

  long long inCycle = ref % cycleSec;
  long long slotIdx = inCycle / 60LL;
  return slotIdx == (long long)(sc.ardfSlot - 1);
}

unsigned long schedulerSlotEndsInMs() {
  if (sc.ardfSlot <= 0)
    return 0xFFFFFFFFUL;
  long long ref;
  long long ep = schedulerWallEpoch();
  if (ep >= 0)
    ref = ep;
  else
    ref = (long long)((millis() - bootMs) / 1000);
  long long inCycle = ref % ((long long)sc.ardfCycleMin * 60LL);
  long long slotStart = ((long long)(sc.ardfSlot - 1)) * 60LL;
  long long secLeft = (slotStart + 60LL) - inCycle;
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
