#include "OV7670.h"
#include "Config.h"
#include <Arduino.h>
#include <HTTPClient.h>
OV7670 *camera;
String endpoint = "http://192.168.1.163:8000/image";

void setup()
{
    Serial.begin(USB_SPEED);

    camera = new OV7670(CAM_VSYNC, CAM_HREF, CAM_XCLK, CAM_PCLK,
                        CAM_D0, CAM_D1, CAM_D2, CAM_D3,
                        CAM_D4, CAM_D5, CAM_D6, CAM_D7);
    Serial.println("Идет подключение к Wi-Fi");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Соединение с Wi-Fi установлено");
}

void loop()
{
    if ((WiFi.status() == WL_CONNECTED))
    {
        HTTPClient http;
        Serial.println("Делается снимок");
        camera->oneFrame();
        Serial.println("Снимок сохранен");
        String url = endpoint;
        http.begin(url.c_str());

        int httpResponseCode =
            http.sendRequest("POST", camera->frame, camera->frameBytes);

        if (httpResponseCode > 0)
        {
            String payload = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(payload);
        }
        else
        {
            Serial.println("Ошибка HTTP-запроса");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }
}