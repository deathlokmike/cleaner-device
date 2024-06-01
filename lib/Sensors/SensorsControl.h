#pragma once
#include "Arduino.h"
#include "distance.h"

class SensorsControl {
private:
  Distance distance = Distance();
  uint8_t frontTrigPin = 0;
  uint8_t frontEchoPin = 0;
  uint8_t sideTrigPin = 0;
  uint8_t sideEchoPin = 0;
  long getSensorDistance(uint8_t trigPin, uint8_t echoPin);
  void debug();

public:
  SensorsControl();
  void attach(uint8_t USFrontTrigPin, uint8_t USFrontEchoPin,
              uint8_t USSideTrigPin, uint8_t USSideEchoPin);
  Distance getDistance();
};