#include "rtttl.h"
#include "pwm.h"
#include <FatFS.h>
#include <ctype.h>

// Octave-4 frequencies (Hz), indexed C, C#, D, D#, E, F, F#, G, G#, A, A#, B.
static const uint16_t notesOct4[12] = {
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};

static int noteFreq(char base, bool sharp, int octave) {
  int idx;
  switch (base) {
    case 'c': idx = sharp ? 1 : 0; break;
    case 'd': idx = sharp ? 3 : 2; break;
    case 'e': idx = 4; break;
    case 'f': idx = sharp ? 6 : 5; break;
    case 'g': idx = sharp ? 8 : 7; break;
    case 'a': idx = sharp ? 10 : 9; break;
    case 'b': idx = 11; break;
    default: return 0;
  }
  int f = notesOct4[idx];
  int shift = octave - 4;
  if (shift >= 0) f <<= shift;
  else            f >>= -shift;
  return f;
}

static void skipWS(const char *&p) {
  while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
    p++;
}

void playRTTTL(const char *path) {
  File f = FatFS.open(path, "r");
  if (!f) {
    Serial.print("RTTTL not found: ");
    Serial.println(path);
    return;
  }
  String s;
  s.reserve(f.size() + 1);
  while (f.available())
    s += (char)f.read();
  f.close();
  s.trim();
  if (s.length() == 0) {
    Serial.print("RTTTL empty: ");
    Serial.println(path);
    return;
  }
  s.toLowerCase();

  int c1 = s.indexOf(':');
  int c2 = s.indexOf(':', c1 + 1);
  if (c1 < 0 || c2 < 0) {
    Serial.println("RTTTL: bad header");
    return;
  }
  String defaults = s.substring(c1 + 1, c2);
  String notes = s.substring(c2 + 1);

  int defDur = 4, defOct = 6, bpm = 63;
  int pos = 0;
  while (pos < (int)defaults.length()) {
    int eq = defaults.indexOf('=', pos);
    int end = defaults.indexOf(',', pos);
    if (end < 0) end = defaults.length();
    if (eq > pos && eq < end) {
      char key = defaults.charAt(pos);
      int val = defaults.substring(eq + 1, end).toInt();
      if (val > 0) {
        if (key == 'd') defDur = val;
        else if (key == 'o') defOct = val;
        else if (key == 'b') bpm = val;
      }
    }
    pos = end + 1;
  }

  long wholeMs = 60000L * 4L / (bpm > 0 ? bpm : 63);

  Serial.print("RTTTL: ");
  Serial.println(path);

  audioBusy = true;
  const char *p = notes.c_str();
  while (*p) {
    skipWS(p);
    if (!*p) break;

    int dur = defDur;
    if (isdigit((unsigned char)*p)) {
      dur = 0;
      while (isdigit((unsigned char)*p))
        dur = dur * 10 + (*p++ - '0');
      if (dur <= 0) dur = defDur;
    }

    skipWS(p);
    char base = *p;
    if (!base) break;
    p++;

    bool sharp = false;
    if (*p == '#') { sharp = true; p++; }

    bool dotted = false;
    if (*p == '.') { dotted = true; p++; }

    int oct = defOct;
    if (isdigit((unsigned char)*p)) {
      oct = *p - '0';
      p++;
    }

    // A few RTTTLs put the dot after the octave.
    if (*p == '.') { dotted = true; p++; }

    skipWS(p);
    if (*p == ',') p++;

    long noteMs = wholeMs / dur;
    if (dotted) noteMs = noteMs * 3 / 2;

    int hz = (base == 'p') ? 0 : noteFreq(base, sharp, oct);

    if (hz <= 0) {
      audioIdle();
      delay(noteMs);
    } else {
      // 15/16 tone + 1/16 silence for per-note articulation, matching
      // PlayRtttl's "natural" style. Keeps pin 15 on the PWM slice the whole
      // time — only the frequency changes — so there's no DC step on the
      // FM modulator between notes.
      long toneMs = noteMs - (noteMs >> 4);
      long silMs = noteMs - toneMs;
      audioTone(hz);
      delay(toneMs);
      audioIdle();
      if (silMs > 0) delay(silMs);
    }
  }
  audioIdle();
  audioBusy = false;
}
