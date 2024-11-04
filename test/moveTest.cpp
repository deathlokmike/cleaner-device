#include <Arduino.h>

#include "Config.h"
#include "Globals.h"
#include "esp_log.h"
byte speed = 191;

void backward() {
    analogWrite(IN1, 0);
    analogWrite(IN2, speed);
    analogWrite(IN3, 0);
    analogWrite(IN4, speed);
}
void forward() {
    analogWrite(IN1, speed);
    analogWrite(IN2, 0);
    analogWrite(IN3, speed);
    analogWrite(IN4, 0);
}
void right() {
    analogWrite(IN1, speed-50);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, speed-50);
}
void left() {
    analogWrite(IN1, 0);
    analogWrite(IN2, speed-50);
    analogWrite(IN3, speed-50);
    analogWrite(IN4, 0);
}
void stop() {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
    delay(2000);
}

void setup() {
    Serial.begin(USB_SPEED);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}
int counter = 0;
void loop() {
    while (counter < 3)
    {
        forward();
        delay(2000);
        stop();

        left();
        delay(400);
        stop();

        backward();
        delay(2000);
        stop();

        right();
        delay(400);
        stop();

        counter += 1;
        
    }
    
    // ESP_LOGD(mainLogTag, "Left");
    // left();
    // delay(400);
    // stop();
    // ESP_LOGD(mainLogTag, "Right");
    // right();
    // delay(400);
    // stop();
}