#pragma once

#include <Arduino.h>
#include <HX711.h>

class HybridScale : public HX711{
    private:
        double abstractForce;
    public:
        unsigned long update();
        double getRawForce();
}