#include "wifi_config.h"
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <auto_mode.h>

using namespace websockets;

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

const char *websocket_server = "ws://192.168.1.163:8000/ws";

WebsocketsClient client;

AutoMode autoMode = AutoMode();

bool isRunning = false;
bool isStopped = true;

void stopMachine() {
  autoMode.stop_();
  Serial.println("Machine stopped");
  isStopped = true;
  isRunning = false;
}

void connectToWebSocket() {
  client.onMessage([](WebsocketsMessage message) {
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
    }
  });

  client.onEvent([](WebsocketsEvent event, String data) {
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

  bool connected = client.connect(websocket_server);
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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  connectToWebSocket();
}

void loop() {
  if (client.available()) {
    client.poll();

  } else {
    if (!isStopped) {
      stopMachine();
    }
    esp_deep_sleep(1000000);
  }

  if (isRunning) {
    autoMode.run();
  }
}