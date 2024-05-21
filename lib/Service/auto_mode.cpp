#include "auto_mode.h"

AutoMode::AutoMode() {};

void AutoMode::attachSteering(uint8_t PWMPin) { steering.attach(PWMPin); }

void AutoMode::attachWheel(uint8_t INAPin, uint8_t INBPin, uint8_t PWMPin) {
  wheel.attach(INAPin, INBPin, PWMPin);
}

void AutoMode::attachSensors(uint8_t USFrontTrigPin, int8_t USFrontEchoPin,
                             int8_t USSideTrigPin, int8_t USSideEchoPin) {
  sensors.attach(USFrontTrigPin, USFrontEchoPin, USSideTrigPin, USSideEchoPin);
}

void AutoMode::stop_() { wheel.stop_(); }

void AutoMode::run() {
  if (timer.isReady()) {
    Distance distance = sensors.getDistance();
    if (distance.getFront() > 50 and distance.getSide() <= 20) {
      wheel.go(forward);
      if (distance.getSide() < 15) {
        steering.turn(left);
        delay(10);
        steering.turn(right);
        delay(10);
      }
      steering.turn(straight);

    } else if (distance.getFront() < 50 and distance.getSide() <= 20) {
      wheel.go(forward);
      steering.turn(left);
    } else if (distance.getFront() > 50 and distance.getSide() > 20) {
      wheel.go(forward);
      steering.turn(right);
      delay(10);
      steering.turn(left);
      delay(10);
      steering.turn(straight);
    } else if (distance.getFront() < 15 and distance.getSide() < 10) {
      wheel.go(backward);
      steering.turn(right);
    }
  }
}