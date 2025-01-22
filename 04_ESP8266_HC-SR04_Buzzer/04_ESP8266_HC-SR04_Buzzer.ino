/*
  AA Digital Ecologies HC-SR04 Ultrasound Ranger and Piezo Buzzer
*/

#include "pitches.h"

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

const int trigPin = D5;
const int echoPin = D6;

const int BUZZER_PIN = D8; // the ESP8266 pin connected to piezo buzzer

long duration;
float distanceCm;

// notes in the initialy melody
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// notes (frequencies in Hz)
const int notes[] = {
  NOTE_C4, // C4
  NOTE_D4, // D4
  NOTE_E4, // E4
  NOTE_F4, // F4
  NOTE_G4, // G4
  NOTE_A4, // A4
  NOTE_B4, // B4
  NOTE_C5, // C5
  NOTE_D5, // D5
  NOTE_E5, // E5
  NOTE_F5, // F5
  NOTE_G5, // G5
  NOTE_A5, // A5
  NOTE_B5, // B5
  NOTE_C6  // C6
};
const int numNotes = sizeof(notes) / sizeof(notes[0]);

void setup() {
  Serial.begin(115200); // starts the serial communication
  pinMode(trigPin, OUTPUT); // sets the trigPin as an Output
  pinMode(echoPin, INPUT); // sets the echoPin as an Input

  // iterate over the notes of the initial melody
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type
    // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them
    // the note's duration + 30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER_PIN);
  }

}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // prints the distance on the Serial Monitor
  Serial.println(distanceCm);

  int noteIndex = map(distanceCm, 15, 99, 0, numNotes - 1); // map distance to note index
  noteIndex = constrain(noteIndex, 0, numNotes - 1); //eensure the note index is within bounds
  int note = notes[noteIndex];
  
  if ((distanceCm <= 100) && (distanceCm > 15)) {
    tone(BUZZER_PIN, note); // play the note continuously
  } else {
    noTone(BUZZER_PIN);
  }
  
  delay(20);
}



