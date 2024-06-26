#include <Arduino.h>
#include <Wire.h>

#include "Globals.h"
#include "esp_log.h"

static uint8_t i2cError;

void writeRegister(byte address, byte reg, byte value) {
    Wire.beginTransmission(address >> 1);
    Wire.write(reg);
    Wire.write(value);
    i2cError = Wire.endTransmission();
    if (i2cError != 0) {
        ESP_LOGE(i2cLogTag, "Error: %d, Addr: 0x%X, Reg: 0x%X, Val: 0x%X",
                 i2cError, address, reg, value);
    }
}