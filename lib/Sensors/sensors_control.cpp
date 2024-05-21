#include "sensors_control.h"

SensorsControl::SensorsControl() {}

void SensorsControl::attach(uint8_t USFrontTrigPin, uint8_t USFrontEchoPin,
                            uint8_t USSideTrigPin, uint8_t USSideEchoPin) {
  this->frontTrigPin = USFrontTrigPin;
  this->frontEchoPin = USFrontEchoPin;
  this->sideTrigPin = USSideTrigPin;
  this->sideEchoPin = USSideEchoPin;

  pinMode(USFrontTrigPin, OUTPUT);
  pinMode(USFrontEchoPin, INPUT);

  pinMode(USSideTrigPin, OUTPUT);
  pinMode(USSideEchoPin, INPUT);
};

long SensorsControl::getSensorDistance(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return (duration * 0.017);
}

void SensorsControl::debug() {
  Serial.print("Front distance: ");
  Serial.print(String(distance.getFront()));
  Serial.print("\n");
  Serial.print("Side distance: ");
  Serial.print(String(distance.getSide()));
  Serial.print("\n");
}

Distance SensorsControl::getDistance() {
  distance.setFront(
      this->getSensorDistance(this->frontTrigPin, this->frontEchoPin));
  distance.setSide(
      this->getSensorDistance(this->sideTrigPin, this->sideEchoPin));
  this->debug();
  return distance;
}
