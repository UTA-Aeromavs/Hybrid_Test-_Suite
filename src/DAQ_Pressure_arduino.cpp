#include <Arduino.h>
const float resistorValue = 100; // Change value of resistor to the actual resistance value
const float minimumVoltage =  resistorValue * 0.1; //Change value for current
const float maximumVoltage=  resistorValue * 1; //Change value for current
const float maxPressure = 10000;

void setup() {
  Serial.begin(115200);    
  analogReadResolution(14);
}

void loop() {
    int value = analogRead(A0); // CHoose pin
    float voltage = (value * 5.0) / 16383.0;
    float pressure = 0;
    if (voltage >= minimumVoltage) {
        pressure = ((voltage - minimumVoltage) / (maximumVoltage - minimumVoltage)) * maxPressure;
    }

    Serial.print("Pressure: ");
    Serial.println(pressure);

    delay(10); //Change delay time
}
