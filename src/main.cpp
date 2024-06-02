#include "Config.h"
#include "OV7670.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <AutoMode.h>
#include <Log.h>
#include <WiFi.h>

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

#define STACK_SIZE 40960

const char *websocket_server = "ws://192.168.1.163:8000/ws";

TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;

void takeImageTask(void *pvParameters);
void machineControlTask(void *pvParameters);
void connectionControllerTask(void *pvParameters);

WebsocketsClient wsClient;

AutoMode autoMode = AutoMode();

bool isRunning = false;
bool isStopped = true;

void stopMachine() {
  autoMode.stop_();
  vTaskSuspend(Task1);
  vTaskSuspend(Task2);
  DEBUG_PRINTLN("Machine stopped");
  isStopped = true;
  isRunning = false;
}

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
    DEBUG_PRINTLN("I2S Run");
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
      isRunning = true;
      isStopped = false;
    } else if (msg == "stop") {
      DEBUG_PRINTLN("Received STOP signal");
      stopMachine();
    } else if (msg == "check") {
      DEBUG_PRINTLN("Received CHECK signal");
      if (!isRunning) {
        autoMode.checkSystems();
      }
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
  autoMode.attachSensors(FRONT_TRIG_PIN, FRONT_ECHO_PIN, SIDE_TRIG_PIN,
                         SIDE_ECHO_PIN);
  autoMode.attachWheel(VNH_INA_PIN, VNH_INB_PIN, VNH_PWM_PIN);
  autoMode.attachSteering(SERVO_PWM_PIN);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("Connected to WiFi");

  xTaskCreatePinnedToCore(connectionControllerTask, "CTT", 2048, NULL, 1, NULL,
                          0);
}

void connectionControllerTask(void *pvParameters) {
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
  OV7670 *camera = new OV7670(
      OV7670::Mode::QQVGA_RGB565, CAM_PIN_SIOD, CAM_PIN_SIOC, CAM_PIN_VSYNC,
      CAM_PIN_HREF, CAM_PIN_XCLK, CAM_PIN_PCLK, CAM_PIN_D0, CAM_PIN_D1,
      CAM_PIN_D2, CAM_PIN_D3, CAM_PIN_D4, CAM_PIN_D5, CAM_PIN_D6, CAM_PIN_D7);
  HTTPClient client;
  const char *endpoint = "http://192.168.1.163:8000/image";
  while (true) {
    if (isRunning) {
      takeImageAndSendPostRequest(camera, &client, endpoint);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  vTaskDelete(NULL);
}

void machineControlTask(void *pvParameters) {
  while (true) {
    if (isRunning) {
      autoMode.run();
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  vTaskDelete(NULL);
}

void loop() {}