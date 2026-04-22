#include <Arduino.h>

enum BandMode { BAND_2M, BAND_80M };

// Detect the target band at boot:
//   - GP17 pulled to GND = 80M PCB
//   - /force80.flag present on FatFS = force 80M regardless
// Result is cached; accessors are cheap.
void bandBegin();
BandMode bandDetected();
bool bandForceFlagPresent();
void bandWriteForceFlag();
void bandRemoveForceFlag();
