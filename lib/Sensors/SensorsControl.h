#pragma once
#include "Arduino.h"
#include "Distance.h"

class SensorsControl {
   private:
    Distance distance = Distance();
    uint8_t frontTrig = 0;
    uint8_t frontEcho = 0;
    uint8_t sideTrig = 0;
    uint8_t sideEcho = 0;
    long getSensorDistance(uint8_t trig, uint8_t echo);
    void debug();

   public:
    SensorsControl();
    void attach(uint8_t USFrontTrig, uint8_t USFrontEcho, uint8_t USSideTrig,
                uint8_t USSideEcho);
    Distance getDistance();
};