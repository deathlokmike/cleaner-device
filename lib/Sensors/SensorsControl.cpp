#include "SensorsControl.h"
#include "Globals.h"
#include "esp_log.h"

SensorsControl::SensorsControl() {}

void SensorsControl::attach(uint8_t USFrontTrig, uint8_t USFrontEcho,
                            uint8_t USSideTrig, uint8_t USSideEcho)
{
    this->frontTrig = USFrontTrig;
    this->frontEcho = USFrontEcho;
    this->sideTrig = USSideTrig;
    this->sideEcho = USSideEcho;

    pinMode(USFrontTrig, OUTPUT);
    pinMode(USFrontEcho, INPUT);

    pinMode(USSideTrig, OUTPUT);
    pinMode(USSideEcho, INPUT);
};

long SensorsControl::getSensorDistance(uint8_t trig, uint8_t echo)
{
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long duration = pulseIn(echo, HIGH);
    return (duration * 0.017);
}

void SensorsControl::debug()
{
    ESP_LOGD(autoModeLogTag, "Front distance: %d", distance.getFront());
    ESP_LOGD(autoModeLogTag, "Side distance: %d", distance.getSide());
}

Distance SensorsControl::getDistance()
{
    distance.setFront(this->getSensorDistance(this->frontTrig, this->frontEcho));
    distance.setSide(this->getSensorDistance(this->sideTrig, this->sideEcho));
    this->debug();
    return distance;
}
