#include <Arduino.h>
#include <ESP32Servo.h>
#include <AutoMode.h>
#include "Config.h"

AutoMode autoMode = AutoMode();
bool one_time = true;

void setup()
{
    Serial.begin(USB_SPEED);
    autoMode.attachSensors(FRONT_TRIG, FRONT_ECHO, SIDE_TRIG, SIDE_ECHO);
    autoMode.attachSteering(SERVO_PWM);
    autoMode.attachWheel(VNH_INA, VNH_INB, VNH_PWM);
}

void loop()
{
    if (one_time)
    {
        autoMode.checkSystems();
        one_time = false;
    }
}
