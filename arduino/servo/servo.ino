#include <Servo.h>

// Servo defines
Servo servo;  
const int servoPin = 9;
int pos = 0;    // variable to store the servo position

// Serial constants
String serialInput;

void setup() {
  // open serial port, set at 9600 baud
  Serial.begin(9600);
  serialInput = "";
  
  // attaches the servo on pin 9 to the servo object
  servo.attach(servoPin);
}

void loop() {
  if(Serial.available() > 0) {
    char c = Serial.read();
    if(c == '\r') {
      // check if serial command was for servo, then set servo
      if(serialInput.startsWith("set:")) {
        Serial.println("Servo command received!");
        serialInput.replace("Servo:", "");
        int position = getServoPosition(serialInput.toInt());
        servo.write(position);
      }
      serialInput = "";
    }
    else {
      serialInput += c;
    }
  }
}

int getServoPosition(int pos) {
  return pos > 180 ? 180 :
        pos < 0 ? 0 :
        pos;
}

