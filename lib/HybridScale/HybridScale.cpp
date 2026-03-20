#include "HybridScale.h"

unsigned long HybridScale::update(){
    abstractForce = this->get_value();
    return micros();
}

double HybridScale::getRawForce(){
    return abstractForce;
}