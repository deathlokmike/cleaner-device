#include <Wire.h>

#include "Config.h"
#include "OV7670.h"

OV7670 *camera;

void setup() {
    Serial.begin(USB_SPEED);
    Wire.begin();
    Wire.setClock(100000);
    uint32_t memBefore = ESP.getFreeHeap();
    camera = new OV7670(CAM_VSYNC, CAM_HREF, CAM_XCLK, CAM_PCLK, CAM_D0, CAM_D1,
                        CAM_D2, CAM_D3, CAM_D4, CAM_D5, CAM_D6, CAM_D7);
    ESP_LOGD(mainLogTag, "Memory allocated: %d", memBefore - ESP.getFreeHeap());
}

void loop() {
    camera->oneFrame();
    delay(2000);
}