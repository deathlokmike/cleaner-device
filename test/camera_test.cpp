#include "OV7670.h"
#include "wifi_config.h"
#include <Arduino.h>
#include <HTTPClient.h>
#define USB_SPEED 115200

#define CAM_PIN_PCLK 13
#define CAM_PIN_XCLK 0

#define CAM_PIN_SIOD 21
#define CAM_PIN_SIOC 22

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 33
#define CAM_PIN_D4 32
#define CAM_PIN_D3 25
#define CAM_PIN_D2 26
#define CAM_PIN_D1 27
#define CAM_PIN_D0 14

#define CAM_PIN_VSYNC 12
#define CAM_PIN_HREF 4

#define FRONT_ECHO_PIN 18
#define FRONT_TRIG_PIN 19

#define SIDE_ECHO_PIN 2
#define SIDE_TRIG_PIN 15

#define VNH_INA_PIN 5
#define VNH_INB_PIN 17
#define VNH_PWM_PIN 16

#define SERVO_PWM_PIN 23

OV7670 *camera;
String endpoint = "http://192.168.1.163:8000/image";

void setup() {
  Serial.begin(USB_SPEED);

  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, CAM_PIN_SIOD, CAM_PIN_SIOC,
                      CAM_PIN_VSYNC, CAM_PIN_HREF, CAM_PIN_XCLK, CAM_PIN_PCLK,
                      CAM_PIN_D0, CAM_PIN_D1, CAM_PIN_D2, CAM_PIN_D3,
                      CAM_PIN_D4, CAM_PIN_D5, CAM_PIN_D6, CAM_PIN_D7);
  Serial.println("Идет подключение к Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Соединение с Wi-Fi установлено");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    Serial.println("Делается снимок");
    camera->oneFrame();
    Serial.println("Снимок сохранен");
    String url = endpoint;
    http.begin(url.c_str());

    int httpResponseCode =
        http.sendRequest("POST", camera->frame, camera->frameBytes);

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(payload);
    } else {
      Serial.println("Ошибка HTTP-запроса");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}