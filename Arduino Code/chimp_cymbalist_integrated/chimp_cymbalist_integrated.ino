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


#define SAMPLES 256             //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be less than 10000 due to ADC

//Music Notes based on Octave--
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
#define modeButton 12 //active low
#define startButton A4 //active low
#define analogPin A5 //active low
#define PLAY_LED 2 
#define TEST_LED 3
#define MANUAL_LED 4
#define display_A 8
#define display_B 5
#define display_C 6
#define display_D 7

float analogval = 0;
int scaled_analog;
int Octave = 4;

//Row Row Row Your Boat
int songLength = 54;  
float notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};
int song_tempo = 250;

//setup the servo output
Servo leftServo;
Servo rightServo;

unsigned int sampling_period_ms;
unsigned long milliseconds;

// Method to find tempo
int findTempo(int (&vReal)[SAMPLES], int (&vTime)[SAMPLES])
{
  int tempo = 0;
  int t = 0;
  int idx_first_peak, idx_second_peak = 0;
  while(t < 5)
  {
    //Find first,last one of note
    for(int i = idx_second_peak; i<(SAMPLES);i++)
    {
      if(vReal[i] == 1 || vReal[i] == 2)
      {
        while(vReal[i] != 0 && i != 256) // TODO: Add error condition for over 256?
        {
          //Serial.println("Trying to find last 1");
          //Serial.println(vReal[i]);
          i++; 
        }
        //Serial.print("Found 1st index: ");
        idx_first_peak = i - 1;
        //Serial.println(idx_first_peak);
        //Serial.println(vReal[i-1]);
        //Serial.println(vReal[i]);
        break;
      }
    }
  
    for(int i = idx_first_peak +1; i< SAMPLES - idx_first_peak - 1;i++)
    {
      if(vReal[i] == 1 || vReal[i] == 2)
      {
        //Serial.print("Found 2nd index: ");
        idx_second_peak = i;
        //Serial.println(idx_second_peak);
        //Serial.println(vReal[i-1]);
        //Serial.println(vReal[i]);
        break;
      }
    }
    Serial.print("Tempo: " );
    Serial.println(vTime[idx_second_peak-1]-vTime[idx_first_peak]);
    tempo += vTime[idx_second_peak-1]-vTime[idx_first_peak];
    t++;
  }
  // TODO: Handle when we get an outlier
  return tempo/5;
}

// Method to find Note lengths
void findNotes(int (&vReal)[SAMPLES], int (&vTime)[SAMPLES], int (&note_length)[16],int avg_tempo)
{
  //Find length of notes
  int idx_first_peak = 0;
  int idx_second_peak = 0;
  int t = 0;
  while(t <= 16)
  {
    bool found = false;
    //Find first,last one of note
    for(int i = idx_second_peak; i<(SAMPLES);i++)
    {
      if(vReal[i-1] == 0 && !found && (vReal[i] == 1 || vReal[i] == 2))
      {
        //Serial.print("\nFound 1st index: ");
        idx_first_peak = i;
        //Serial.println(idx_first_peak);
        //Serial.println(vReal[i-1]);
        //Serial.println(vReal[i]);
        while(vReal[i] != 0 && i != 256) // TODO: Add error condition for over 256?
        {
          //Serial.println("Trying to find last 1");
          //Serial.println(vReal[i]);
          i++; 
        }
        //Serial.print("Found 2nd index: ");
        idx_second_peak = i;
        //Serial.println(idx_second_peak);
        //Serial.println(vReal[i-1]);
        //Serial.println(vReal[i]);

        note_length[t] = (vTime[idx_second_peak]-vTime[idx_first_peak]) / avg_tempo;
        t++;
        note_length[t] = 1; // Rest note
        t++;
        found = true;
      }
    }
    
  }
  
}

// Method to see if sequence of notes is within beats
// TODO: Nearest Note
int findTimeDelay(int (&note_length)[16])
{
  int arr_length = sizeof(beats)/ sizeof(beats[0]);
  int time_delay_idx = 0;
  for(int i = 0; i < arr_length - 3; i++)
  {
    //Serial.print(beats[i]);
    //Serial.print(" ");
    if(beats[i] == note_length[0] && beats[i + 1] == note_length[1] && beats[i + 2] == note_length[2] && beats[i + 3] == note_length[3] && beats[i + 4] == note_length[4]
      && beats[i + 5] == note_length[5] && beats[i + 6] == note_length[6] && beats[i + 7] == note_length[7] && beats[i + 8] == note_length[8])
    {
      //Serial.print("Index: ");
      time_delay_idx = i + 15; // or 7?
      return time_delay_idx;
      //Serial.println(i+15);
    } 
  }
  Serial.println("No matching sequence");
  return -1;
}

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
  leftServo.attach(10);
  rightServo.attach(11);

  leftServo.write(122);
  rightServo.write(58);

  sampling_period_ms = 50; //Wait every 50 ms to sample. TODO: Fix this?
  

  digitalWrite(PLAY_LED, HIGH);
  digitalWrite(MANUAL_LED, LOW);
  digitalWrite(TEST_LED, LOW);

  
  displayNum(Octave);
  //debug only
  Serial.begin(9600);
  Serial.print("Setup Complete\n");
}

void toggle() {
  if (leftServo.read() == 122) {
    leftServo.write(80);
  } else {
    leftServo.write(122);
  }

  if (rightServo.read() == 58) {
    rightServo.write(110);
  } else {
    rightServo.write(58);
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

void play_song(int tempo, int timeDelay) {
  int duration;                  
  //play the song
  int i_note_index = timeDelay;
  //int metronome = 0;
  bool reset = false; 
  int beatSum = 0;
  while (!reset)
  { 
      Serial.print(String(tempo) + "\n\n");
 
      //play the song
      duration = beats[i_note_index] * tempo;
      tone(speakerPIN, notes[i_note_index]*pow(2,Octave), duration);
      delay(duration);
      beatSum += beats[i_note_index];
        
      //increment the note counter
      i_note_index++;
      if(i_note_index >= songLength) 
      {
        i_note_index = 0;
        beatSum = 0;
      }

      // Reset
      if(digitalRead(resetButton) == LOW) 
      {
        Serial.println("Reset");
        reset = true;
      }
      
      // TODO: Incorporate motors
      // Toggle Motors
      if(beatSum % 3 == 1) {
        toggle();
      } 
      
  }
}

void loop()
{

  // 7 Segment Display
  if ((digitalRead(octaveUp) == LOW) && (digitalRead(octaveDown) == LOW)) {
    //do nothing
  } else if (digitalRead(octaveUp) == LOW) {
    if (Octave < 9) {
      Serial.print(Octave);
      Octave += 1;
      displayNum(Octave);
    }
  } else if (digitalRead(octaveDown) == LOW) {
    if (Octave > 0) {
      Serial.print(Octave);
      Octave -= 1;
      displayNum(Octave);
    }
  }


  if (digitalRead(modeButton) == LOW) {
    LED_update();
  }


  // Mode selection
  if (digitalRead(startButton) == LOW) 
  {
    bool done = false;
    int timeDelay = 0;
    int avg_tempo = 0;
    
    // Play Mode
    if(digitalRead(PLAY_LED) == HIGH)
    {
        //Serial.print("Play Mode");
        
        // TODO: Incorporate reset pin
        // Check if speaker or mic is connected
        // Wait for audio signal to play
        bool reset = false;
        int vReal[SAMPLES];
        int vTime[SAMPLES];
        int temp[5];
        int count = 0;

        // Wait for notes to be played
        while(!reset)
        {
          if(count == 5)
          {
            Serial.println("Listening");
            count = 0; // Reset loop if it doesn't break
          }
          analogval = analogRead(analogPin);
          temp[count] = (int) analogval / 650;
          int temp_count = 0;
          for(int i = 0; i < sizeof(temp)/temp[0]; i++)
          {
            if(temp[i] > 0) 
            {
              temp_count++;
            }
          }
          if(temp_count > 3) //means notes are being played
          {
            break;
          }

          // Reset to mode selection
          if(digitalRead(resetButton) == LOW) 
          {
            Serial.println("Reset");
            reset = true;
          }
           
          
          count++;
           
        }
      
        while(!done && !reset)
        {
          Serial.println("Starting Tempo Detection");
      
          analogval = analogRead(analogPin);
          scaled_analog = (int) analogval / 650; 
          //Serial.println(scaled_analog);
          //Serial.println("Starting to sample");
          
          //SAMPLING 
          for(int i=0; i<SAMPLES; i++)
          {
              milliseconds = millis();    //Overflows after around 70 minutes!
              analogval = analogRead(analogPin);
              scaled_analog = (int) analogval / 650;
              vReal[i] = scaled_analog;
              vTime[i] = millis();
              
              // TODO: Reset to mode selection
              if(digitalRead(resetButton) == LOW) 
              {
                Serial.println("Reset");
                reset = true;
              }
           
              while(millis() < (milliseconds + sampling_period_ms)){} //Wait for X amount of milliseconds
          }
        
          for(int i=0; i<(SAMPLES/2); i++)
          {
              //View all these three lines in serial terminal to see which frequencies has which amplitudes
              //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
              //Serial.print(" ");
              //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
              Serial.print(vReal[i]);
              //Serial.print(" ");
              //Serial.print(vTime[i]);
              //Serial.print("\n");
          }
          Serial.print("\n" );

          // TODO: Reset to mode selection
          if(digitalRead(resetButton) == LOW) 
          {
            Serial.println("Reset");
            reset = true;
          }
        
          // Find Tempo
          avg_tempo = findTempo(vReal,vTime);
          Serial.print("\nMeasured Tempo ");
          Serial.println(avg_tempo);
          //Serial.println("\n\n");
        
          Serial.println("Starting Time Delay Detection");
        
          //Find length of notes
          int note_length[16];
          findNotes(vReal,vTime,note_length,avg_tempo);
        
          // Find Time Delay
          timeDelay = findTimeDelay(note_length);
          if(timeDelay != -1)
          {
            Serial.print("Measured Time delay: ");
            Serial.println(timeDelay);
            done = true;
          } else {
            //TODO: return to top
          }
          
        }

        // Play song
        play_song(avg_tempo,timeDelay);
    }
       
    // Debug Mode
    else if (digitalRead(TEST_LED) == HIGH)
    {
      Serial.print("Debug Mode");

      // Test #1 Audio Subsystem
      // Description: Play each note on the speakers
      // Default: Set Octave to 4
      float DebugNotes[] = {C,rest,D,rest,E,rest,F,rest,G,rest,A,rest,B,rest,high_C};
      int i = 0;
      while(i < 15)
      {
        tone(speakerPIN, DebugNotes[i]*pow(2,4), 300);
        delay(300);
        ++i;
      }
      

      // Test #2 User Interface
      // Description: Flash each LED and go through every number on seven segment display
      for(int i = 0; i < 10; i++)
      {
        displayNum(i);
        delay(300);
      }

      // Flash LEDs
      for(int i = 0; i < 9; i++)
      {
        LED_update();
        delay(300);
      }


      // Test #3 Motors
      // Description: Clap those hands
      toggle();
      delay(300);
      toggle();
      
    }
    // Manual Mode
    else if (digitalRead(MANUAL_LED) == HIGH)
    {
      Serial.print("Manual Mode");
      int duration;                  
      //play the song
      int i_note_index = 0;
      bool reset = false;
      int tempo;
      int tempo_pot;  
      int beatSum = 0;
      while (!reset)
      { 
          //read the tempo pot
          tempo_pot = analogRead(tempoKnob);
          Serial.print(String(tempo_pot) + "\n");
          tempo = song_tempo*float(tempo_pot)/TempoCal; //read the tempo POT
          Serial.print(String(tempo) + "\n\n");
     
          //play the song
          duration = beats[i_note_index] * tempo;
          tone(speakerPIN, notes[i_note_index]*pow(2,Octave), duration);
          delay(duration);
          beatSum += beats[i_note_index];
            
          //increment the note counter
          i_note_index++;
          if(i_note_index >= songLength) 
          {
            i_note_index = 0;
            beatSum = 0;
          }
    
          // Reset
          if(digitalRead(resetButton) == LOW) 
          {
            Serial.println("Reset");
            reset = true;
          }

          
          // TODO: Incorporate motors
          if(beatSum % 2 == 1) {
            toggle();
          }
      }
      
    }
  }

  // Set pushbuttons
  delay(150);
}
