#include "music.h"

const int doomMelody[] = {
    // At Doom's Gate (E1M1)
    // Score available at https://musescore.com/pieridot/doom
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_FS3, -16, NOTE_D3,  -16,
    NOTE_B2,  -16, NOTE_A3,  -16, NOTE_FS3, -16, NOTE_B2,  -16, NOTE_D3,  -16,
    NOTE_FS3, -16, NOTE_A3,  -16, NOTE_FS3, -16, NOTE_D3,  -16, NOTE_B2,  -16,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B3,  -16, NOTE_G3,  -16, NOTE_E3,  -16,
    NOTE_G3,  -16, NOTE_B3,  -16, NOTE_E4,  -16, NOTE_G3,  -16, NOTE_B3,  -16,
    NOTE_E4,  -16, NOTE_B3,  -16, NOTE_G4,  -16, NOTE_B4,  -16, NOTE_A2,  8,
    NOTE_A2,  8,   NOTE_A3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_G3,  8,
    NOTE_A2,  8,   NOTE_A2,  8,   NOTE_F3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_DS3, 8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_E3,  8,   NOTE_F3,  8,
    NOTE_A2,  8,   NOTE_A2,  8,   NOTE_A3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_G3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_F3,  8,   NOTE_A2,  8,
    NOTE_A2,  8,   NOTE_DS3, -2,  NOTE_A2,  8,   NOTE_A2,  8,   NOTE_A3,  8,
    NOTE_A2,  8,   NOTE_A2,  8,   NOTE_G3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_F3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_DS3, 8,   NOTE_A2,  8,
    NOTE_A2,  8,   NOTE_E3,  8,   NOTE_F3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_A3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_G3,  8,   NOTE_A2,  8,
    NOTE_A2,  8,   NOTE_A3,  -16, NOTE_F3,  -16, NOTE_D3,  -16, NOTE_A3,  -16,
    NOTE_F3,  -16, NOTE_D3,  -16, NOTE_C4,  -16, NOTE_A3,  -16, NOTE_F3,  -16,
    NOTE_A3,  -16, NOTE_F3,  -16, NOTE_D3,  -16, NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_CS3, 8,
    NOTE_CS3, 8,   NOTE_CS4, 8,   NOTE_CS3, 8,   NOTE_CS3, 8,   NOTE_B3,  8,
    NOTE_CS3, 8,   NOTE_CS3, 8,   NOTE_A3,  8,   NOTE_CS3, 8,   NOTE_CS3, 8,
    NOTE_G3,  8,   NOTE_CS3, 8,   NOTE_CS3, 8,   NOTE_GS3, 8,   NOTE_A3,  8,
    NOTE_B2,  8,   NOTE_B2,  8,   NOTE_B3,  8,   NOTE_B2,  8,   NOTE_B2,  8,
    NOTE_A3,  8,   NOTE_B2,  8,   NOTE_B2,  8,   NOTE_G3,  8,   NOTE_B2,  8,
    NOTE_B2,  8,   NOTE_F3,  -2,  NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_B3,  -16, NOTE_G3,  -16, NOTE_E3,  -16, NOTE_G3,  -16,
    NOTE_B3,  -16, NOTE_E4,  -16, NOTE_G3,  -16, NOTE_B3,  -16, NOTE_E4,  -16,
    NOTE_B3,  -16, NOTE_G4,  -16, NOTE_B4,  -16, NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_FS3, -16, NOTE_DS3, -16, NOTE_B2,  -16, NOTE_FS3, -16, NOTE_DS3, -16,
    NOTE_B2,  -16, NOTE_G3,  -16, NOTE_D3,  -16, NOTE_B2,  -16, NOTE_DS4, -16,
    NOTE_DS3, -16, NOTE_B2,  -16, NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E4,  -16,
    NOTE_B3,  -16, NOTE_G3,  -16, NOTE_G4,  -16, NOTE_E4,  -16, NOTE_G3,  -16,
    NOTE_B3,  -16, NOTE_D4,  -16, NOTE_E4,  -16, NOTE_G4,  -16, NOTE_E4,  -16,
    NOTE_G3,  -16, NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_A2,  8,
    NOTE_A2,  8,   NOTE_A3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_G3,  8,
    NOTE_A2,  8,   NOTE_A2,  8,   NOTE_F3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_DS3, 8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_E3,  8,   NOTE_F3,  8,
    NOTE_A2,  8,   NOTE_A2,  8,   NOTE_A3,  8,   NOTE_A2,  8,   NOTE_A2,  8,
    NOTE_G3,  8,   NOTE_A2,  8,   NOTE_A2,  8,   NOTE_A3,  -16, NOTE_F3,  -16,
    NOTE_D3,  -16, NOTE_A3,  -16, NOTE_F3,  -16, NOTE_D3,  -16, NOTE_C4,  -16,
    NOTE_A3,  -16, NOTE_F3,  -16, NOTE_A3,  -16, NOTE_F3,  -16, NOTE_D3,  -16,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, -2,  NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_B2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_AS2, -2,  NOTE_E2,  8,
    NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_D3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_C3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_AS2, 8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B2,  8,   NOTE_C3,  8,
    NOTE_E2,  8,   NOTE_E2,  8,   NOTE_E3,  8,   NOTE_E2,  8,   NOTE_E2,  8,
    NOTE_D3,  8,   NOTE_E2,  8,   NOTE_E2,  8,   NOTE_B3,  -16, NOTE_G3,  -16,
    NOTE_E3,  -16, NOTE_B2,  -16, NOTE_E3,  -16, NOTE_G3,  -16, NOTE_C4,  -16,
    NOTE_B3,  -16, NOTE_G3,  -16, NOTE_B3,  -16, NOTE_G3,  -16, NOTE_E3,  -16,
};

void playMelody(const int *melody, int notes, int tempo) {
  int wholenote = (60000 * 4) / tempo;
  int divider, noteDuration;

  for (int i = 0; i < notes * 2; i += 2) {
    divider = melody[i + 1];
    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else {
      noteDuration = (wholenote / abs(divider)) * 1.5;
    }

    tone(15, melody[i], noteDuration * 0.9); // Play the note
    delay(noteDuration);                     // Wait for the duration
    noTone(15);                              // Stop the tone
  }
}

void babyelephantwalk() {
  Serial.println("Playing babyelephantwalk");
  int melody[] = {
      // Baby Elephant Walk
      // Score available at https://musescore.com/user/7965776/scores/1862611
      NOTE_C4,  -8, NOTE_E4,  16, NOTE_G4,  8,  NOTE_C5,  8,  NOTE_E5,  8,
      NOTE_D5,  8,  NOTE_C5,  8,  NOTE_A4,  8,  NOTE_FS4, 8,  NOTE_G4,  8,
      REST,     4,  REST,     2,  NOTE_C4,  -8, NOTE_E4,  16, NOTE_G4,  8,
      NOTE_C5,  8,  NOTE_E5,  8,  NOTE_D5,  8,  NOTE_C5,  8,  NOTE_A4,  8,
      NOTE_G4,  -2, NOTE_A4,  8,  NOTE_DS4, 1,  NOTE_A4,  8,  NOTE_E4,  8,
      NOTE_C4,  8,  REST,     4,  REST,     2,  NOTE_C4,  -8, NOTE_E4,  16,
      NOTE_G4,  8,  NOTE_C5,  8,  NOTE_E5,  8,  NOTE_D5,  8,  NOTE_C5,  8,
      NOTE_A4,  8,  NOTE_FS4, 8,  NOTE_G4,  8,  REST,     4,  REST,     4,
      REST,     8,  NOTE_G4,  8,  NOTE_D5,  4,  NOTE_D5,  4,  NOTE_B4,  8,
      NOTE_G4,  8,  REST,     8,  NOTE_G4,  8,  NOTE_C5,  4,  NOTE_C5,  4,
      NOTE_AS4, 16, NOTE_C5,  16, NOTE_AS4, 16, NOTE_G4,  16, NOTE_F4,  8,
      NOTE_DS4, 8,  NOTE_FS4, 4,  NOTE_FS4, 4,  NOTE_F4,  16, NOTE_G4,  16,
      NOTE_F4,  16, NOTE_DS4, 16, NOTE_C4,  8,  NOTE_G4,  8,  NOTE_AS4, 8,
      NOTE_C5,  8,  REST,     4,  REST,     2,
  };
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  playMelody(melody, notes, 132);
}

void doom() {
  Serial.println("Playing doom");
  int notes = sizeof(doomMelody) / sizeof(doomMelody[0]) / 2;
  playMelody(doomMelody, notes, 225);
}

void supermario() {
  Serial.println("Playing supermario");
  int melody[] = {
      // Super Mario Bros theme
      // Score available at https://musescore.com/user/2123/scores/2145
      // Theme by Koji Kondo
      NOTE_E5,  8,  NOTE_E5,  8,  REST,     8,  NOTE_E5,  8,  REST,     8,
      NOTE_C5,  8,  NOTE_E5,  8,  NOTE_G5,  4,  REST,     4,  NOTE_G4,  8,
      REST,     4,  NOTE_C5,  -4, NOTE_G4,  8,  REST,     4,  NOTE_E4,  -4,
      NOTE_A4,  4,  NOTE_B4,  4,  NOTE_AS4, 8,  NOTE_A4,  4,  NOTE_G4,  -8,
      NOTE_E5,  -8, NOTE_G5,  -8, NOTE_A5,  4,  NOTE_F5,  8,  NOTE_G5,  8,
      REST,     8,  NOTE_E5,  4,  NOTE_C5,  8,  NOTE_D5,  8,  NOTE_B4,  -4,
      NOTE_C5,  -4, NOTE_G4,  8,  REST,     4,  NOTE_E4,  -4, NOTE_A4,  4,
      NOTE_B4,  4,  NOTE_AS4, 8,  NOTE_A4,  4,  NOTE_G4,  -8, NOTE_E5,  -8,
      NOTE_G5,  -8, NOTE_A5,  4,  NOTE_F5,  8,  NOTE_G5,  8,  REST,     8,
      NOTE_E5,  4,  NOTE_C5,  8,  NOTE_D5,  8,  NOTE_B4,  -4, REST,     4,
      NOTE_G5,  8,  NOTE_FS5, 8,  NOTE_F5,  8,  NOTE_DS5, 4,  NOTE_E5,  8,
      REST,     8,  NOTE_GS4, 8,  NOTE_A4,  8,  NOTE_C4,  8,  REST,     8,
      NOTE_A4,  8,  NOTE_C5,  8,  NOTE_D5,  8,  REST,     4,  NOTE_DS5, 4,
      REST,     8,  NOTE_D5,  -4, NOTE_C5,  2,  REST,     2,  REST,     4,
      NOTE_G5,  8,  NOTE_FS5, 8,  NOTE_F5,  8,  NOTE_DS5, 4,  NOTE_E5,  8,
      REST,     8,  NOTE_GS4, 8,  NOTE_A4,  8,  NOTE_C4,  8,  REST,     8,
      NOTE_A4,  8,  NOTE_C5,  8,  NOTE_D5,  8,  REST,     4,  NOTE_DS5, 4,
      REST,     8,  NOTE_D5,  -4, NOTE_C5,  2,  REST,     2,  NOTE_C5,  8,
      NOTE_C5,  4,  NOTE_C5,  8,  REST,     8,  NOTE_C5,  8,  NOTE_D5,  4,
      NOTE_E5,  8,  NOTE_C5,  4,  NOTE_A4,  8,  NOTE_G4,  2,  NOTE_C5,  8,
      NOTE_C5,  4,  NOTE_C5,  8,  REST,     8,  NOTE_C5,  8,  NOTE_D5,  8,
      NOTE_E5,  8,  REST,     1,  NOTE_C5,  8,  NOTE_C5,  4,  NOTE_C5,  8,
      REST,     8,  NOTE_C5,  8,  NOTE_D5,  4,  NOTE_E5,  8,  NOTE_C5,  4,
      NOTE_A4,  8,  NOTE_G4,  2,  NOTE_E5,  8,  NOTE_E5,  8,  REST,     8,
      NOTE_E5,  8,  REST,     8,  NOTE_C5,  8,  NOTE_E5,  4,  NOTE_G5,  4,
      REST,     4,  NOTE_G4,  4,  REST,     4,  NOTE_C5,  -4, NOTE_G4,  8,
      REST,     4,  NOTE_E4,  -4, NOTE_A4,  4,  NOTE_B4,  4,  NOTE_AS4, 8,
      NOTE_A4,  4,  NOTE_G4,  -8, NOTE_E5,  -8, NOTE_G5,  -8, NOTE_A5,  4,
      NOTE_F5,  8,  NOTE_G5,  8,  REST,     8,  NOTE_E5,  4,  NOTE_C5,  8,
      NOTE_D5,  8,  NOTE_B4,  -4, NOTE_C5,  -4, NOTE_G4,  8,  REST,     4,
      NOTE_E4,  -4, NOTE_A4,  4,  NOTE_B4,  4,  NOTE_AS4, 8,  NOTE_A4,  4,
      NOTE_G4,  -8, NOTE_E5,  -8, NOTE_G5,  -8, NOTE_A5,  4,  NOTE_F5,  8,
      NOTE_G5,  8,  REST,     8,  NOTE_E5,  4,  NOTE_C5,  8,  NOTE_D5,  8,
      NOTE_B4,  -4, NOTE_E5,  8,  NOTE_C5,  4,  NOTE_G4,  8,  REST,     4,
      NOTE_GS4, 4,  NOTE_A4,  8,  NOTE_F5,  4,  NOTE_F5,  8,  NOTE_A4,  2,
      NOTE_D5,  -8, NOTE_A5,  -8, NOTE_A5,  -8, NOTE_A5,  -8, NOTE_G5,  -8,
      NOTE_F5,  -8, NOTE_E5,  8,  NOTE_C5,  4,  NOTE_A4,  8,  NOTE_G4,  2,
      NOTE_E5,  8,  NOTE_C5,  4,  NOTE_G4,  8,  REST,     4,  NOTE_GS4, 4,
      NOTE_A4,  8,  NOTE_F5,  4,  NOTE_F5,  8,  NOTE_A4,  2,  NOTE_B4,  8,
      NOTE_F5,  4,  NOTE_F5,  8,  NOTE_F5,  -8, NOTE_E5,  -8, NOTE_D5,  -8,
      NOTE_C5,  8,  NOTE_E4,  4,  NOTE_E4,  8,  NOTE_C4,  2,  NOTE_E5,  8,
      NOTE_C5,  4,  NOTE_G4,  8,  REST,     4,  NOTE_GS4, 4,  NOTE_A4,  8,
      NOTE_F5,  4,  NOTE_F5,  8,  NOTE_A4,  2,  NOTE_D5,  -8, NOTE_A5,  -8,
      NOTE_A5,  -8, NOTE_A5,  -8, NOTE_G5,  -8, NOTE_F5,  -8, NOTE_E5,  8,
      NOTE_C5,  4,  NOTE_A4,  8,  NOTE_G4,  2,  NOTE_E5,  8,  NOTE_C5,  4,
      NOTE_G4,  8,  REST,     4,  NOTE_GS4, 4,  NOTE_A4,  8,  NOTE_F5,  4,
      NOTE_F5,  8,  NOTE_A4,  2,  NOTE_B4,  8,  NOTE_F5,  4,  NOTE_F5,  8,
      NOTE_F5,  -8, NOTE_E5,  -8, NOTE_D5,  -8, NOTE_C5,  8,  NOTE_E4,  4,
      NOTE_E4,  8,  NOTE_C4,  2,  NOTE_C5,  8,  NOTE_C5,  4,  NOTE_C5,  8,
      REST,     8,  NOTE_C5,  8,  NOTE_D5,  8,  NOTE_E5,  8,  REST,     1,
      NOTE_C5,  8,  NOTE_C5,  4,  NOTE_C5,  8,  REST,     8,  NOTE_C5,  8,
      NOTE_D5,  4,  NOTE_E5,  8,  NOTE_C5,  4,  NOTE_A4,  8,  NOTE_G4,  2,
      NOTE_E5,  8,  NOTE_E5,  8,  REST,     8,  NOTE_E5,  8,  REST,     8,
      NOTE_C5,  8,  NOTE_E5,  4,  NOTE_G5,  4,  REST,     4,  NOTE_G4,  4,
      REST,     4,  NOTE_E5,  8,  NOTE_C5,  4,  NOTE_G4,  8,  REST,     4,
      NOTE_GS4, 4,  NOTE_A4,  8,  NOTE_F5,  4,  NOTE_F5,  8,  NOTE_A4,  2,
      NOTE_D5,  -8, NOTE_A5,  -8, NOTE_A5,  -8, NOTE_A5,  -8, NOTE_G5,  -8,
      NOTE_F5,  -8, NOTE_E5,  8,  NOTE_C5,  4,  NOTE_A4,  8,  NOTE_G4,  2,
      NOTE_E5,  8,  NOTE_C5,  4,  NOTE_G4,  8,  REST,     4,  NOTE_GS4, 4,
      NOTE_A4,  8,  NOTE_F5,  4,  NOTE_F5,  8,  NOTE_A4,  2,  NOTE_B4,  8,
      NOTE_F5,  4,  NOTE_F5,  8,  NOTE_F5,  -8, NOTE_E5,  -8, NOTE_D5,  -8,
      NOTE_C5,  8,  NOTE_E4,  4,  NOTE_E4,  8,  NOTE_C4,  2,  NOTE_C5,  -4,
      NOTE_G4,  -4, NOTE_E4,  4,  NOTE_A4,  -8, NOTE_B4,  -8, NOTE_A4,  -8,
      NOTE_GS4, -8, NOTE_AS4, -8, NOTE_GS4, -8, NOTE_G4,  8,  NOTE_D4,  8,
      NOTE_E4,  -2,
  };
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  playMelody(melody, notes, 200);
}

void nokia() {
  Serial.println("Playing nokia");
  int melody[] = {NOTE_E5,  8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4,
                  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4,  4, NOTE_E4,  4,
                  NOTE_B4,  8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4,  4,
                  NOTE_A4,  2};
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  playMelody(melody, notes, 180);
}

void pacman() {
  Serial.println("Playing pacman");
  int melody[] = {NOTE_B4,  16,  NOTE_B5,  16,  NOTE_FS5, 16, NOTE_DS5, 16,
                  NOTE_B5,  32,  NOTE_FS5, -16, NOTE_DS5, 8,  NOTE_C5,  16,
                  NOTE_C6,  16,  NOTE_G6,  16,  NOTE_E6,  16, NOTE_C6,  32,
                  NOTE_G6,  -16, NOTE_E6,  8,   NOTE_B4,  16, NOTE_B5,  16,
                  NOTE_FS5, 16,  NOTE_DS5, 16,  NOTE_B5,  32, NOTE_FS5, -16,
                  NOTE_DS5, 8,   NOTE_DS5, 32,  NOTE_E5,  32, NOTE_F5,  32,
                  NOTE_F5,  32,  NOTE_FS5, 32,  NOTE_G5,  32, NOTE_G5,  32,
                  NOTE_GS5, 32,  NOTE_A5,  16,  NOTE_B5,  8};
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  playMelody(melody, notes, 105);
}