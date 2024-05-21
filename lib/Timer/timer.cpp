#include "timer.h"

Timer::Timer(int period) { _period = period; }
bool Timer::isReady() {
  if (millis() - _tmr >= _period) {
    _tmr = millis();
    return true;
  }
  return false;
}

uint32_t Timer::getRemainingTime() { return millis() - _tmr; }