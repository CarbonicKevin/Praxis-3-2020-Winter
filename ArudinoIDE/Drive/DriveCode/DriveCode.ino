#include <SoftwareSerial.h>

#define TX A0
#define RX A1

#define noMot 3

struct vector {
  int deg; // in rad
  int mag;
  int rot;
}; typedef vector vector;

struct stepper {
  const int dirPin;
  const int stepPin;
  float dely;
  int dir;
}; typedef stepper stepper;

struct motor {
  float motVel;
  float motAngle;
  stepper mot;
}; typedef motor motor;


stepper stp1 = {.dirPin=2, .stepPin=3, .dely=0, .dir=0};
stepper stp2 = {.dirPin=4, .stepPin=5, .dely=0, .dir=0};
stepper stp3 = {.dirPin=6, .stepPin=7, .dely=0, .dir=0};

motor mot1 = {.motVel = 0, .motAngle =  (       0.00)*PI , .mot=stp1};
motor mot2 = {.motVel = 0, .motAngle =  (120.0/180.0)*PI , .mot=stp2};
motor mot3 = {.motVel = 0, .motAngle =  (240.0/180.0)*PI , .mot=stp3};
motor motList[] = {mot1, mot2, mot3};

SoftwareSerial iSerial(RX, TX);

const int minDely = 500;
String uInput;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // set up Serial library at 9600 bps
  iSerial.begin(9600);
  Serial.println("Booting");

  digitalWrite(mot1.mot.dirPin, LOW);
  analogWrite (mot1.mot.stepPin, 0);

  digitalWrite(mot2.mot.dirPin, LOW);
  analogWrite (mot2.mot.stepPin, 0);

  digitalWrite(mot3.mot.dirPin, LOW);
  analogWrite (mot3.mot.stepPin, 0);
}

void loop() {
  if (iSerial.available()) { // Parse through input
    uInput = iSerial.readString(); // Read Input
    Serial.println(uInput);
    mov(uInput);
  }
}

void mov(String uInput) {
  vector uVector = {.deg=0, .mag=0, .rot=0};
  
  char *token;
  char buf[200]; char split[] = ",";
  String listStr[3];

  int i;
  
  // Function to parse through the string and package input into vector

  // PARSING --------------------------------------------------------------------------------------------
  // Parsing done with help from: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
  // and https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tochararray/
  // Creates a vector object uVector
  if      (uInput == "x") {uVector.deg =     0; uVector.mag =   0; uVector.rot =  0.0;} 
  else if (uInput == "w") {uVector.deg =   0.0; uVector.mag = 255; uVector.rot =  0.0;} 
  else if (uInput == "s") {uVector.deg = 180.0; uVector.mag = 255; uVector.rot =  0.0;} 
  else if (uInput == "a") {uVector.deg =  90.0; uVector.mag = 255; uVector.rot =  0.0;} 
  else if (uInput == "d") {uVector.deg = 270.0; uVector.mag = 255; uVector.rot =  0.0;} 
  else if (uInput == "q") {uVector.deg =   0.0; uVector.mag =   0; uVector.rot =  1.0;}
  else if (uInput == "e") {uVector.deg =   0.0; uVector.mag =   0; uVector.rot = -1.0;}
  else {
    uInput.toCharArray(buf, 200); // Convert input to char array
    
    token = strtok(buf, split);
    i = 0;
    while (token != NULL) {
      //printf("%s\n", token);
      listStr[i] = String(token);
      token = strtok(NULL, split);
      i++;
    }
    if (i != 3) {
      Serial.println("Failed to parse: " + String(i));
      return;
    }
    
    uVector.deg=listStr[0].toInt(); uVector.mag=listStr[1].toInt(); uVector.rot=listStr[2].toInt(); // creating vector
    Serial.println("Deg: " + listStr[0] + " Mag: " + listStr[1]+ " Rot: " +listStr[2]); // Displaying vector for debug
  }
  runMotVelo(uVector, motList);
} 
    
void runMotVelo(vector V, motor* motP) {
  // Calculates and runs motors
  //Mathmatics from http://robocup.mi.fu-berlin.de/buch/omnidrive.pdf

  float deg = V.deg * PI/180;
  float xVel = V.mag*cos(deg); float yVel = V.mag*sin(deg);
  int i; float maxVel; float normal;
  String outString;

  Serial.println("Velocity: " +  String(xVel) + ", " + String(yVel));

  // Calculate velocity, save the max abs velocity and a list of velocities to work on
  maxVel = 0.0;
  for (i=0; i<noMot; i++) {
    motP[i].motVel = -sin((motP[i]).motAngle) * xVel + cos((motP[i]).motAngle) * yVel + 255*V.rot; // Calculating motor speed
    maxVel = max(abs(motP[i].motVel), abs(maxVel));
    Serial.println("Motor " + String(i) + " Velocity: " + String((motP[i]).motVel)); // Printing calculated motor speeds for debug
  }
  
  // Calculate 'delta' the multiple of the delay required
  if (maxVel == 0) {for (i=0; i<noMot; i++) {motP[i].mot.dely = 0.0; motP[i].mot.dir =0;}}
  else {
    for (i=0; i<noMot; i++) {
      normal = (abs(motP[i].motVel)/maxVel);
      if (normal < 0.0001) {motP[i].mot.dely = 0;}
      else {motP[i].mot.dely = (1/(abs(motP[i].motVel)/maxVel))*minDely;}
      if (motP[i].motVel >= 0) {motP[i].mot.dir = 0;}
      else                     {motP[i].mot.dir = 1;}
    }
  }

  // Package and send to each slave
  for (i=0; i<noMot; i++) {
    Serial.println("Motor: " + String(i) + " Delay: " + String(motP[i].mot.dely) + " Direction: " + String(motP[i].mot.dir));
    digitalWrite(motP[i].mot.dirPin, motP[i].mot.dir);
    analogWrite (motP[i].mot.stepPin, (motP[i].mot.dely/1000)*255);
  }
}
