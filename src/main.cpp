#include <ArduinoWebsockets.h>
#include <HCSR04.h>
#include <INA219_WE.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "Globals.h"
#include "WheelControl.h"
#include "esp_log.h"

websockets::WebsocketsClient wsClient;
INA219_WE ina219;
MPU6050 accgyro;
WheelControl wheels = WheelControl();
TaskHandle_t movementTaskHandle = NULL;

int16_t ax, ay, az, gx, gy, gz;
float currentAngle = 0;

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
            wheels.stop();
        }
    } else if (message.data() == "suspend") {
        ESP_LOGI(mainLogTag, "Machine suspended");
    } else if (message.data() == "resume") {
        ESP_LOGI(mainLogTag, "Machine resumed");
    }
}

void connectToServer() {
    wsClient = websockets::WebsocketsClient();
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onEventsCallback);
    ESP_LOGD(mainLogTag, "Connect to server");
    while (!wsClient.connect(websocket_server)) {
        ESP_LOGW(mainLogTag, "Failed to connect to server, retrying...");
        vTaskDelay(5000);
    }

    ESP_LOGD(mainLogTag, "Connected to server");
    String mac = "mac:" + String(WiFi.macAddress());
    wsClient.send(mac);
}

void connectToWifi() {
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    ESP_LOGI(mainLogTag, "Connected to WiFi");
}

void setup() {
    Serial.begin(USB_SPEED);
    wheels.attach(IN1, IN2, IN3, IN4);
    wheels.stop();

    Wire.begin();
    Wire.setClock(100000);
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());

    if (!ina219.init()) {
        ESP_LOGE(mainLogTag, "INA219 initialization failed!");
        return;
    }
    ESP_LOGI(mainLogTag, "INA219: Successful");
    connectToWifi();
    connectToServer();

    accgyro.initialize();
    accgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    ESP_LOGI(mainLogTag, "Calibrate MPU (20)");
    accgyro.CalibrateGyro(20);
    accgyro.CalibrateAccel(20);
    ESP_LOGI(mainLogTag, "MPU6050: Successful");

    byte *echoPins = new byte[2]{FRONT_ECHO, SIDE_ECHO};
    HCSR04.begin(TRIG, echoPins, 2);

    xTaskCreatePinnedToCore(mainLoop, "main", 32768, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(sensorTask, "sensor", 8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(movementTask, "movement", 4096, NULL, 1,
                            &movementTaskHandle, 1);
    vTaskSuspend(movementTaskHandle);
}

void sensorTask(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[sensorTask]: started");
    float busVoltage, current_mA;
    uint8_t poolLoop = 0;

    String data;
    while (true) {
        busVoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA();
        double *distances = HCSR04.measureDistanceCm();

        data = "vol:" + String(busVoltage) + ",cur:" + String(current_mA) +
               ",ang:" + String(currentAngle) + ",df:" + String(distances[0]) +
               ",ds:" + String(distances[1]);

        poolLoop += 1;
        if (wsClient.available()) {
            if (poolLoop == 4) {
                poolLoop = 0;
                wsClient.poll();
            } else {
                wsClient.send(data);
            }
        } else {
            ESP_LOGD(mainLogTag, "Wi-fi status: %d", WiFi.status());
            if (WiFi.status() != WL_CONNECTED) {
                connectToWifi();
            }
            wsClient.close();
            connectToServer();
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    vTaskDelete(NULL);
}

void rotate(float angle) {
    // Alpha - beta filter
    static float alpha = 0.9;
    static float beta = 0.0025;
    float estimatedRate = 0;
    float dt = 0;
    float gyroZ = 0;
    bool once = true;
    long previousTime = 0;
    currentAngle = 0;
    while (true) {
        accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        if (once) {
            wheels.left();
            once = false;
        }

        gyroZ = gz / 131;
        long currentTime = millis();
        dt = (currentTime - previousTime) / 1000.0;
        previousTime = currentTime;

        float predictedAngle = currentAngle + estimatedRate * dt;
        float predictedRate = estimatedRate;

        float residual = gyroZ - predictedRate;
        currentAngle = predictedAngle + alpha * residual * dt;
        estimatedRate = predictedRate + beta * residual;
        ESP_LOGD(mainLogTag, "currentAngle: %f", currentAngle);
        if (currentAngle >= angle) {
            wheels.stop();
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void movementTask(void *pvParameters) {
    while (true) {
        wheels.forward();
        vTaskDelay(pdMS_TO_TICKS(2000));
        wheels.stop();
        vTaskDelay(pdMS_TO_TICKS(1000));
        rotate(69.5);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void mainLoop(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[main]: started");
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    vTaskDelete(NULL);
}

void loop() {}