#include "AutoMode.h"

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

void AutoMode::checkSystems() {
  // Distance distance = sensors.getDistance();
  // wheel.go(forward);
  // vTaskDelay(pdMS_TO_TICKS(50));
  // wheel.go(forward);
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // wheel.stop_();

  steering.turn(left);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(right);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(straight);
}

void AutoMode::run() {
  steering.turn(left);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(right);
  vTaskDelay(pdMS_TO_TICKS(500));
  steering.turn(straight);
  vTaskDelay(pdMS_TO_TICKS(500));
  // Distance distance = sensors.getDistance();
  // if (distance.getFront() > 50 and distance.getSide() <= 20) {

  //   if (distance.getSide() < 15) {
  //     steering.turn(left);
  //     vTaskDelay(pdMS_TO_TICKS(10));
  //     steering.turn(right);
  //     vTaskDelay(pdMS_TO_TICKS(10));
  //   }
  //   steering.turn(straight);

  // } else if (distance.getFront() < 50 and distance.getSide() <= 20) {
  //   wheel.go(forward);
  //   steering.turn(left);
  // } else if (distance.getFront() > 50 and distance.getSide() > 20) {
  //   wheel.go(forward);
  //   steering.turn(right);
  //   vTaskDelay(pdMS_TO_TICKS(10));
  //   steering.turn(left);
  //   vTaskDelay(pdMS_TO_TICKS(10));
  //   steering.turn(straight);
  // } else if (distance.getFront() < 15 and distance.getSide() < 10) {
  //   wheel.go(backward);
  //   steering.turn(right);
}
// void AutoMode::run() {
//   Distance distance = sensors.getDistance();
//   if (distance.getFront() > 50 and distance.getSide() <= 20) {
//     wheel.go(forward);
//     if (distance.getSide() < 15) {
//       steering.turn(left);
//       vTaskDelay(pdMS_TO_TICKS(10));
//       steering.turn(right);
//       vTaskDelay(pdMS_TO_TICKS(10));
//     }
//     steering.turn(straight);

//   } else if (distance.getFront() < 50 and distance.getSide() <= 20) {
//     wheel.go(forward);
//     steering.turn(left);
//   } else if (distance.getFront() > 50 and distance.getSide() > 20) {
//     wheel.go(forward);
//     steering.turn(right);
//     vTaskDelay(pdMS_TO_TICKS(10));
//     steering.turn(left);
//     vTaskDelay(pdMS_TO_TICKS(10));
//     steering.turn(straight);
//   } else if (distance.getFront() < 15 and distance.getSide() < 10) {
//     wheel.go(backward);
//     steering.turn(right);
//   }
// }