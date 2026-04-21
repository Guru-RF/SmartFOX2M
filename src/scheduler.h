#include <Arduino.h>

struct ScheduleConfig {
  int ardfSlot;
  int ardfCycleMin;
  int idIntervalMin;
  String idCallsign;
  String runWindow;
  String freqList;
  int freqCycleSec;
  String currentTime;
};

void schedulerBegin(const ScheduleConfig &cfg, void (*setFreqHzFn)(uint64_t hz));
void schedulerTick();
bool schedulerShouldTx();
bool schedulerIDDue();
void schedulerNoteIDSent();
unsigned long schedulerSlotEndsInMs();
long schedulerWallSecOfDay();
long long schedulerWallEpoch();
void schedulerSetWallEpoch(long long epoch);
String schedulerCurrentFreqMHz();
void schedulerForceNextFreq();
