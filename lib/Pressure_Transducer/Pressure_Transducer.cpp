#include "Pressure_Transducer.h"

Pressure_Transducer::Pressure_Transducer(int inputPin){
    pin = inputPin;
    pinMode(inputPin, INPUT);
    analogReference(AR_INTERNAL); //Note Max input is 1.5V
    analogReadResolution(14);
}
unsigned long Pressure_Transducer::update(){
    ADC_Value = analogRead(pin);
    return micros();
}
int Pressure_Transducer::getRawValue(){
    return ADC_Value;
}
double Pressure_Transducer::getRawVoltage(){
    RawVoltage = ((ADC_Value/ADC_STEPS)*V_REF);
    return RawVoltage;
}
double Pressure_Transducer::getVoltage(){
    RawVoltage = ((ADC_Value/ADC_STEPS)*V_REF);
    //Adjusts voltGain based on known values to adjust for nonlinearity
    if(RawVoltage >= 1.481){
        voltGain = .961;
    }
    else if(RawVoltage >= 1.364){
        voltGain = .959;
    }
    else if(RawVoltage >= 1.247){
        voltGain = .956;
    }
    else if(RawVoltage >= 1.153){
        voltGain = .959;
    }
    else if(RawVoltage >= 1.364){
        voltGain = .959;
    }
    else if(RawVoltage > 1.047){
        voltGain = .952;
    }
    else if(RawVoltage >= .937){
        voltGain = .958;
    }
    else if(RawVoltage >= .843){
        voltGain = .945;
    }
    else if(RawVoltage >= .738){
        voltGain = .945;
    }
    else if(RawVoltage > .635){
        voltGain = .943;
    }
    else if(RawVoltage >= .528){
        voltGain = .935;
    }
    else if(RawVoltage >= .434){
        voltGain = .933;
    }
    else if(RawVoltage > .328){
        voltGain = .932;
    }
    else if(RawVoltage >= .205){
        voltGain = .941;
    }
    else if(RawVoltage >= .111){
        voltGain = .936;
    }
    else if(RawVoltage > .009883){
        voltGain = 1.135;
    }
    else{
        voltGain = 1.683;
    }
    Voltage=(voltGain*RawVoltage)-offset;
}
double Pressure_Transducer::getPressure(){
  RawVoltage = ((ADC_Value/ADC_STEPS)*V_REF);
    //Adjusts voltGain based on known values to adjust for nonlinearity
    if(RawVoltage >= 1.481){
        voltGain = .961;
    }
    else if(RawVoltage >= 1.364){
        voltGain = .959;
    }
    else if(RawVoltage >= 1.247){
        voltGain = .956;
    }
    else if(RawVoltage >= 1.153){
        voltGain = .959;
    }
    else if(RawVoltage >= 1.364){
        voltGain = .959;
    }
    else if(RawVoltage > 1.047){
        voltGain = .952;
    }
    else if(RawVoltage >= .937){
        voltGain = .958;
    }
    else if(RawVoltage >= .843){
        voltGain = .945;
    }
    else if(RawVoltage >= .738){
        voltGain = .945;
    }
    else if(RawVoltage > .635){
        voltGain = .943;
    }
    else if(RawVoltage >= .528){
        voltGain = .935;
    }
    else if(RawVoltage >= .434){
        voltGain = .933;
    }
    else if(RawVoltage > .328){
        voltGain = .932;
    }
    else if(RawVoltage >= .205){
        voltGain = .941;
    }
    else if(RawVoltage >= .111){
        voltGain = .936;
    }
    else if(RawVoltage > .009883){
        voltGain = 1.135;
    }
    else{
        voltGain = 1.683;
    }
    Voltage=(voltGain*RawVoltage)-offset;
    /*
    Place code to compensate for pressure transducer nonlinearity
    */
    Pressure=Voltage*pressureScale;

}