#include "OV7670.h"
#include "wifi_config.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <auto_mode.h>

using namespace websockets;

#define USB_SPEED 115200

#define CAM_PIN_PCLK 33
#define CAM_PIN_XCLK 32

#define CAM_PIN_SIOD 21
#define CAM_PIN_SIOC 22

#define CAM_PIN_D7 4
#define CAM_PIN_D6 12
#define CAM_PIN_D5 13
#define CAM_PIN_D4 14
#define CAM_PIN_D3 15
#define CAM_PIN_D2 16
#define CAM_PIN_D1 17
#define CAM_PIN_D0 27

#define CAM_PIN_VSYNC 34
#define CAM_PIN_HREF 35

#define FRONT_ECHO_PIN 18
#define FRONT_TRIG_PIN 19

#define SIDE_ECHO_PIN 39
#define SIDE_TRIG_PIN 2

#define VNH_INA_PIN 5
#define VNH_INB_PIN 26
#define VNH_PWM_PIN 25

#define SERVO_PWM_PIN 23

const char *websocket_server = "ws://192.168.1.163:8000/ws";
const char *endpoint = "http://192.168.1.163:8000/image";

void takeImageTask(void *parameters);
void machineControlTask(void *parameters);

WebsocketsClient wsClient;
OV7670 *camera;
AutoMode autoMode = AutoMode();
HTTPClient httpClient;

bool isRunning = false;
bool isStopped = true;
bool isImageSended = true;

void stopMachine() {
  autoMode.stop_();
  Serial.println("Machine stopped");
  isStopped = true;
  isRunning = false;
}

void connectToWebSocket() {
  wsClient.onMessage([](WebsocketsMessage message) {
    String msg = message.data();
    Serial.println("Message received: " + msg);

    if (msg == "start") {
      Serial.println("Received START signal");
      isRunning = true;
      isStopped = false;
    } else if (msg == "stop") {
      Serial.println("Received STOP signal");
      stopMachine();
    } else if (msg == "check") {
      Serial.println("Received CHECK signal");
      if (!isRunning)
      {
        // autoMode.checkSystems();
      }
    } else if (msg == "take_image") {
      Serial.println("Received TAKE_IMAGE signal");
      camera->oneFrame();
      httpClient.begin(endpoint);

      int httpResponseCode =
          httpClient.sendRequest("POST", camera->frame, camera->frameBytes);

      if (httpResponseCode > 0) {
        String payload = httpClient.getString();
        Serial.println(httpResponseCode);
        Serial.println(payload);
      } else {
        Serial.println("HTTP-response error");
      }
      httpClient.end();
    }
  });

  wsClient.onEvent([](WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
      Serial.println("WebSocket connection opened");
    } else if (event == WebsocketsEvent::ConnectionClosed) {
      Serial.println("WebSocket connection closed, reconnecting...");
      connectToWebSocket();
    } else if (event == WebsocketsEvent::GotPing) {
      Serial.println("Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
      Serial.println("Got a Pong!");
    }
  });

  bool connected = wsClient.connect(websocket_server);
  if (connected) {
    Serial.println("Connected to WebSocket server");
  } else {
    Serial.println("Failed to connect to WebSocket server, retrying...");
    delay(5000);
    connectToWebSocket();
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  autoMode.attachSensors(FRONT_TRIG_PIN, FRONT_ECHO_PIN, SIDE_TRIG_PIN,
                         SIDE_ECHO_PIN);
  autoMode.attachSteering(SERVO_PWM_PIN);
  autoMode.attachWheel(VNH_INA_PIN, VNH_INB_PIN, VNH_PWM_PIN);

  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, CAM_PIN_SIOD, CAM_PIN_SIOC,
                      CAM_PIN_VSYNC, CAM_PIN_HREF, CAM_PIN_XCLK, CAM_PIN_PCLK,
                      CAM_PIN_D0, CAM_PIN_D1, CAM_PIN_D2, CAM_PIN_D3,
                      CAM_PIN_D4, CAM_PIN_D5, CAM_PIN_D6, CAM_PIN_D7);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  connectToWebSocket();
  xTaskCreatePinnedToCore(machineControlTask, "machineControlTask", 10000, NULL,
                          1, NULL, 0);
  xTaskCreatePinnedToCore(takeImageTask, "takeImageTask", 10000, NULL, 1, NULL,
                          1);
}

void takeImageTask(void *parameters) {
  for (;;) {
    if (isRunning and isImageSended) {
      camera->oneFrame();
      isImageSended = false;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void machineControlTask(void *parameters) {
  for (;;) {
    if (wsClient.available()) {
      wsClient.poll();

    } else {
      if (!isStopped) {
        stopMachine();
      }
      esp_deep_sleep(1000000);
    }

    if (isRunning) {
      // autoMode.run();

      if (!isImageSended) {
        httpClient.begin(endpoint);

        int httpResponseCode =
            httpClient.sendRequest("POST", camera->frame, camera->frameBytes);

        if (httpResponseCode > 0) {
          String payload = httpClient.getString();
          Serial.println(httpResponseCode);
          Serial.println(payload);
        } else {
          Serial.println("HTTP-response error");
        }
        httpClient.end();
        isImageSended = true;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void loop() {}