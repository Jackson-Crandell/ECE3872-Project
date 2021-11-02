int analogPin = A5;       
float analogval = 0;
int digitalPin = 4;
int digitalval = 0;
int digitalLED = 7;
int threshold = 300;
int scaled_analog; 

void setup() {
  pinMode(digitalPin, INPUT);
  pinMode(digitalLED, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
}

void loop() {
  analogval = analogRead(analogPin);
  scaled_analog = (int) analogval / 600; //* (5.0 / 1023.0) - .8;
  if (scaled_analog == 1)
  {
    digitalWrite(digitalLED, 1);
  }
  else
  {
    digitalWrite(digitalLED, 0);
  }
  //digitalval = digitalRead(digitalPin);
  //digitalWrite(digitalLED, digitalval);
  //Serial.println(digitalval);
  Serial.println(scaled_analog); 

}
