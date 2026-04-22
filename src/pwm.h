#include <Arduino.h>
#include <PWMAudio.h>

extern PWMAudio pwm;

// True while playWAV / playMOD is actively rendering. Used by the FatFSUSB
// driveReady callback to tell the host the drive is not ready, so MSC reads
// don't stall the XIP cache (and FatFS) mid-playback and click the audio.
extern volatile bool audioBusy;

void playPWM();
void playWAV(const char *path);

// Park pin 15 at 50%-duty inaudible PWM so the FM modulator's DC input stays
// at mid-rail. Call this after any tone/WAV/MOD ends to kill the "stop click".
void audioIdle();

// Click-free PWM tone generator for pin 15. Uses a PWM slice (not tone()'s PIO)
// so switching to audioIdle() only changes frequency — no peripheral swap,
// no dip to 0 V, no modulator click.
void audioTone(int hz);
void audioTone(int hz, int duty1024);  // explicit duty 0..1024, for CW volume
