#pragma once
#include <ESP32Servo.h>
#include <Arduino.h>

enum steering_directions { straight = 90, left = 125, right = 55 };

class SteeringControl {
private:
  Servo servo;

  void debug();

public:
  SteeringControl();
  steering_directions currentDirection = straight;
  void attach(uint8_t PWMPin);
  void turn(steering_directions direction);
};