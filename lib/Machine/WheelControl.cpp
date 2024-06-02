#include "WheelControl.h"
#include "Log.h"
WheelControl::WheelControl() {};

void WheelControl::attach(uint8_t INAPin, uint8_t INBPin, uint8_t PWMPin) {
  ina = INAPin;
  inb = INBPin;
  pwm = PWMPin;
  this->setup();
}

void WheelControl::debug() {
  DEBUG_PRINT("Speed: ");
  DEBUG_PRINTLN(String(currentDirection));
}

void WheelControl::setup() {
  pinMode(ina, OUTPUT);
  pinMode(inb, OUTPUT);
  pinMode(pwm, OUTPUT);
}

void WheelControl::go(wheel_directions direction) {
  if (currentDirection != direction) {
    if (direction == forward)
      this->goForward();
    else if (direction == backward)
      this->goBackward();

    currentDirection = currentDirection != boost ? boost : direction;
    analogWrite(pwm, currentDirection);
  }
  this->debug();
}

void WheelControl::stop_() {
  currentDirection = stop;
  analogWrite(pwm, currentDirection);
  digitalWrite(ina, LOW);
  digitalWrite(inb, LOW);
}

void WheelControl::goBackward() {
  digitalWrite(ina, HIGH);
  digitalWrite(inb, LOW);
}

void WheelControl::goForward() {
  digitalWrite(ina, LOW);
  digitalWrite(inb, HIGH);
}