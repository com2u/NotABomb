#ifndef LED_CHAIN_H
#define LED_CHAIN_H

#include <Arduino.h>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct ChainPixel {
    Color color;
    bool active;
};

#endif
