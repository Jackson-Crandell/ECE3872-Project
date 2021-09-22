//-----------------------------------------------------
// Engineer: Rafael Laury
// Based on: Code by Tim Brothers
// Overview
//    When this device is turned on it plays the song Row
//    Row Row Your Boat.
// Design Name:   The Chimp Cymbalist
// File Name:     chimp_cymbalist.c
//
// Inputs: 
//    Volume: controls the volume
//    Tempo: controls the tempo of the song
// Outputs: 
//    Servo: Moves the conductor out of the box
//
// History:       4 January 2020 File created
//				  20 September 2021 Adapted for Chimp Cymbalist
//
//-----------------------------------------------------

#include <Servo.h>
#include <stdio.h>
#include <math.h>

#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255

#define rest 0

#define Octive 4

//Music Notes based on Octive--
#define C 16.3516*pow(2,Octive)
#define D 18.35405*pow(2,Octive)
#define E 20.60172*pow(2,Octive)
#define F 21.82676*pow(2,Octive)
#define G 24.49971*pow(2,Octive)
#define A 27.5*pow(2,Octive)
#define B 30.86771*pow(2,Octive)
#define high_C 32.70320*pow(2,Octive)
#define rest 0


#define speakerPIN 5
#define motorPIN 6


//Row Row Row Your Boat
int songLength = 54;  
int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};
int ten = 90;
int song_tempo = 250;

//setup the servo output
Servo servo_10;

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  
    //set up outputs
  pinMode(speakerPIN, OUTPUT);
  //pinMode(4, OUTPUT);
  //pinMode(7, OUTPUT);
  //pinMode(8, OUTPUT);
    //pinMode(motorPIN, OUTPUT);

  servo_10.attach(10);
  
    //set the motor and servo to zero initial condition.
  digitalWrite(motorPIN, 0);
  servo_10.write(5);

  //debug only
  Serial.begin(9600);
    Serial.print("Setup Complete\n");
}

void toggle() {
  if (servo_10.read() > 0) {
    servo_10.write(0);
  } else {
    servo_10.write(90);
  }
}

void loop()
{
  int duration;                  
  int tempo;
  int tempo_pot;
  int motor_speed;
  static const uint8_t analog_pins[] = {A2,A3,A4,A5};
 
  
    //when the conductor box is open extend the hand
    servo_10.write(0);
  
    //------------------------------------
    //while the box is open play the song
    //------------------------------------
    
    //play the song
    int i_note_index = 0;
    int metronome = 0;
    while (1)
  { 
      //read the tempo pot
      tempo_pot = song_tempo;
      tempo = song_tempo*float(tempo_pot)/TempoCal; //read the tempo POT
      metronome += tempo;
 
      //play the song
      duration = beats[i_note_index] * tempo;
      tone(speakerPIN, notes[i_note_index], duration);
      delay(duration);
        
      //increment the note counter
      ++i_note_index;
      if(i_note_index >= songLength) 
      {
        i_note_index = 0;
        beatSum = 0;
      }
      if(beatSum % 3 == 1) {
        toggle();
        }
      }
  delay(50);
}


    //Set the LEDs





/*
switch (currentState) {
   case ST_IDLE:
       // do something in the idle state
       break;
    case ST_STOP:
       // do something in the stop state
       break;
    // etc...
}
*/