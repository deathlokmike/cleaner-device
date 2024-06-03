#include "AutoMode.h"

AutoMode::AutoMode() {};

void AutoMode::attachSteering(uint8_t PWM) { steering.attach(PWM); }

void AutoMode::attachWheel(uint8_t INA, uint8_t INB, uint8_t PWM) {
  wheel.attach(INA, INB, PWM);
}

void AutoMode::attachSensors(uint8_t USFrontTrig, int8_t USFrontEcho,
                             int8_t USSideTrig, int8_t USSideEcho) {
  sensors.attach(USFrontTrig, USFrontEcho, USSideTrig, USSideEcho);
}

void AutoMode::stop_() { wheel.stop_(); }

void AutoMode::checkSystems() {
  Distance distance = sensors.getDistance();
  wheel.go(forward);
  vTaskDelay(pdMS_TO_TICKS(50));
  wheel.go(forward);
  vTaskDelay(pdMS_TO_TICKS(1000));
  wheel.stop_();

  steering.turn(left);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(right);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(straight);
}

void AutoMode::run() {
  Distance distance = sensors.getDistance();
  if (distance.getFront() > 50 and distance.getSide() <= 20) {

    if (distance.getSide() < 15) {
      steering.turn(left);
      vTaskDelay(pdMS_TO_TICKS(10));
      steering.turn(right);
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    steering.turn(straight);

  } else if (distance.getFront() < 50 and distance.getSide() <= 20) {
    wheel.go(forward);
    steering.turn(left);
  } else if (distance.getFront() > 50 and distance.getSide() > 20) {
    wheel.go(forward);
    steering.turn(right);
    vTaskDelay(pdMS_TO_TICKS(10));
    steering.turn(left);
    vTaskDelay(pdMS_TO_TICKS(10));
    steering.turn(straight);
  } else if (distance.getFront() < 15 and distance.getSide() < 10) {
    wheel.go(backward);
    steering.turn(right);
  }
}
