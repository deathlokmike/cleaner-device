#include <HTTPClient.h>
#include <Wire.h>

#include "Config.h"
#include "OV7670.h"

OV7670 camera;
HTTPClient http;
int32_t initSize;
uint8_t counter = 0;

void setup() {
    Serial.begin(USB_SPEED);
    Wire.begin();
    initSize = ESP.getFreeHeap();
    ESP_LOGD(mainLogTag, "Free: %d", initSize);
    if (!camera.init(CAM_VSYNC, CAM_HREF, CAM_XCLK, CAM_PCLK, CAM_D0, CAM_D1,
                     CAM_D2, CAM_D3, CAM_D4, CAM_D5, CAM_D6, CAM_D7)) {
        ESP_LOGE(mainLogTag, "Camera initialization failed!");
        return;
    }

    ESP_LOGD(mainLogTag, "Memory allocated: %d", initSize - ESP.getFreeHeap());
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void loop() {
    if ((WiFi.status() == WL_CONNECTED)) {
        camera.oneFrame();
        ++counter;
        if (counter > 5) {
            ESP_LOGD(mainLogTag, "Before photo: %d",
                     initSize - ESP.getFreeHeap());
            ESP_LOGD(mainLogTag, "After photo: %d",
                     initSize - ESP.getFreeHeap());
            http.begin(endpoint);
            ESP_LOGD(mainLogTag, "http: %d", initSize - ESP.getFreeHeap());
            int httpResponseCode =
                http.sendRequest("POST", camera.frame, camera.frameBytes);

            if (httpResponseCode > 0) {
                String payload = http.getString();
                ESP_LOGD(mainLogTag, "Payload: %s", payload.c_str());
            }
            http.end();
            counter = 0;
        }
    }
}