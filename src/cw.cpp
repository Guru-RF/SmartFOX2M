#include "cw.h"

void cw(bool state, int hz) {
  if (state) {
    tone(15, hz);
  } else {
    tone(15, 0);
  }
}

//----------
void di(int hz, int duration) {
  cw(true, hz); // TX di
  delay(duration);

  cw(false, hz); // stop TX di
  delay(duration);
}
//----------
void dah(int hz, int duration) {
  cw(true, hz); // TX dah
  delay(3 * duration);

  cw(false, hz); // stop TX dah
  delay(duration);
}
//----------
void char_space(int duration) { // 3x, pause between letters
  delay(2 * duration); // 1x from end of character + 2x from the beginning of
                       // new character
}
//----------
void word_space(int duration) { // 7x, pause between words
  delay(6 * duration); // 1x from end of the word + 6x from the beginning of
                       // new word
}

//----------
void cw_char_proc(char m, int hz,
                  int duration) { // Processing characters to Morse symbols
  String s;

  if (m == ' ') { // Pause between words
    word_space(duration);
    return;
  }

  if (m > 96) // ACSII, case change a-z to A-Z
    if (m < 123)
      m -= 32;

  switch (m) { // Morse
  case 'A':
    s = ".-#";
    break;
  case 'B':
    s = "-...#";
    break;
  case 'C':
    s = "-.-.#";
    break;
  case 'D':
    s = "-..#";
    break;
  case 'E':
    s = ".#";
    break;
  case 'F':
    s = "..-.#";
    break;
  case 'G':
    s = "--.#";
    break;
  case 'H':
    s = "....#";
    break;
  case 'I':
    s = "..#";
    break;
  case 'J':
    s = ".---#";
    break;
  case 'K':
    s = "-.-#";
    break;
  case 'L':
    s = ".-..#";
    break;
  case 'M':
    s = "--#";
    break;
  case 'N':
    s = "-.#";
    break;
  case 'O':
    s = "---#";
    break;
  case 'P':
    s = ".--.#";
    break;
  case 'Q':
    s = "--.-#";
    break;
  case 'R':
    s = ".-.#";
    break;
  case 'S':
    s = "...#";
    break;
  case 'T':
    s = "-#";
    break;
  case 'U':
    s = "..-#";
    break;
  case 'V':
    s = "...-#";
    break;
  case 'W':
    s = ".--#";
    break;
  case 'X':
    s = "-..-#";
    break;
  case 'Y':
    s = "-.--#";
    break;
  case 'Z':
    s = "--..#";
    break;

  case '1':
    s = ".----#";
    break;
  case '2':
    s = "..---#";
    break;
  case '3':
    s = "...--#";
    break;
  case '4':
    s = "....-#";
    break;
  case '5':
    s = ".....#";
    break;
  case '6':
    s = "-....#";
    break;
  case '7':
    s = "--...#";
    break;
  case '8':
    s = "---..#";
    break;
  case '9':
    s = "----.#";
    break;
  case '0':
    s = "-----#";
    break;

  case '?':
    s = "..--..#";
    break;
  case '=':
    s = "-...-#";
    break;
  case ',':
    s = "--..--#";
    break;
  case '/':
    s = "-..-.#";
    break;
  }

  for (uint8_t i = 0; i < 7; i++) {
    switch (s[i]) {
    case '.':
      Serial.print(".");
      di(hz, duration);
      break; // di
    case '-':
      Serial.print("-");
      dah(hz, duration);
      break; // dah
    case '#':
      Serial.print(" ");
      char_space(duration);
      return; // end of morse symbol
    }
  }
}

// Morse looper
void cw_string_proc(String str, int hz, int duration) {
  Serial.println("Starting to transmit CW message: " + str);
  Serial.println("");
  for (uint8_t j = 0; j < str.length(); j++)
    cw_char_proc(str[j], hz, duration);
}