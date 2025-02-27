#pragma once
#include <Arduino.h>

enum wheel_speed : uint8_t {
    forward = 190,
    backward = 130,
    turn = forward - 120
};

class WheelControl {
   private:
    uint8_t in1;
    uint8_t in2;
    uint8_t in3;
    uint8_t in4;
    uint8_t speed = -1;

    void smoothControl(uint8_t gpio1, uint8_t gpio2, bool isStarting);
    void setup();

   public:
    WheelControl();
    void attach(uint8_t in1_, uint8_t in2_, uint8_t in3_, uint8_t in4_);
    void forward();
    void backward();
    void left();
    void stop();
};