#include "servo_control.h"

Servo myservo;
int pos = 180;

void setupServo() {
  myservo.attach(13);
  delay(10);
}

void unlock() {
  for (int pos = 90; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(10);
  }
  delay(5000);
  for (int pos = 180; pos >= 90; pos -= 1) {
    myservo.write(pos);
    delay(10);
  }
  Serial.println("Door unlocked");
}