#pragma once

#include <SensorsControl.h>
#include <SteeringControl.h>
#include <WheelControl.h>

class AutoMode {
  WheelControl wheel = WheelControl();
  SteeringControl steering = SteeringControl();
  SensorsControl sensors = SensorsControl();

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