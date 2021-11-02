#include <stdio.h>
#include <math.h>

#define Octive 2

#define SAMPLES 256             //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be less than 10000 due to ADC

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

int analogPin = A5;       
float analogval = 0;
int scaled_analog;

//Row Row Row Your Boat
int songLength = 54;  
float notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};

unsigned int sampling_period_ms;
unsigned long milliseconds;
//int idx_first_peak,idx_second_peak,tempo;

bool done = false;

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
    Serial.print("\nTempo: " );
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  sampling_period_ms = 50; //Wait every 50 ms to sample. This will have to change based on tempo!!
  
  //debug only
  Serial.print("Setup Complete\n");

}

void loop() {
  Serial.print("Actual Tempo: ");
  Serial.println("150 ms");
  Serial.print("Actual Time Delay: ");
  Serial.println("15");

  // Mode Selection
  // Case 1: Play
  // Case 2: Debug
  // Test Audio Subsystem

  
  // Wait for audio signal to play
  int vReal[SAMPLES];
  int vTime[SAMPLES];
  int temp[5];
  int count = 0;
  

  while(1)
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
    
    count++;
     
  }

  while(!done)
  {
    Serial.println("Starting Tempo Detection");

    analogval = analogRead(analogPin);
    scaled_analog = (int) analogval / 650; 
    //Serial.println(scaled_analog);
    //Serial.println("Starting to sample");
    
    /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        milliseconds = millis();    //Overflows after around 70 minutes!
        analogval = analogRead(analogPin);
        scaled_analog = (int) analogval / 650;
        vReal[i] = scaled_analog;
        vTime[i] = millis();
     
        while(millis() < (milliseconds + sampling_period_ms)){} //Wait for X amount of milliseconds
    }
  
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
        Serial.print(vReal[i]);
        //Serial.print(" ");
        //Serial.print(vTime[i]);
        //Serial.print("\n");
    }
  
    // Find Tempo
    int avg_tempo = findTempo(vReal,vTime);
    Serial.print("\nMeasured Tempo ");
    Serial.println(avg_tempo);
    //Serial.println("\n\n");
  
    Serial.println("Starting Time Delay Detection");
  
    //Find length of notes
    int note_length[16];
    findNotes(vReal,vTime,note_length,avg_tempo);
  
    // Find Time Delay
    int timeDelay = findTimeDelay(note_length);
    if(timeDelay != -1)
    {
      Serial.print("Measured Time delay: ");
      Serial.println(timeDelay);
      done = true;
    } else {
      //TODO: return to top
    }
    
  }
  
  //Serial.print("\nNotes: ");
  /*for(int note : note_length)
  {
    //Serial.print(note);
    //Serial.print(" ");
  }*/

  //Serial.print("\n");
  // Check if notes appear in sequence
  // Maybe check if notes are all the same (i.e. 1's)?
  // TODO: check circular condition


  //TODO: Add condition when doesn't hear anything or doesn't recognize any notes
  //TODO: Calculate time it takes to run all code
  
  while(1); // Run code once

}
