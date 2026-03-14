#pragma once

#include <Arduino.h>

class Relay_Valve{
    private:
        int pin; //Takes Arduino Pin for relay use
    public:
        Relay_Valve(int outputPin);
        void setState(bool state); //0 is off, 1 is on
        void timedOpen(int time);


};
