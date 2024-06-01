#include "SteeringControl.h"

SteeringControl::SteeringControl() {};

void SteeringControl::attach(uint8_t PWMPin) {
  ledcSetup(1, 50, 16);
  ledcAttachPin(PWMPin, 1);
  servo.attach(PWMPin);
}

void SteeringControl::debug() {
  Serial.print("Steering: ");
  Serial.print(String(currentDirection));
  Serial.print(" ");
  Serial.print(String(servo.read()));
  Serial.print("\n");
}

void SteeringControl::turn(steering_directions direction) {
  int correction = 2;
  currentDirection = direction;
  servo.write(direction + correction);
  vTaskDelay(pdMS_TO_TICKS(15));
  this->debug();
}
