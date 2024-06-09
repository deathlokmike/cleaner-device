#include "WheelControl.h"
#include "Globals.h"
#include "esp_log.h"

WheelControl::WheelControl() {};

void WheelControl::attach(uint8_t INA, uint8_t INB, uint8_t PWM)
{
    ina = INA;
    inb = INB;
    pwm = PWM;
    this->setup();
}

void WheelControl::debug()
{
    ESP_LOGD(autoModeLogTag, "Speed: %d", currentDirection);
}

void WheelControl::setup()
{
    ledcSetup(2, 20000, 8);
    ledcAttachPin(pwm, 2);
    pinMode(ina, OUTPUT);
    pinMode(inb, OUTPUT);
}

void WheelControl::go(wheel_directions direction)
{
    if (currentDirection != direction)
    {
        if (direction == forward)
            this->goForward();
        else if (direction == backward)
            this->goBackward();

        if (currentDirection == stop)
        {
            analogWrite(pwm, boost);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        currentDirection = direction;
        analogWrite(pwm, currentDirection);
    }
    this->debug();
}

void WheelControl::stop_()
{
    currentDirection = stop;
    analogWrite(pwm, currentDirection);
    digitalWrite(ina, HIGH);
    digitalWrite(inb, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(ina, LOW);
    digitalWrite(inb, LOW);
}

void WheelControl::goBackward()
{
    digitalWrite(ina, HIGH);
    digitalWrite(inb, LOW);
}

void WheelControl::goForward()
{
    digitalWrite(ina, LOW);
    digitalWrite(inb, HIGH);
}