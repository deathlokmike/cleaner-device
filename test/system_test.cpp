#include <Arduino.h>
#include <ESP32Servo.h>
#include <auto_mode.h>


#define USB_SPEED 115200

#define FRONT_ECHO_PIN 18
#define FRONT_TRIG_PIN 19

#define SIDE_ECHO_PIN 36
#define SIDE_TRIG_PIN 2

#define VNH_INA_PIN 5
#define VNH_INB_PIN 25
#define VNH_PWM_PIN 26

#define SERVO_PWM_PIN 23

AutoMode autoMode = AutoMode();
bool one_time = true;

void setup() {
  Serial.begin(USB_SPEED);
  autoMode.attachSensors(FRONT_TRIG_PIN, FRONT_ECHO_PIN, SIDE_TRIG_PIN,
                         SIDE_ECHO_PIN);
  autoMode.attachSteering(SERVO_PWM_PIN);
  autoMode.attachWheel(VNH_INA_PIN, VNH_INB_PIN, VNH_PWM_PIN);
}

void loop() {
  if (one_time) {
    autoMode.checkSystems();
    one_time = false;
  }
}
