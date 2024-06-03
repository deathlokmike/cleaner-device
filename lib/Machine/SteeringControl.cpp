#include "SteeringControl.h"
#include "Globals.h"
#include "esp_log.h"

SteeringControl::SteeringControl() {};

void SteeringControl::attach(uint8_t PWM) {
  ledcSetup(1, 50, 16);
  ledcAttachPin(PWM, 1);
  servo.attach(PWM);
}

void SteeringControl::debug() {
  ESP_LOGD(autoModeLogTag, "Steering. %d:%d", currentDirection, servo.read());
}

void SteeringControl::turn(steering_directions direction) {
  int correction = 2;
  currentDirection = direction;
  servo.write(direction + correction);
  vTaskDelay(pdMS_TO_TICKS(15));
  this->debug();
}
