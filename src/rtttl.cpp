#include "rtttl.h"
#include <FatFS.h>
#include <PlayRtttl.hpp>

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
  Serial.print("RTTTL: ");
  Serial.println(path);
  playRtttlBlocking(15, s.c_str());
}
