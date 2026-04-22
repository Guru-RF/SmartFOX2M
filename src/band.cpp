#include "band.h"
#include <FatFS.h>

#define BAND_DETECT_PIN 17
#define FORCE80_FLAG "/force80.flag"

static BandMode detected = BAND_2M;
static bool gp17Low = false;
static bool flagged = false;

bool bandForceFlagPresent() {
  return FatFS.exists(FORCE80_FLAG);
}

void bandWriteForceFlag() {
  File f = FatFS.open(FORCE80_FLAG, "w");
  if (f) {
    f.print("force80");
    f.close();
  }
}

void bandRemoveForceFlag() {
  if (FatFS.exists(FORCE80_FLAG))
    FatFS.remove(FORCE80_FLAG);
}

void bandBegin() {
  pinMode(BAND_DETECT_PIN, INPUT_PULLUP);
  delay(5);
  gp17Low = (digitalRead(BAND_DETECT_PIN) == LOW);
  flagged = bandForceFlagPresent();
  detected = (gp17Low || flagged) ? BAND_80M : BAND_2M;
  Serial.print("[band] GP17=");
  Serial.print(gp17Low ? "LOW" : "high");
  Serial.print(" force80.flag=");
  Serial.print(flagged ? "yes" : "no");
  Serial.print(" -> ");
  Serial.println(detected == BAND_80M ? "80M" : "2M");
}

BandMode bandDetected() { return detected; }
