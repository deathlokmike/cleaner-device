#include <Arduino.h>
#include <Wire.h>

void writeRegister(byte address, byte reg, byte value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}