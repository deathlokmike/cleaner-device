#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "Globals.h"
#include "INA219_WE.h"
#include "MPU6050.h"
#include "WheelControl.h"
#include "esp_log.h"

websockets::WebsocketsClient wsClient;
INA219_WE ina219;
MPU6050 accgyro;
WheelControl wheels = WheelControl();
TaskHandle_t movementTaskHandle = NULL;

void mainLoop(void *pvParameters);
void sensorTask(void *pvParameters);
void movementTask(void *pvParameters);

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    using websockets::WebsocketsEvent;
    if (event == WebsocketsEvent::ConnectionOpened) {
        ESP_LOGI(mainLogTag, "WebSocket connection opened");
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        ESP_LOGW(mainLogTag, "WebSocket connection closed");
    } else if (event == WebsocketsEvent::GotPing) {
        ESP_LOGD(mainLogTag, "Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
        ESP_LOGD(mainLogTag, "Got a Pong!");
    }
}

void onMessageCallback(websockets::WebsocketsMessage message) {
    if (message.data() == "start") {
        ESP_LOGI(mainLogTag, "Machine started");
        if (movementTaskHandle != NULL) {
            vTaskResume(movementTaskHandle);
        }
    } else if (message.data() == "stop") {
        ESP_LOGI(mainLogTag, "Machine stopped");
        if (movementTaskHandle != NULL) {
            vTaskSuspend(movementTaskHandle);
        }
    } else if (message.data() == "suspend") {
        ESP_LOGI(mainLogTag, "Machine suspended");
    } else if (message.data() == "resume") {
        ESP_LOGI(mainLogTag, "Machine resumed");
    }
}

void connectToWebSocket() {
    while (!wsClient.connect(websocket_server)) {
        ESP_LOGW(mainLogTag,
                 "Failed to connect to WebSocket server, retrying...");
        vTaskDelay(5000);
    }

    ESP_LOGI(mainLogTag, "Connected to WebSocket server");
    String mac = "mac:" + String(WiFi.macAddress());
    wsClient.send(mac);
}

void setup() {
    pinMode(IN1, OUTPUT);
    analogWrite(IN1, 0);

    Serial.begin(USB_SPEED);
    wheels.attach(IN1, IN2, IN3, IN4);
    wheels.stop();

    Wire.begin();
    Wire.setClock(100000);
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    ESP_LOGI(mainLogTag, "Connected to WiFi");
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onEventsCallback);
    connectToWebSocket();
    accgyro.initialize();

    if (!ina219.init()) {
        ESP_LOGE(mainLogTag, "INA219 initialization failed!");
    }
    xTaskCreatePinnedToCore(mainLoop, "main", 32768, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(sensorTask, "sensor", 8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(movementTask, "movement", 4096, NULL, 1,
                            &movementTaskHandle, 1);
    vTaskSuspend(movementTaskHandle);
}

void sensorTask(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[sensorTask]: started");
    int16_t ax, ay, az, gx, gy, gz;
    float busVoltage, current_mA;
    while (true) {
        busVoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA();
        accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        String data = "vol:" + String(busVoltage) +
                      ",cur:" + String(current_mA) + ",ax:" + String(ax) +
                      ",ay:" + String(ay) + ",az:" + String(az);

        if (wsClient.available()) {
            wsClient.send(data);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void movementTask(void *pvParameters) {
    while (true) {
        wheels.forward();
        vTaskDelay(pdMS_TO_TICKS(1000));

        wheels.stop();
        vTaskDelay(pdMS_TO_TICKS(1000));

        wheels.left();
        vTaskDelay(pdMS_TO_TICKS(250));

        wheels.stop();
        vTaskDelay(pdMS_TO_TICKS(1000));

        wheels.backward();
        vTaskDelay(pdMS_TO_TICKS(1000));

        wheels.stop();
        vTaskDelay(pdMS_TO_TICKS(1000));

        wheels.right();
        vTaskDelay(pdMS_TO_TICKS(250));

        wheels.stop();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void mainLoop(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[main]: started");
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());
    while (true) {
        if (wsClient.available()) {
            wsClient.poll();
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    vTaskDelete(NULL);
}

void loop() {}