
#include <Servo.h>


const int rowPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};    
const int colPins[8] = {A0, A1, A2, A3, A4, A5, 10, 11}; 
Servo servos[6]; 

bool boardState[8][8];
float boardMinX = 100.0, boardMaxX = 300.0; 
float boardMinY = -100.0, boardMaxY = 100.0;
float zRest = 150.0, zPickup = 20.0;

void setup() {
  Serial.begin(9600);
  for(int i=0; i<8; i++) {
    pinMode(rowPins[i], OUTPUT);
    pinMode(colPins[i], INPUT_PULLUP);
    digitalWrite(rowPins[i], HIGH);
  }
  
}

void loop() {
  scanMatrix();
  if (Serial.available() > 0) {
    String move = Serial.readStringUntil('\n');
    executePhysicalMove(move);
    Serial.println("DONE"); 
  }
}

void scanMatrix() {
  for (int r = 0; r < 8; r++) {
    digitalWrite(rowPins[r], LOW);
    for (int c = 0; c < 8; c++) {
      bool reading = !digitalRead(colPins[c]);
      if (reading != boardState[r][c]) {
        delay(50); 
        boardState[r][c] = reading;
        char file = 'a' + c;
        char rank = '1' + r;
        Serial.print("UPDATE:"); Serial.print(file); Serial.print(rank);
        Serial.println(reading ? ":FULL" : ":EMPTY");
      }
    }
    digitalWrite(rowPins[r], HIGH);
  }
}

void executePhysicalMove(String move) {

  String startSq = move.substring(0, 2);
  String endSq = move.substring(2, 4);
  
  float x1, y1, x2, y2;
  getCoords(startSq, x1, y1);
  getCoords(endSq, x2, y2);


  moveArm(x1, y1, zRest);   
  moveArm(x1, y1, zPickup); 
  grip(true);               
  moveArm(x1, y1, zRest);   
  moveArm(x2, y2, zRest);   
  moveArm(x2, y2, zPickup); 
  grip(false);              
  moveArm(x2, y2, zRest);   
}

void getCoords(String sq, float &x, float &y) {
  if(sq == "z9") { x = 0; y = 200; return; } 
  int col = sq[0] - 'a';
  int row = sq[1] - '1';
  x = boardMinX + (col * ((boardMaxX - boardMinX) / 7.0));
  y = boardMinY + (row * ((boardMaxY - boardMinY) / 7.0));
}

void moveArm(float x, float y, float z) {
  
  // incomplete. waiting on robot arm to ship to measure lengths for inverse kinematics
}

void grip(bool close) {
  servos[5].write(close ? 40 : 10);
  delay(500);
}
