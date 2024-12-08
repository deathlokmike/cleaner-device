#pragma once
#include <Arduino.h>

enum wheel_speed : uint8_t {
    forward = 190,
    backward = 128,
    turn = forward - 125
};

class WheelControl {
   private:
    uint8_t in1;
    uint8_t in2;
    uint8_t in3;
    uint8_t in4;

    void setup();

   public:
    WheelControl();
    void attach(uint8_t in1_, uint8_t in2_, uint8_t in3_, uint8_t in4_);
    void forward();
    void backward();
    void left();
    void right();
    void stop();
};