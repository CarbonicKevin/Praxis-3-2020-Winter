#include "motorLibrary.h"
#include <math.h>

void setup() {
    slave eeSlave = {.addr=1, .dely=1000, .noSteps=-1, .timeMode=1, .dir=0, .stpSize=2};
    slave  zSlave = {.addr=2, .dely=2000, .noSteps=-1, .timeMode=1, .dir=0, .stpSize=1};
    slave dSlave1 = {.addr=3, .dely=2000, .noSteps=-1, .timeMode=1, .dir=0, .stpSize=1};
    slave dSlave2 = {.addr=4, .dely=2000, .noSteps=-1, .timeMode=1, .dir=0, .stpSize=1};
    slave dSlave3 = {.addr=5, .dely=2000, .noSteps=-1, .timeMode=1, .dir=0, .stpSize=1};

    leadMotor eeMot = {.stepAng=1.8, .lead=4, .totSteps=0, .motSlave=eeSlave};
    leadMotor  zMot = {.stepAng=1.8, .lead=4, .totSteps=0, .motSlave=zSlave};
    
    Wire.begin();   // join i2c bus
    Serial.begin(9600);
    Serial.println(1);
    zMot.moveLead(20, 1);
    Serial.println(2);
}

void loop() {
}
