#pragma once

#include <SensorsControl.h>
#include <SteeringControl.h>
#include <WheelControl.h>

class AutoMode
{
    WheelControl wheel = WheelControl();
    SteeringControl steering = SteeringControl();
    SensorsControl sensors = SensorsControl();

public:
    AutoMode();
    void attachSteering(uint8_t PWM);
    void attachWheel(uint8_t INA, uint8_t INB, uint8_t PWM);
    void attachSensors(uint8_t USFrontTrig, int8_t USFrontEcho, int8_t USSideTrig,
                       int8_t USSideEcho);
    void run();
    void stop_();

    void checkSystems();
    void turnLeft();
};