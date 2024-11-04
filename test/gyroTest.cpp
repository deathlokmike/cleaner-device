#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Globals.h"
#include "MPU6050.h"
#include "esp_log.h"
#define TIME_OUT 20

MPU6050 accgyro;
unsigned long int t1;


void setup() {
    Serial.begin(USB_SPEED);
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());
    Wire.begin();
    ESP_LOGD(mainLogTag, "Wire started");
    accgyro.initialize();
    ESP_LOGD(mainLogTag, "Gyro initialized");
}

void loop() {
    long int t = millis();
    if( t1 < t ){
        int16_t ax, ay, az, gx, gy, gz;

        t1 = t + TIME_OUT;
        accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.println(ay);
    }
}