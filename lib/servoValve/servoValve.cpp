#include "servoValve.h"
servoValve::servoValve(){
    this->begin();
    this->setOscillatorFrequency(25000000);
    this->setPWMFreq(SERVO_FRQ);
    for (int index = 0; index<=15; index++){
        OPEN[index] = defaultOpen;
        CLOSED[index] = defaultClosed;
    }
}
void servoValve::setChannelValues(int channel_number, int open_pulse, int closed_pulse){
    OPEN[channel_number] = open_pulse;
    CLOSED[channel_number] = closed_pulse;
}
void servoValve::setValveState(int channel_number, bool state){
    if(state == 1){
        this->writeMicroseconds(channel_number, OPEN[channel_number]);
    }
    else{
        this->writeMicroseconds(channel_number, CLOSED[channel_number]);
    }
}