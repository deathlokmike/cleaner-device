#pragma once
#include <ESP32Servo.h>
#include <Arduino.h>

enum steering_directions
{
    right = 1236,
    straight = 1500,
    left = 1764
};

class SteeringControl
{
private:
    Servo servo;

    void debug();

public:
    SteeringControl();
    steering_directions currentDirection = straight;
    void attach(uint8_t PWM);
    void turn(steering_directions direction);
};