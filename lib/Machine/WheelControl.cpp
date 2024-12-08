#include "WheelControl.h"

WheelControl::WheelControl() {};

void WheelControl::attach(uint8_t in1_, uint8_t in2_, uint8_t in3_,
                          uint8_t in4_) {
    in1 = in1_;
    in2 = in2_;
    in3 = in3_;
    in4 = in4_;
    setup();
}

void WheelControl::setup() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
}

void WheelControl::backward() {
    analogWrite(in1, wheel_speed::backward);
    analogWrite(in2, 0);
    analogWrite(in3, wheel_speed::backward);
    analogWrite(in4, 0);
}

void WheelControl::forward() {
    analogWrite(in1, 0);
    analogWrite(in2, wheel_speed::forward);
    delay(6);  // compensation for out-of-sync
    analogWrite(in3, 0);
    analogWrite(in4, wheel_speed::forward);
}

void WheelControl::left() {
    analogWrite(in1, wheel_speed::turn);
    analogWrite(in2, 0);
    analogWrite(in3, 0);
    analogWrite(in4, wheel_speed::turn);
}

void WheelControl::right() {
    analogWrite(in1, 0);
    analogWrite(in2, wheel_speed::turn);
    analogWrite(in3, wheel_speed::turn);
    analogWrite(in4, 0);
}

void WheelControl::stop() {
    analogWrite(in1, 0);
    analogWrite(in2, 0);
    analogWrite(in3, 0);
    analogWrite(in4, 0);
}