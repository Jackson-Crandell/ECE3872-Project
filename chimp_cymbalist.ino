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
//          20 September 2021 Adapted for Chimp Cymbalist
//
//-----------------------------------------------------

#include <Servo.h>
#include <stdio.h>
#include <math.h>

#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255

#define rest 0

int Octave = 6;

//Music Notes--
float C = 16.3516;
float D = 18.35405;
float E = 20.60172;
float F = 21.82676;
float G = 24.49971;
float A = 27.5;
float B = 30.86771;
float high_C = 32.70320;
#define rest 0


#define speakerPIN 9

//IO definitions
#define octaveUp A1 //active low
#define octaveDown A0 //active low
#define tempoKnob A2
#define resetButton A3 //active low
#define modeButton A5 //active low
#define startButton A4 //active low
#define PLAY_LED 2 
#define TEST_LED 3
#define MANUAL_LED 4
#define display_A 8
#define display_B 5
#define display_C 6
#define display_D 7


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
  //inputs
  pinMode(octaveUp, INPUT);
  pinMode(octaveDown, INPUT);
  pinMode(tempoKnob, INPUT);
  pinMode(resetButton, INPUT);
  pinMode(modeButton, INPUT);
  pinMode(startButton, INPUT);
  
    //set up outputs
  pinMode(speakerPIN, OUTPUT);
  pinMode(PLAY_LED, OUTPUT);
  pinMode(TEST_LED, OUTPUT);
  pinMode(MANUAL_LED, OUTPUT);
  pinMode(display_A, OUTPUT);
  pinMode(display_B, OUTPUT);
  pinMode(display_C, OUTPUT);
  pinMode(display_D, OUTPUT);
  //pinMode(4, OUTPUT);
  //pinMode(7, OUTPUT);
  //pinMode(8, OUTPUT);
    //pinMode(motorPIN, OUTPUT);

  //servo 1 on pin 10
  servo_10.attach(10);
  
   //set the motor and servo to zero initial condition.
  
  servo_10.write(0);
  

  digitalWrite(PLAY_LED, HIGH);
  digitalWrite(MANUAL_LED, LOW);
  digitalWrite(TEST_LED, LOW);

  
  displayNum(Octave);
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

void LED_update() {
  if (digitalRead(PLAY_LED) == HIGH) {
    digitalWrite(PLAY_LED,LOW);
    digitalWrite(MANUAL_LED,HIGH);
    digitalWrite(TEST_LED,LOW);
  } else if (digitalRead(MANUAL_LED) == HIGH) {
    digitalWrite(PLAY_LED,LOW);
    digitalWrite(MANUAL_LED,LOW);
    digitalWrite(TEST_LED,HIGH);
  } else if (digitalRead(TEST_LED) == HIGH) {
    digitalWrite(PLAY_LED,HIGH);
    digitalWrite(MANUAL_LED,LOW);
    digitalWrite(TEST_LED,LOW);
  }
}


void displayNum(int number) {
 //number must be greater than or equal to 0 and less than or equal to 9
 
  switch (number) {
    case 0:
      digitalWrite(display_A, LOW);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, LOW);
      break;
    case 1:
      digitalWrite(display_A, HIGH);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, LOW);
      break;
    case 2:
      digitalWrite(display_A, LOW);
      digitalWrite(display_B, HIGH);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, LOW);
      break;
    case 3:
      digitalWrite(display_A, HIGH);
      digitalWrite(display_B, HIGH);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, LOW);
      break;
    case 4:
      digitalWrite(display_A, LOW);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, HIGH);
      digitalWrite(display_D, LOW);
      break;
    case 5:
      digitalWrite(display_A, HIGH);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, HIGH);
      digitalWrite(display_D, LOW);
      break;
    case 6:
      digitalWrite(display_A, LOW);
      digitalWrite(display_B, HIGH);
      digitalWrite(display_C, HIGH);
      digitalWrite(display_D, LOW);
      break;
    case 7:
      digitalWrite(display_A, HIGH);
      digitalWrite(display_B, HIGH);
      digitalWrite(display_C, HIGH);
      digitalWrite(display_D, LOW);
      break;
    case 8:
      digitalWrite(display_A, LOW);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, HIGH);
      break;
    case 9:
      digitalWrite(display_A, HIGH);
      digitalWrite(display_B, LOW);
      digitalWrite(display_C, LOW);
      digitalWrite(display_D, HIGH);
      break;
  }
}

void loop()
{
  //MOTOR TEST
  /*while(1) {
    servo_10.write(0);
    delay(100);
    servo_10.write(110);
    delay(100);
  }
  */
  if ((digitalRead(octaveUp) == LOW) && (digitalRead(octaveDown) == LOW)) {
    //do nothing
  } else if (digitalRead(octaveUp) == LOW) {
    if (Octave < 9) {
      Octave += 1;
      displayNum(Octave);
    }
  } else if (digitalRead(octaveDown) == LOW) {
    if (Octave > 0) {
      Octave -= 1;
      displayNum(Octave);
    }
  }


  if (digitalRead(modeButton) == LOW) {
    LED_update();
  }
  int duration;                  
  int tempo;
  int tempo_pot; 
  
    //when the conductor box is open extend the hand
  //servo_10.write(0);
  
    //------------------------------------
    //while the box is open play the song
    //------------------------------------
    
    //play the song
    int i_note_index = 0;
    int metronome = 0; 
    while (digitalRead(startButton) == LOW)
  { 
      //read the tempo pot
      tempo_pot = analogRead(tempoKnob);
      Serial.print(String(tempo_pot) + "\n");
      tempo = song_tempo*float(tempo_pot)/TempoCal; //read the tempo POT
      Serial.print(String(tempo) + "\n\n");
      metronome += tempo;
 
      //play the song
      duration = beats[i_note_index] * tempo;
      tone(speakerPIN, notes[i_note_index]*pow(2,Octave), duration);
      delay(duration);
        
      //increment the note counter
      ++i_note_index;
      if(i_note_index >= songLength) 
      {
        i_note_index = 0;
        //beatSum = 0;
      }
      /*
      if(beatSum % 3 == 1) {
        toggle();
        }
      }
      */
}


    //Set the LEDs

}


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
