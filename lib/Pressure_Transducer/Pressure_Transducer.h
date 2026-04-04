#pragma once

#include <Arduino.h>

class Pressure_Transducer{
    private:
        const float V_REF = 1.5;     // Analog reference voltage (e.g., 5V or 3.3V)
        const float R_BITS = 12.0;   // ADC resolution (bits)
        const float ADC_STEPS = (1 << int(R_BITS)) - 1; // Number of steps (2^R_BITS - 1)
        const float offset = 0;
        double Voltage = 0; //Voltage with compensation for nonlinearity
        double RawVoltage = 0; //Raw voltage reading
        double Pressure = 0;
        int ADC_Value = 0; //Reads ADC Value
        int pin; //Takes Arduino Pin for use as ADC
        int voltGain = 1; //Value used to adjust RawVoltage for Voltage Output
        int pressureScale = 1; //scale fact for voltage into pressure
    public:
        Pressure_Transducer(int inputPin);
        unsigned long update();
        int getRawValue();
        double getRawVoltage();
        double getVoltage();
        double getPressure();
};