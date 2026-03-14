#include "HybridScale.h"

unsigned long HybridScale::update(){
    abstractForce = this->get_value(15);
    return micros();
}

double HybridScale::getRawForce(){
    return abstractForce;
}