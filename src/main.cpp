#include "Wire.h"
#include "si5351.h"
#include <Arduino.h>

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0

Si5351 si5351;

uint16_t duration = 85; // Morse code typing speed in milliseconds - higher
                        // number means slower typing
uint16_t hz = 750;      // The More Tone in HZ
String cw_message = "VVV de ON6URE  LOCATOR IS JO20cw  PWR IS 32mW  ANT IS A "
                    "MINI NAGOYA VERTICAL"; // Your message

// #define PLLB_FREQ 87270000000ULL 6xFREQ
#define PLLB_FREQ 86865000000ULL

void babyelephantwalk() {
  // change this to make the song slower or faster
  int tempo = 132;

  // change this to whichever pin you want to use

  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an
  // eighteenth!!
  int melody[] = {

      // Baby Elephant Walk
      // Score available at https://musescore.com/user/7965776/scores/1862611

      NOTE_C4, -8, NOTE_E4,  16, NOTE_G4,  8,  NOTE_C5,  8,  NOTE_E5,  8,
      NOTE_D5, 8,  NOTE_C5,  8,  NOTE_A4,  8,  NOTE_FS4, 8,  NOTE_G4,  8,
      REST,    4,  REST,     2,  NOTE_C4,  -8, NOTE_E4,  16, NOTE_G4,  8,
      NOTE_C5, 8,  NOTE_E5,  8,  NOTE_D5,  8,  NOTE_C5,  8,  NOTE_A4,  8,
      NOTE_G4, -2, NOTE_A4,  8,  NOTE_DS4, 1,

      NOTE_A4, 8,  NOTE_E4,  8,  NOTE_C4,  8,  REST,     4,  REST,     2,
      NOTE_C4, -8, NOTE_E4,  16, NOTE_G4,  8,  NOTE_C5,  8,  NOTE_E5,  8,
      NOTE_D5, 8,  NOTE_C5,  8,  NOTE_A4,  8,  NOTE_FS4, 8,  NOTE_G4,  8,
      REST,    4,  REST,     4,  REST,     8,  NOTE_G4,  8,  NOTE_D5,  4,
      NOTE_D5, 4,  NOTE_B4,  8,  NOTE_G4,  8,  REST,     8,  NOTE_G4,  8,

      NOTE_C5, 4,  NOTE_C5,  4,  NOTE_AS4, 16, NOTE_C5,  16, NOTE_AS4, 16,
      NOTE_G4, 16, NOTE_F4,  8,  NOTE_DS4, 8,  NOTE_FS4, 4,  NOTE_FS4, 4,
      NOTE_F4, 16, NOTE_G4,  16, NOTE_F4,  16, NOTE_DS4, 16, NOTE_C4,  8,
      NOTE_G4, 8,  NOTE_AS4, 8,  NOTE_C5,  8,  REST,     4,  REST,     2,
  };

  // sizeof gives the number of bytes, each int value is composed of two bytes
  // (16 bits) there are two values per note (pitch and duration), so for each
  // note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(15, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(15);
  }
}

void doom() {
  // change this to make the song slower or faster
  int tempo = 225;

  // change this to whichever pin you want to use
  int buzzer = 11;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an
  // eighteenth!! This code uses PROGMEM to fit the melody to flash as it was
  // to long to fit in SRAM. It may not work on other Arduino arquitectures
  // other than AVR
  const int melody[] PROGMEM = {

      // At Doom's Gate (E1M1)
      // Score available at https://musescore.com/pieridot/doom

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 1
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 5
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 9
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 13
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_FS3,
      -16,
      NOTE_D3,
      -16,
      NOTE_B2,
      -16,
      NOTE_A3,
      -16,
      NOTE_FS3,
      -16,
      NOTE_B2,
      -16,
      NOTE_D3,
      -16,
      NOTE_FS3,
      -16,
      NOTE_A3,
      -16,
      NOTE_FS3,
      -16,
      NOTE_D3,
      -16,
      NOTE_B2,
      -16,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 17
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 21
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_E3,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_E4,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_E4,
      -16,
      NOTE_B3,
      -16,
      NOTE_G4,
      -16,
      NOTE_B4,
      -16,

      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8, // 25
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_DS3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_E3,
      8,
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_DS3,
      -2,

      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8, // 29
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_DS3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_E3,
      8,
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,
      NOTE_C4,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 33
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 37
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_CS3,
      8,
      NOTE_CS3,
      8,
      NOTE_CS4,
      8,
      NOTE_CS3,
      8,
      NOTE_CS3,
      8,
      NOTE_B3,
      8,
      NOTE_CS3,
      8,
      NOTE_CS3,
      8, // 41
      NOTE_A3,
      8,
      NOTE_CS3,
      8,
      NOTE_CS3,
      8,
      NOTE_G3,
      8,
      NOTE_CS3,
      8,
      NOTE_CS3,
      8,
      NOTE_GS3,
      8,
      NOTE_A3,
      8,
      NOTE_B2,
      8,
      NOTE_B2,
      8,
      NOTE_B3,
      8,
      NOTE_B2,
      8,
      NOTE_B2,
      8,
      NOTE_A3,
      8,
      NOTE_B2,
      8,
      NOTE_B2,
      8,
      NOTE_G3,
      8,
      NOTE_B2,
      8,
      NOTE_B2,
      8,
      NOTE_F3,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 45
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_E3,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_E4,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_E4,
      -16,
      NOTE_B3,
      -16,
      NOTE_G4,
      -16,
      NOTE_B4,
      -16,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 49
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 53
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_FS3,
      -16,
      NOTE_DS3,
      -16,
      NOTE_B2,
      -16,
      NOTE_FS3,
      -16,
      NOTE_DS3,
      -16,
      NOTE_B2,
      -16,
      NOTE_G3,
      -16,
      NOTE_D3,
      -16,
      NOTE_B2,
      -16,
      NOTE_DS4,
      -16,
      NOTE_DS3,
      -16,
      NOTE_B2,
      -16,

      // -/-

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 57
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 61
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E4,
      -16,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_G4,
      -16,
      NOTE_E4,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_D4,
      -16,
      NOTE_E4,
      -16,
      NOTE_G4,
      -16,
      NOTE_E4,
      -16,
      NOTE_G3,
      -16,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 65
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8, // 69
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_DS3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_E3,
      8,
      NOTE_F3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_G3,
      8,
      NOTE_A2,
      8,
      NOTE_A2,
      8,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,
      NOTE_C4,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_A3,
      -16,
      NOTE_F3,
      -16,
      NOTE_D3,
      -16,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 73
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 77
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 81
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      -2,

      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8, // 73
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_AS2,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B2,
      8,
      NOTE_C3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_E3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_D3,
      8,
      NOTE_E2,
      8,
      NOTE_E2,
      8,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_E3,
      -16,
      NOTE_B2,
      -16,
      NOTE_E3,
      -16,
      NOTE_G3,
      -16,
      NOTE_C4,
      -16,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_B3,
      -16,
      NOTE_G3,
      -16,
      NOTE_E3,
      -16,
  };

  // sizeof gives the number of bytes, each int value is composed of two bytes
  // (16 bits) there are two values per note (pitch and duration), so for each
  // note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = pgm_read_word_near(melody + thisNote + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(15, pgm_read_word_near(melody + thisNote), noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(15);
  }
}

void supermario() {
  // change this to make the song slower or faster
  int tempo = 200;

  // change this to whichever pin you want to use
  int buzzer = 11;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an
  // eighteenth!!
  int melody[] = {

      // Super Mario Bros theme
      // Score available at https://musescore.com/user/2123/scores/2145
      // Theme by Koji Kondo

      NOTE_E5,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_E5,
      8, // 1
      NOTE_G5,
      4,
      REST,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_C5,
      -4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_E4,
      -4, // 3
      NOTE_A4,
      4,
      NOTE_B4,
      4,
      NOTE_AS4,
      8,
      NOTE_A4,
      4,
      NOTE_G4,
      -8,
      NOTE_E5,
      -8,
      NOTE_G5,
      -8,
      NOTE_A5,
      4,
      NOTE_F5,
      8,
      NOTE_G5,
      8,
      REST,
      8,
      NOTE_E5,
      4,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_B4,
      -4,
      NOTE_C5,
      -4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_E4,
      -4, // repeats from 3
      NOTE_A4,
      4,
      NOTE_B4,
      4,
      NOTE_AS4,
      8,
      NOTE_A4,
      4,
      NOTE_G4,
      -8,
      NOTE_E5,
      -8,
      NOTE_G5,
      -8,
      NOTE_A5,
      4,
      NOTE_F5,
      8,
      NOTE_G5,
      8,
      REST,
      8,
      NOTE_E5,
      4,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_B4,
      -4,

      REST,
      4,
      NOTE_G5,
      8,
      NOTE_FS5,
      8,
      NOTE_F5,
      8,
      NOTE_DS5,
      4,
      NOTE_E5,
      8, // 7
      REST,
      8,
      NOTE_GS4,
      8,
      NOTE_A4,
      8,
      NOTE_C4,
      8,
      REST,
      8,
      NOTE_A4,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      REST,
      4,
      NOTE_DS5,
      4,
      REST,
      8,
      NOTE_D5,
      -4,
      NOTE_C5,
      2,
      REST,
      2,

      REST,
      4,
      NOTE_G5,
      8,
      NOTE_FS5,
      8,
      NOTE_F5,
      8,
      NOTE_DS5,
      4,
      NOTE_E5,
      8, // repeats from 7
      REST,
      8,
      NOTE_GS4,
      8,
      NOTE_A4,
      8,
      NOTE_C4,
      8,
      REST,
      8,
      NOTE_A4,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      REST,
      4,
      NOTE_DS5,
      4,
      REST,
      8,
      NOTE_D5,
      -4,
      NOTE_C5,
      2,
      REST,
      2,

      NOTE_C5,
      8,
      NOTE_C5,
      4,
      NOTE_C5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      4, // 11
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2,

      NOTE_C5,
      8,
      NOTE_C5,
      4,
      NOTE_C5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_E5,
      8, // 13
      REST,
      1,
      NOTE_C5,
      8,
      NOTE_C5,
      4,
      NOTE_C5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      4,
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2,
      NOTE_E5,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_E5,
      4,
      NOTE_G5,
      4,
      REST,
      4,
      NOTE_G4,
      4,
      REST,
      4,
      NOTE_C5,
      -4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_E4,
      -4, // 19

      NOTE_A4,
      4,
      NOTE_B4,
      4,
      NOTE_AS4,
      8,
      NOTE_A4,
      4,
      NOTE_G4,
      -8,
      NOTE_E5,
      -8,
      NOTE_G5,
      -8,
      NOTE_A5,
      4,
      NOTE_F5,
      8,
      NOTE_G5,
      8,
      REST,
      8,
      NOTE_E5,
      4,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_B4,
      -4,

      NOTE_C5,
      -4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_E4,
      -4, // repeats from 19
      NOTE_A4,
      4,
      NOTE_B4,
      4,
      NOTE_AS4,
      8,
      NOTE_A4,
      4,
      NOTE_G4,
      -8,
      NOTE_E5,
      -8,
      NOTE_G5,
      -8,
      NOTE_A5,
      4,
      NOTE_F5,
      8,
      NOTE_G5,
      8,
      REST,
      8,
      NOTE_E5,
      4,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_B4,
      -4,

      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4, // 23
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_D5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_G5,
      -8,
      NOTE_F5,
      -8,

      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2, // 26
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4,
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_B4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_F5,
      -8,
      NOTE_E5,
      -8,
      NOTE_D5,
      -8,
      NOTE_C5,
      8,
      NOTE_E4,
      4,
      NOTE_E4,
      8,
      NOTE_C4,
      2,

      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4, // repeats from 23
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_D5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_G5,
      -8,
      NOTE_F5,
      -8,

      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2, // 26
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4,
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_B4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_F5,
      -8,
      NOTE_E5,
      -8,
      NOTE_D5,
      -8,
      NOTE_C5,
      8,
      NOTE_E4,
      4,
      NOTE_E4,
      8,
      NOTE_C4,
      2,
      NOTE_C5,
      8,
      NOTE_C5,
      4,
      NOTE_C5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      8,
      NOTE_E5,
      8,
      REST,
      1,

      NOTE_C5,
      8,
      NOTE_C5,
      4,
      NOTE_C5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_D5,
      4, // 33
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2,
      NOTE_E5,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_E5,
      8,
      REST,
      8,
      NOTE_C5,
      8,
      NOTE_E5,
      4,
      NOTE_G5,
      4,
      REST,
      4,
      NOTE_G4,
      4,
      REST,
      4,
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4,
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_D5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_A5,
      -8,
      NOTE_G5,
      -8,
      NOTE_F5,
      -8,

      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_A4,
      8,
      NOTE_G4,
      2, // 40
      NOTE_E5,
      8,
      NOTE_C5,
      4,
      NOTE_G4,
      8,
      REST,
      4,
      NOTE_GS4,
      4,
      NOTE_A4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_A4,
      2,
      NOTE_B4,
      8,
      NOTE_F5,
      4,
      NOTE_F5,
      8,
      NOTE_F5,
      -8,
      NOTE_E5,
      -8,
      NOTE_D5,
      -8,
      NOTE_C5,
      8,
      NOTE_E4,
      4,
      NOTE_E4,
      8,
      NOTE_C4,
      2,

      // game over sound
      NOTE_C5,
      -4,
      NOTE_G4,
      -4,
      NOTE_E4,
      4, // 45
      NOTE_A4,
      -8,
      NOTE_B4,
      -8,
      NOTE_A4,
      -8,
      NOTE_GS4,
      -8,
      NOTE_AS4,
      -8,
      NOTE_GS4,
      -8,
      NOTE_G4,
      8,
      NOTE_D4,
      8,
      NOTE_E4,
      -2,

  };

  // sizeof gives the number of bytes, each int value is composed of two bytes
  // (16 bits) there are two values per note (pitch and duration), so for each
  // note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(15, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(15);
  }
}

void nokia() {
  // change this to make the song slower or faster
  int tempo = 180;

  // change this to whichever pin you want to use
  int buzzer = 11;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an
  // eighteenth!!
  int melody[] = {

      // Nokia Ringtone
      // Score available at https://musescore.com/user/29944637/scores/5266155

      NOTE_E5,  8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, NOTE_CS5, 8,
      NOTE_B4,  8, NOTE_D4, 4, NOTE_E4,  4, NOTE_B4,  8, NOTE_A4,  8,
      NOTE_CS4, 4, NOTE_E4, 4, NOTE_A4,  2,
  };

  // sizeof gives the number of bytes, each int value is composed of two bytes
  // (16 bits) there are two values per note (pitch and duration), so for each
  // note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(15, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(15);
  }
}

void pacman() {
  // change this to make the song slower or faster
  int tempo = 105;

  // change this to whichever pin you want to use
  int buzzer = 11;

  // notes of the moledy followed by the duration.
  // a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
  // !!negative numbers are used to represent dotted notes,
  // so -4 means a dotted quarter note, that is, a quarter plus an
  // eighteenth!!
  int melody[] = {

      // Pacman
      // Score available at https://musescore.com/user/85429/scores/107109
      NOTE_B4,  16,  NOTE_B5,  16,  NOTE_FS5, 16, NOTE_DS5, 16, // 1
      NOTE_B5,  32,  NOTE_FS5, -16, NOTE_DS5, 8,  NOTE_C5,  16,
      NOTE_C6,  16,  NOTE_G6,  16,  NOTE_E6,  16, NOTE_C6,  32,
      NOTE_G6,  -16, NOTE_E6,  8,

      NOTE_B4,  16,  NOTE_B5,  16,  NOTE_FS5, 16, NOTE_DS5, 16,
      NOTE_B5,  32, // 2
      NOTE_FS5, -16, NOTE_DS5, 8,   NOTE_DS5, 32, NOTE_E5,  32,
      NOTE_F5,  32,  NOTE_F5,  32,  NOTE_FS5, 32, NOTE_G5,  32,
      NOTE_G5,  32,  NOTE_GS5, 32,  NOTE_A5,  16, NOTE_B5,  8};

  // sizeof gives the number of bytes, each int value is composed of two bytes
  // (16 bits) there are two values per note (pitch and duration), so for each
  // note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(15, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(15);
  }
}

void cw(bool state) {
  if (state) {
    tone(15, hz);
  } else {
    tone(15, 0);
  }
}

void ring(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Ring");
    tone(15, 425, 1000);
    delay(1000);
    tone(15, 0, 3000);
    delay(3000);
  }
}

void busy(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Busy");
    tone(15, 425, 500);
    delay(500);
    tone(15, 0, 500);
    delay(500);
  }
}

void congestion(int x) {
  for (int i = 0; i < x; i++) {
    Serial.println("Congestion");
    tone(15, 425, 167);
    delay(167);
    tone(15, 0, 167);
    delay(167);
  }
}

//----------
void di() {
  cw(true); // TX di
  delay(duration);

  cw(false); // stop TX di
  delay(duration);
}
//----------
void dah() {
  cw(true); // TX dah
  delay(3 * duration);

  cw(false); // stop TX dah
  delay(duration);
}
//----------
void char_space() {    // 3x, pause between letters
  delay(2 * duration); // 1x from end of character + 2x from the beginning of
                       // new character
}
//----------
void word_space() {    // 7x, pause between words
  delay(6 * duration); // 1x from end of the word + 6x from the beginning of
                       // new word
}

//----------
void cw_char_proc(char m) { // Processing characters to Morse symbols
  String s;

  if (m == ' ') { // Pause between words
    word_space();
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
      di();
      break; // di
    case '-':
      Serial.print("-");
      dah();
      break; // dah
    case '#':
      Serial.print(" ");
      char_space();
      return; // end of morse symbol
    }
  }
}

// Morse looper
void cw_string_proc(String str) {
  Serial.println("Starting to transmit CW message: " + str);
  Serial.println("");
  for (uint8_t j = 0; j < str.length(); j++)
    cw_char_proc(str[j]);
}

void setup() {
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  // reset the SI chip
  digitalWrite(3, LOW);
  Serial.println("SmartFOX booted !");
  // start the SI chip
  digitalWrite(3, HIGH);

  Wire1.setSDA(26);
  Wire1.setSCL(27);
  delay(10);

  bool i2c_found;

  // Start serial and initialize the Si5351
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_0PF, 250000000, 24000);
  if (!i2c_found) {
    Serial.println("Device not found on I2C bus!");
  }
  si5351.reset();
  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLB);
  si5351.set_freq(14477500000ULL, SI5351_CLK0);
  si5351.set_vcxo(PLLB_FREQ, 50); // pull weight
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.set_clock_pwr(SI5351_CLK1, 0);
  si5351.set_clock_pwr(SI5351_CLK2, 0);
  si5351.set_clock_pwr(SI5351_CLK3, 0);
  si5351.set_clock_pwr(SI5351_CLK4, 0);
  si5351.set_clock_pwr(SI5351_CLK5, 0);
  si5351.set_clock_pwr(SI5351_CLK6, 0);
  si5351.set_clock_pwr(SI5351_CLK7, 0);
}

void loop() {
  si5351.output_enable(SI5351_CLK0, 1);

  babyelephantwalk();
  delay(2000);
  doom();
  delay(2000);
  supermario();
  delay(2000);
  pacman();
  delay(2000);
  nokia();
  delay(2000);

  // 5 rings
  ring(5);
  // busy(3);
  // congestion(3);

  delay(500);
  // Keydown for 2 seconds
  cw(true);
  delay(2000);
  cw(false);
  delay(2000);

  // cw message
  cw_string_proc(cw_message);

  si5351.output_enable(SI5351_CLK0, 0);

  delay(20000);
}