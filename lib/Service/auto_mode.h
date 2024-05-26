#pragma once

#include <sensors_control.h>
#include <steering_control.h>
#include <wheel_control.h>

class AutoMode {
  WheelControl wheel = WheelControl();
  SteeringControl steering = SteeringControl();
  SensorsControl sensors = SensorsControl();

  Timer timer = Timer(50);

public:
  AutoMode();
  void attachSteering(uint8_t PWMPin);
  void attachWheel(uint8_t INAPin, uint8_t INBPin, uint8_t PWMPin);
  void attachSensors(uint8_t USFrontTrigPin, int8_t USFrontEchoPin,
                     int8_t USSideTrigPin, int8_t USSideEchoPin);
  void run();
  void stop_();

  void checkSystems();
};