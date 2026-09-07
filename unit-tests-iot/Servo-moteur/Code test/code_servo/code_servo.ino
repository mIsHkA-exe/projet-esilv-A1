#include <Servo.h>

Servo servo ;

void setup(){
 Serial.begin(9600);
 servo.attach(9);
}

void loop(){
 Serial.println("Angle : 0");
 servo.write(0);
 delay(1000);
 Serial.println("Angle : 90");
 servo.write(90); 
 delay(1000);
}