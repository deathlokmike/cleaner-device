#include "AutoMode.h"

AutoMode::AutoMode() {};

void AutoMode::attachSteering(uint8_t PWM) { steering.attach(PWM); }

void AutoMode::attachWheel(uint8_t INA, uint8_t INB, uint8_t PWM)
{
    wheel.attach(INA, INB, PWM);
}

void AutoMode::attachSensors(uint8_t USFrontTrig, int8_t USFrontEcho,
                             int8_t USSideTrig, int8_t USSideEcho)
{
    sensors.attach(USFrontTrig, USFrontEcho, USSideTrig, USSideEcho);
}

void AutoMode::stop_() { wheel.stop_(); }

void AutoMode::checkSystems()
{
    Distance distance = sensors.getDistance();
    wheel.go(forward);
    vTaskDelay(pdMS_TO_TICKS(1000));
    wheel.stop_();

    steering.turn(left);
    vTaskDelay(pdMS_TO_TICKS(500));
    steering.turn(right);
    vTaskDelay(pdMS_TO_TICKS(500));
    steering.turn(straight);
}

bool AutoMode::turnLeft()
{
    Distance distance = sensors.getDistance();
    if (distance.getFront() <= 49)
    {
        steering.turn(left);
        vTaskDelay(pdMS_TO_TICKS(1300));
        steering.turn(straight);
        vTaskDelay(pdMS_TO_TICKS(500));
        wheel.stop_();
        return true;
    }
    else
    {
        wheel.go(forward);
    }
    return false;
}

void AutoMode::run()
{
    this->checkSystems();
}
