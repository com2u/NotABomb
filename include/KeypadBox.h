#ifndef KEYPADBOX_H
#define KEYPADBOX_H

#include <Arduino.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include "TM1638Box.h"

class KeypadBox {
private:
    static const byte ROWS = 4;
    static const byte COLS = 4;
    char hexaKeys[ROWS][COLS];
    byte colPins[ROWS];
    byte rowPins[COLS];
    Keypad customKeypad;
    TM1638Box& tm1638;
    Adafruit_NeoPixel& pixels;
    int& red;
    int& green;
    int& blue;

public:
    KeypadBox(TM1638Box& tm, Adafruit_NeoPixel& px, int& r, int& g, int& b);
    void handle();
};

#endif
