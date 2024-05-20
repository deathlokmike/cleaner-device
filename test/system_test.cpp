#include <Arduino.h>
#include <ESP32Servo.h>
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
int correction = 3;
enum steering_directions { straight = 90, left = 125, right = 55 };
Servo servo = Servo();
bool one_time = true;

void setup() {
  Serial.begin(USB_SPEED);
  servo.attach(SERVO_PWM_PIN);

  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);

  pinMode(SIDE_TRIG_PIN, OUTPUT);
  pinMode(SIDE_ECHO_PIN, INPUT);

  pinMode(VNH_INA_PIN, OUTPUT);
  pinMode(VNH_INB_PIN, OUTPUT);
  pinMode(VNH_PWM_PIN, OUTPUT);
}

void test_turn(steering_directions direction) {
  int correction_direction = direction + correction;
  servo.write(correction_direction);
  delay(15);
}

void test_ultrasonic(uint8_t trig, uint8_t echo) {
  Serial.print("Distance: ");
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  long distance = duration * 0.017;
  Serial.println(String(distance));
}

void loop() {
  if (one_time) {
    test_turn(left);
    delay(500);
    test_turn(right);
    delay(500);
    test_turn(straight);

    one_time = false;
  }
  test_ultrasonic(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
  test_ultrasonic(SIDE_TRIG_PIN, SIDE_ECHO_PIN);
}
