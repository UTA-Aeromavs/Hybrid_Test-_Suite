#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class servoValve : public Adafruit_PWMServoDriver{
    private:
        const int SERVO_FRQ = 50; //Frequancy of Servo in HZ
        int OPEN[15]; //the time in microseconds for a pulse that opens the Servo actuated valve
        int CLOSED[15]; //the time in microseconds for a pulse closed the Servo actuated valve
        //defaults for default constructor
        int defaultOpen = 1450;
        int defaultClosed = 2450;
    public:
        servoValve(); //Sets pulses corresponding to open and closed states of valve to control servo
        void setChannelValues(int channel_number, int open_pulse, int closed_pulse); //changes default open and closed pulses
        void setValveState(int channel_number, bool state);
        
};
