#include <Servo.h>

#define CW 1
#define CCW -1
#define SWEEP_DELAY 10
#define CONVEYOR_SPD 85
#define DEBOUNCE_DELAY 35
#define TIMEOUT 5000

#define CW_CMD "cw:"
#define CCW_CMD "ccw:"
#define SET_CMD "s:"
#define AS_CMD "as:"
#define CV_CMD "cv:"

#define FORWARD 1
#define BACKWARD 0

int laserPin = 12;
int photoPin = 5;

#define LASER_BREAK 800

// Servo defines
Servo servo;  
int servoPin = 3;
// variable to store the servo position
int servoPos = 0;

// Motor defines
int E1 = 5;  
int M1 = 4;

// Serial constants
enum SerialCommand {
  AUTO_SORT,
  SET,
  TURN_CW,
  TURN_CCW,
  CONVEYOR
};

String mainBuffer,
       sortBuffer;

void setup() {
  // open serial port, set at 9600 baud
  Serial.begin(9600);
  mainBuffer = "";
  sortBuffer = "";
  
  // attaches the servo on pin 9 to the servo object
  servo.attach(servoPin);

  // set up motor output
  pinMode(M1, OUTPUT); 

  // set up limit switch
  pinMode(laserPin, OUTPUT); 
}

void loop() {
  if(Serial.available() > 0) {
    char c = Serial.read();
    if(c == '\r') {
      // parse serial command
      SerialCommand cmd = getCommand(mainBuffer);
      String data = getSerialData(mainBuffer);

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
      else if(cmd == CONVEYOR) {
        if(data == "1") {
          conveyor(FORWARD, CONVEYOR_SPD);
        }
        else if(data == "0") {
          conveyor(FORWARD, 0);
        }
      }
      // clear serial command queue
      mainBuffer = "";
    }
    else {
      // if command hasn't finished sending, append char string
      mainBuffer += c;
    }
  }
}

// -------------------------------- //
//         Movement Functions       //
// -------------------------------- //
void autoSort() {
  // allow laser to turn on
  digitalWrite(laserPin, HIGH);
  delay(300);
  
  // turn on conveyor
  conveyor(FORWARD, CONVEYOR_SPD);

  // debouncing variables
  unsigned long lastLaserTime = 0;
  int currLaserState, lastLaserState = (analogRead(photoPin) < LASER_BREAK);
  unsigned long timeElapsed = millis();
  while(1) {
    // laser debounce logic
    int photoReading = (analogRead(photoPin) < LASER_BREAK);
    if(photoReading != lastLaserState) {
      lastLaserTime = millis();
    }
    if(millis() - lastLaserTime > DEBOUNCE_DELAY) {
      if(photoReading != currLaserState) {
        currLaserState = (analogRead(laserPin) < LASER_BREAK);
        if(currLaserState) {
          digitalWrite(laserPin, LOW);
          break;
        }
      }
    }
    lastLaserState = photoReading;

    // check if conveyor has been running for more than TIMEOUT seconds
    if(millis() - timeElapsed > TIMEOUT) {
      conveyor(FORWARD, 0);
      digitalWrite(laserPin, LOW);
      Serial.write("dn\n");
      sortBuffer = "";
      return;
    }

    // check if DE1-SoC has sent a STOP command
    if(Serial.available() > 0) {
      char c = Serial.read();
      if(c == '\r') {
        // parse serial command
        if(getCommand(sortBuffer) == CONVEYOR && getSerialData(sortBuffer) == "0") {
          conveyor(FORWARD, 0);
          digitalWrite(laserPin, LOW);
          sortBuffer = "";
          return;
        }
        else {
          sortBuffer = "";
        }
      }
      sortBuffer += c;
    }
  }
  
  conveyor(FORWARD, 0);
  // tell DE1-SoC that an object is ready
  Serial.write("ls\n");
}

void conveyor(int dir, int spd) {
  digitalWrite(M1, dir);
  analogWrite(E1, spd);   //PWM Speed Control
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

// -------------------------------- //
// Serial Command Parsing Funcitons //
// -------------------------------- //
SerialCommand getCommand(String input) {
  if(input.startsWith(AS_CMD)) {
    return AUTO_SORT;
  }
  else if(input.startsWith(SET_CMD)) {
    return SET;
  }
  else if(input.startsWith(CW_CMD)) {
    return TURN_CW;
  }
  else if(input.startsWith(CCW_CMD)) {
    return TURN_CCW;
  }
  else if(input.startsWith(CV_CMD)) {
    return CONVEYOR;
  }
}

String getSerialData(String input) {
  return input.substring(input.indexOf(':')+1);
}

