#include <Arduino.h>
#include <ESP32Servo.h>
#include <AutoMode.h>
#include <esp_log.h>
#include <Globals.h>
#include <Config.h>

AutoMode autoMode = AutoMode();
bool one_time = true;

void setup()
{
    Serial.begin(USB_SPEED);
    autoMode.attachSensors(FRONT_TRIG, FRONT_ECHO, SIDE_TRIG, SIDE_ECHO);
    autoMode.attachWheel(VNH_INA, VNH_INB, VNH_PWM);
    autoMode.attachSteering(SERVO_PWM);
}

void loop()
{
    while(one_time){
        if (autoMode.turnLeft())
        {
           one_time = false;
        }
    }   
}
