#include "SensorsControl.h"
#include "Distance.h"
#include "Globals.h"
#include "esp_log.h"

SensorsControl::SensorsControl() {}

void SensorsControl::attach(uint8_t frontEcho, uint8_t rightSideEcho,
                            uint8_t rightFrontEcho, uint8_t trig) {
    front = frontEcho;
    rightSide = rightSideEcho;
    rightFront = rightFrontEcho;
    trig = trig;

    pinMode(front, INPUT);
    pinMode(rightSide, INPUT);
    pinMode(rightFront, INPUT);
    pinMode(trig, OUTPUT);
};

uint8_t SensorsControl::getSensorDistance(uint8_t echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH);
    return uint8_t(duration * 0.017);
}

void SensorsControl::debug() {
    ESP_LOGD(autoModeLogTag, "Front distance: %d", distance.front);
    ESP_LOGD(autoModeLogTag, "Right side distance: %d", distance.rightSide);
    ESP_LOGD(autoModeLogTag, "Right front distance: %d", distance.rightFront);
}

Distance SensorsControl::getDistance() {
    distance.front = getSensorDistance(front);
    distance.rightSide = getSensorDistance(rightSide);
    distance.rightFront = getSensorDistance(rightFront);
    this->debug();
    return distance;
}
