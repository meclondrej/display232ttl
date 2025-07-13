#include <Arduino.h>
#include "PinConfig.hpp"

void setup() {
    pinMode(PIN_OE, OUTPUT);
    pinMode(PIN_RCLK, OUTPUT);
    pinMode(PIN_SRCLR, OUTPUT);
    pinMode(PIN_SRCLK, OUTPUT);
    pinMode(PIN_SER, OUTPUT);
}

void loop() {}
