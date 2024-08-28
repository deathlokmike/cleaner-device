#pragma once
#include "Arduino.h"
#include "Distance.h"

class SensorsControl {
   private:
    Distance distance = Distance();
    uint8_t front = 0;
    uint8_t rightSide = 0;
    uint8_t rightFront = 0;
    uint8_t trig = 0;
    uint8_t getSensorDistance(uint8_t echo);
    void debug();

   public:
    SensorsControl();
    void attach(uint8_t frontEcho, uint8_t rightSideEcho,
                uint8_t rightFrontEcho, uint8_t trig);
    Distance getDistance();
};