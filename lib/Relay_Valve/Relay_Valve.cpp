#include "Relay_Valve.h"

Relay_Valve::Relay_Valve(int outputPin){
    pin=outputPin;
    pinMode(pin, OUTPUT);
}
void Relay_Valve::setState(bool state){
    if(state == 1){
        digitalWrite(pin, HIGH);
    }
    else{
        digitalWrite(pin, LOW);
    }
}
void Relay_Valve::timedOpen(int time){
       
}