#include <ArduinoWebsockets.h>
#include <AutoMode.h>
#include <WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "OV7670.h"

TaskHandle_t TaskMachineHandler = NULL;
TaskHandle_t TaskCameraHandler = NULL;
HTTPClient client;
websockets::WebsocketsClient wsClient;
OV7670 *camera;
AutoMode autoMode = AutoMode();

void takeImageTask(void *pvParameters);
void machineControlTask(void *pvParameters);
void superLoopTask(void *pvParameters);
// void checkSystemTask(void *pvParameters);
// void checkCameraTask(void *pvParameters);

void takeImageAndSendPostRequest() {
    camera->oneFrame();
    client.begin(endpoint);
    int httpResponseCode =
        client.sendRequest("POST", camera->frame, camera->frameBytes);
    if (httpResponseCode > 0) {
        String payload = client.getString();
        ESP_LOGI(mainLogTag, "HTTP: [%d] %s", httpResponseCode, payload);
    } else {
        ESP_LOGW(mainLogTag, "HTTP-response error");
    }
    client.end();
}

// void checkSystemTask(void *pvParameters) {
//   ESP_LOGD(mainLogTag, "[CST]: started");
//   autoMode.checkSystems();
//   wsClient.send("done");
//   vTaskDelete(NULL);
// }

// void checkCameraTask(void *pvParameters) {
//   ESP_LOGD(mainLogTag, "[CCT]: started");
//   takeImageAndSendPostRequest();
//   wsClient.send("done");
//   vTaskDelete(NULL);
// }

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
        vTaskResume(TaskMachineHandler);
        vTaskResume(TaskCameraHandler);
        ESP_LOGI(mainLogTag, "Machine started");
    } else if (message.data() == "stop") {
        vTaskSuspend(TaskMachineHandler);
        vTaskSuspend(TaskCameraHandler);
        autoMode.stop_();
        ESP_LOGI(mainLogTag, "Machine stopped");
    }
    // else if (message.data() == "check_system") {
    //   xTaskCreatePinnedToCore(checkSystemTask, "CST", 2048, NULL, 1,
    //                           &TaskCameraHandler, 0);
    // } else if (message.data() == "check_camera") {
    //   xTaskCreatePinnedToCore(checkCameraTask, "CCT", 2048, NULL, 1,
    //                           &TaskCameraHandler, 0);
    // }
    else if (message.data() == "suspend") {
        vTaskSuspend(TaskMachineHandler);
        autoMode.stop_();
        ESP_LOGI(mainLogTag, "Machine stopped");
    } else if (message.data() == "resume") {
        vTaskResume(TaskMachineHandler);
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
    Serial.begin(USB_SPEED);
    Wire.begin();
    Wire.setClock(100000);
    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());

    camera = new OV7670(CAM_VSYNC, CAM_HREF, CAM_XCLK, CAM_PCLK, CAM_D0, CAM_D1,
                        CAM_D2, CAM_D3, CAM_D4, CAM_D5, CAM_D6, CAM_D7);

    autoMode.attachSensors(FRONT_TRIG, FRONT_ECHO, SIDE_TRIG, SIDE_ECHO);
    autoMode.attachWheel(VNH_INA, VNH_INB, VNH_PWM);
    autoMode.attachSteering(SERVO_PWM);

    ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    ESP_LOGI(mainLogTag, "Connected to WiFi");
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onEventsCallback);
    connectToWebSocket();

    xTaskCreatePinnedToCore(superLoopTask, "SLT", 32768, NULL, 1, NULL, 0);
}

void superLoopTask(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[SLT]: started");
    xTaskCreatePinnedToCore(machineControlTask, "MCT", 2048, NULL, 1,
                            &TaskMachineHandler, 1);
    xTaskCreatePinnedToCore(takeImageTask, "TIT", 16384, NULL, 1,
                            &TaskCameraHandler, 0);

    if (TaskCameraHandler == NULL) {
        ESP_LOGW(mainLogTag, "TIT: Failed to create");
    } else if (TaskMachineHandler == NULL) {
        ESP_LOGW(mainLogTag, "MCT: Failed to create");
    }

    else {
        vTaskSuspend(TaskCameraHandler);
        vTaskSuspend(TaskMachineHandler);
        ESP_LOGD(mainLogTag, "Memory free: %d", ESP.getFreeHeap());
        while (true) {
            if (wsClient.available()) {
                wsClient.poll();
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
    vTaskDelete(NULL);
}

void takeImageTask(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[TIT]: started");
    while (true) {
        ESP_LOGD(mainLogTag, "[TIT]: iter");
        takeImageAndSendPostRequest();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    vTaskDelete(NULL);
}

void machineControlTask(void *pvParameters) {
    ESP_LOGD(mainLogTag, "[MCT]: started");
    while (true) {
        ESP_LOGD(mainLogTag, "[MCT]: iter");
        autoMode.run();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    vTaskDelete(NULL);
}

void loop() {}