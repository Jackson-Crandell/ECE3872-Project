#include "fix_fft.h"

#define SAMPLES 128

#define AUDIO A0

char im[SAMPLES];
char data[SAMPLES];
int barht[SAMPLES];
int freq[SAMPLES];

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  Serial.println("setup complete");

}

void loop()
{
  static int i, j;
  int val;


  // get audio data
  for (i = 0; i < SAMPLES; i++)
  {
    val = analogRead(AUDIO); // 0-1023
    data[i] = (char)(val / 4 - 128); // store as char
    im[i] = 0; // init all as 0
  }


  // run FFT
  fix_fft(data, im, 7, 0);

  // extract absolute value of data only, for 64 results
  for (i = 0; i < SAMPLES / 2; i++)
  {
    barht[i] = (int)sqrt(data[i] * data[i] + im[i] * im[i]);
  }

  for (i = 0, j = 0; i < SAMPLES / 2; i++, j += 2)
  {
    barht[i] = barht[j] + barht[j + 1];
  }

  // display barchart
  for (int i =0; i < 128; i++)
  {
    if(barht[i] > 20)
    {
      freq[i] = barht[i];
      Serial.println(barht[i]);
    }
  }

  delay(500);
  
}
