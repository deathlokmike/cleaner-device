#pragma once
#include "timer.h"

enum wheel_directions { forward = 25, backward = 25, boost = 100, stop = 0 };

class WheelControl {
private:
  uint8_t ina;
  uint8_t inb;
  uint8_t pwm;

  wheel_directions currentDirection = stop;

  void debug();
  void setup();

  void goForward();
  void goBackward();

public:
  WheelControl();
  void attach(uint8_t INAPin, uint8_t INBPin, uint8_t PWMPin);
  void go(wheel_directions direction);
  void stop_();
};