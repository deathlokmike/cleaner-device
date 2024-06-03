#include "Config.h"
#include "OV7670.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <AutoMode.h>
#include <Log.h>
#include <WiFi.h>

using namespace websockets;

const char *websocket_server = "ws://192.168.1.163:8000/ws";

TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;

void takeImageTask(void *pvParameters);
void machineControlTask(void *pvParameters);
void superLoopTask(void *pvParameters);

WebsocketsClient wsClient;

AutoMode autoMode = AutoMode();

void takeImageAndSendPostRequest(OV7670 *camera, HTTPClient *client,
                                 const char *endpoint) {
  camera->oneFrame();
  client->begin(endpoint);
  int httpResponseCode =
      client->sendRequest("POST", camera->frame, camera->frameBytes);
  if (httpResponseCode > 0) {
    String payload = client->getString();
    DEBUG_PRINTLN(httpResponseCode);
    DEBUG_PRINTLN(payload);
  } else {
    DEBUG_PRINTLN("HTTP-response error");
  }
  client->end();
}

void connectToWebSocket() {
  wsClient.onMessage([](WebsocketsMessage message) {
    String msg = message.data();
    if (msg == "start") {
      DEBUG_PRINTLN("Received START signal");
      vTaskResume(Task1);
      vTaskResume(Task2);
      DEBUG_PRINTLN("Machine started");
    } else if (msg == "stop") {
      DEBUG_PRINTLN("Received STOP signal");
      vTaskSuspend(Task1);
      vTaskSuspend(Task2);
      autoMode.stop_();
      DEBUG_PRINTLN("Machine stopped");
    } else if (msg == "check") {
      DEBUG_PRINTLN("Received CHECK signal");
      autoMode.checkSystems();
    } else if (msg == "take_image") {
      DEBUG_PRINTLN("Received TAKE_IMAGE signal");
    }
  });

  wsClient.onEvent([](WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
      DEBUG_PRINTLN("WebSocket connection opened");
    } else if (event == WebsocketsEvent::ConnectionClosed) {
      DEBUG_PRINTLN("WebSocket connection closed, reconnecting...");
      connectToWebSocket();
    } else if (event == WebsocketsEvent::GotPing) {
      DEBUG_PRINTLN("Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
      DEBUG_PRINTLN("Got a Pong!");
    }
  });

  bool connected = wsClient.connect(websocket_server);
  if (connected) {
    DEBUG_PRINTLN("Connected to WebSocket server");
  } else {
    DEBUG_PRINTLN("Failed to connect to WebSocket server, retrying...");
    vTaskDelay(5000);
    connectToWebSocket();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  autoMode.attachSensors(FRONT_TRIG, FRONT_ECHO, SIDE_TRIG, SIDE_ECHO);
  autoMode.attachWheel(VNH_INA, VNH_INB, VNH_PWM);
  autoMode.attachSteering(SERVO_PWM);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("Connected to WiFi");

  xTaskCreatePinnedToCore(superLoopTask, "SLT", 2048, NULL, 1, NULL, 0);
}

void superLoopTask(void *pvParameters) {
  connectToWebSocket();
  xTaskCreatePinnedToCore(machineControlTask, "MCT", 2048, NULL, 1, &Task1, 1);
  vTaskSuspend(Task1);
  static StaticTask_t xTaskBuffer;
  static StackType_t xStack[STACK_SIZE];
  Task2 = xTaskCreateStaticPinnedToCore(takeImageTask, "TIT", STACK_SIZE, NULL,
                                        1, xStack, &xTaskBuffer, 0);

  if (Task2 == NULL) {
    DEBUG_PRINTLN("Task 2: Failed to create");
  } else {
    vTaskSuspend(Task2);
    while (true) {
      if (wsClient.available()) {
        wsClient.poll();
      } else {
        esp_deep_sleep(1000000);
      }
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
  vTaskDelete(NULL);
}

void takeImageTask(void *pvParameters) {
  OV7670 *camera =
      new OV7670(OV7670::Mode::QQVGA_RGB565, CAM_SIOD, CAM_SIOC, CAM_VSYNC,
                 CAM_HREF, CAM_XCLK, CAM_PCLK, CAM_D0, CAM_D1, CAM_D2, CAM_D3,
                 CAM_D4, CAM_D5, CAM_D6, CAM_D7);
  HTTPClient client;
  const char *endpoint = "http://192.168.1.163:8000/image";
  while (true) {
    takeImageAndSendPostRequest(camera, &client, endpoint);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  vTaskDelete(NULL);
}

void machineControlTask(void *pvParameters) {
  while (true) {
    autoMode.run();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  vTaskDelete(NULL);
}

void loop() {}