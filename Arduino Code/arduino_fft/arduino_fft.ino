#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be less than 10000 due to ADC
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;

int analogPin = A0;       
float analogval = 0;
int digitalPin = 4;
int digitalval = 0;
int digitalLED = 7;
int threshold = 300;
int scaled_analog;
 
unsigned int sampling_period_ms;
unsigned long milliseconds;
double vReal[SAMPLES];
double vImag[SAMPLES];
 
void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Array\n");
    //sampling_period_ms = round(1000000*(1.0/SAMPLING_FREQUENCY));
    sampling_period_ms = 50; //Wait every 50 ms to sample
    
    //debug only
    Serial.print("Setup Complete\n");
}
 
void loop() {
   
    /*SAMPLING*/
    for(int i=0; i<SAMPLES; i++)
    {
        milliseconds = millis();    //Overflows after around 70 minutes!
        analogval = analogRead(analogPin);
        scaled_analog = (int) analogval * (5.0 / 1023.0) - .79;
        vReal[i] = analogval;
        vImag[i] = 0;
     
        while(millis() < (milliseconds + sampling_period_ms)){} //Wait for X amount of milliseconds
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
 
    /*PRINT RESULTS*/
    Serial.println(peak);     //Print out what frequency is the most dominant.
 
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
         
        //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }
 
    //delay(1000);  //Repeat the process every second OR:
    while(1);       //Run code once
}
