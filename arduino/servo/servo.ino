#include <Servo.h>

// Servo defines
Servo servo;  
const int servoPin = 9;
int servoPos = 0;    // variable to store the servo position

#define CW 1
#define CCW -1
#define SWEEP_DELAY 15

// Serial constants
String serialInput;
enum SerialCommand {
  SET,
  TURN_CW,
  TURN_CCW
};

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
      // parse serial command
      SerialCommand cmd = getCommand();
      String data = getSerialData();
      
      // set servo command
      if(cmd == SET) {
        setServoPosition(data.toInt());
      }
      // CW sweep command
      else if(cmd == TURN_CW) {
        if(data == "start") {
          sweep(CW);
        }
      }
      else if(cmd == TURN_CCW) {
        if(data == "start") {
          sweep(CCW);
        }
      }
      // clear serial command queue
      serialInput = "";
    }
    else {
      // if command hasn't finished sending, append char string
      serialInput += c;
    }
  }
}

// -------------------------------- //
// Serial Command Parsing Funcitons //
// -------------------------------- //
SerialCommand getCommand() {
  if(serialInput.startsWith("set:")) {
    return SET;
  }
  if(serialInput.startsWith("cw:")) {
    return TURN_CW;
  }
  if(serialInput.startsWith("ccw:")) {
    return TURN_CCW;
  }
}

String getSerialData() {
  return serialInput.substring(serialInput.indexOf(':')+1);
}

// -------------------------------- //
//     Servo Movement Functions     //
// -------------------------------- //

// initializes a loop that continues sweeping the servo until serial command is sent
void sweep(int dir) {
  // loop until stop command is sent
  while(true) {
    // check if stop command has been sent
    if(Serial.available() > 0) {
      char ch = Serial.read();
      if(ch == '\r') break;
    }
    servoPos += dir;
    setServoPosition(servoPos);
    delay(SWEEP_DELAY);
  }
}

// sets servo and updates servo position var
void setServoPosition(int pos) {
  int position = getServoPosition(pos);
  // update global var
  servoPos = position;
  servo.write(position);
}

// caps position between 0 and 180
int getServoPosition(int pos) {
  return pos > 180 ? 180 :
        pos < 0 ? 0 :
        pos;
}

