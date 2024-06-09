#include "SteeringControl.h"
#include "Globals.h"
#include "esp_log.h"

SteeringControl::SteeringControl() {};

void SteeringControl::attach(uint8_t PWM)
{
    servo.attach(PWM);
}

void SteeringControl::debug()
{
    ESP_LOGD(autoModeLogTag, "Steering. %d", servo.readMicroseconds());
}

void SteeringControl::turn(steering_directions direction)
{
    int correction = 0;
    if (currentDirection == right)
    {
        correction = 100;
    }
    else if (currentDirection == left)
    {
        correction = -20;
    }

    servo.writeMicroseconds(direction + correction);
    currentDirection = direction;
    vTaskDelay(pdMS_TO_TICKS(400));
    this->debug();
}