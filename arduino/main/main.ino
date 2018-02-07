#include <Servo.h>

// Servo defines
Servo servo;  
const int servoPin = 9;
int servoPos = 0;    // variable to store the servo position

const int switchPin = 7;

#define CW 1
#define CCW -1
#define SWEEP_DELAY 10

#define CW_CMD "cw:"
#define CCW_CMD "ccw:"
#define SET_CMD "s:"
#define AS_CMD "as:"

// Serial constants
String serialInput;
enum SerialCommand {
  AUTO_SORT,
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

  pinMode(LED_BUILTIN, OUTPUT);
  
  pinMode(switchPin, INPUT); 
  digitalWrite(switchPin, LOW);
}

void loop() {
  if(Serial.available() > 0) {
    char c = Serial.read();
    if(c == '\r') {
      // parse serial command
      SerialCommand cmd = getCommand();
      String data = getSerialData();

      if(cmd == AUTO_SORT) {
        if(data == "1") {
          autoSort();  
        }
      }
      // set servo command
      else if(cmd == SET) {
        setServoPosition(data.toInt());
      }
      // CW sweep command
      else if(cmd == TURN_CW) {
        if(data == "1") {
          sweep(CW);
        }
      }
      else if(cmd == TURN_CCW) {
        if(data == "1") {
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
  if(serialInput.startsWith(AS_CMD)) {
    return AUTO_SORT;
  }
  else if(serialInput.startsWith(SET_CMD)) {
    return SET;
  }
  else if(serialInput.startsWith(CW_CMD)) {
    return TURN_CW;
  }
  else if(serialInput.startsWith(CCW_CMD)) {
    return TURN_CCW;
  }
}

String getSerialData() {
  return serialInput.substring(serialInput.indexOf(':')+1);
}

// -------------------------------- //
//     Servo Movement Functions     //
// -------------------------------- //
void autoSort() {
  // turn on conveyor
  digitalWrite(LED_BUILTIN, HIGH);
  // busy wait until limit switch is hit
  delay(1500);
//  while(true) {
//    //Serial.println(digitalRead(switchPin));
//    if(digitalRead(switchPin) == 1) {
//      break;
//    }
//  }
  // turn off conveyor
  digitalWrite(LED_BUILTIN, LOW);
  // tell DE1-SoC that an object is ready
  Serial.write("ls\n");
}

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

