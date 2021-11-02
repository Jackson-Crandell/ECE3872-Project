#include <Servo.h>


Servo servo1;
Servo servo2;

void setup() {
  servo1.attach(10);
  servo2.attach(11);
  
}
void loop() {
  servo1.write(122);
  servo2.write(58);
  delay(500);
  servo1.write(80);
  servo2.write(110);
  delay(500);


  while(1);
}
