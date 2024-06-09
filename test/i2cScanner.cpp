#include "Config.h"
#include "Globals.h"
#include "esp_log.h"
#include <Arduino.h>
#include <Wire.h>
#include <AutoMode.h>

byte error, address;

int nDevices;

void setup()
{
    Serial.begin(USB_SPEED);

    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());
    Wire.begin();
    ESP_LOGD(mainLogTag, "Wire started");
}

void loop()
{
    ESP_LOGD(mainLogTag, "Scanning");
    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            if (address < 16)
            {
                ESP_LOGD(mainLogTag, "I2C device found at address 0x0%X", address);
            }
            else
            {
                ESP_LOGD(mainLogTag, "I2C device found at address 0x%X", address);
            }
            nDevices++;
        }
        else if (error == 4)
        {
            if (address < 16)
            {
                ESP_LOGD(mainLogTag, "Unknown error at address 0x0%X", address);
            }
            else
            {
                ESP_LOGD(mainLogTag, "Unknown error at address 0x%X", address);
            }
        }
    }
    if (nDevices == 0)
        ESP_LOGD(mainLogTag, "No I2C devices found");
    else
        ESP_LOGD(mainLogTag, "Done");
    delay(5000);
}
