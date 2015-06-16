#include "Arduino.h"
#include "pitches.h"
int speakerPin = 8;

int length = 15; // the number of notes
char notes[] = "ccggaagffeeddc "; // a space represents a rest
int beats[] = { 
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
  
// Array with the notes in the melody (see pitches.h for reference)
int melody[] = {NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4};

// Array with the note durations: a quarter note has a duration of 4, half note 2 etc.
int durations[]  = {4, 4, 4, 5, 16, 4, 5, 16, 2, 4, 4, 4, 5, 16, 4, 5, 16, 2};
  
int tempo = 100;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'   };
  int tones[] = { 
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956   };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playTune(int notes[], int durations[], int BPM) 
{
  int tuneSize = sizeof(melody) / sizeof(int);
  
  // iterate over the notes of the tune:
  for (int thisNote = 0; thisNote < tuneSize; thisNote++) {

    // For details on calculating the note duration using the tempo and the note type,
    // see http://bradthemad.org/guitar/tempo_explanation.php.
    // A quarter note at 60 BPM lasts exactly one second and at 120 BPM - half a second.
    
    int noteDuration = (int)((1000 * (60 * 4 / BPM)) / durations[thisNote] + 0.);
    tone(speakerPin, notes[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 20% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.20;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speakerPin);
  }
}

void playMelody() {
  // for (int i = 0; i < length; i++) {
    // if (notes[i] == ' ') {
      // delay(beats[i] * tempo); // rest
    // } 
    // else {
      // playNote(notes[i], beats[i] * tempo);
    // }
    // delay(tempo / 2); 
  // }
  playTune(melody, durations, tempo);
}



