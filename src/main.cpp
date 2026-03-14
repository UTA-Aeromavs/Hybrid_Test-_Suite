#include <Arduino.h>
#include "HybridScale.h"
#include "Pressure_Transducer.h"
#include "servoValve.h"
#include "Relay_Valve.h"

// put function declarations here:
Relay_Valve relayOutput(1);
Pressure_Transducer transducer1(A0);
Pressure_Transducer transducer2(A1);
int userInput = 0;

void setup() {
  relayOutput.setState(0);
}

void loop() {
  //relay test
  while(Serial.available() == 0){}
  userInput = Serial.parseInt();
  relayOutput.setState(userInput);

  transducer1.update();
  transducer1.getVoltage();

  transducer2.update();
  transducer2.getVoltage();

}