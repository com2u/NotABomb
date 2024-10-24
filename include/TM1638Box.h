#ifndef TM1638BOX_H
#define TM1638BOX_H

#include <Arduino.h>
#include <TM1638.h>
#include <Adafruit_GFX.h> 
#include <TM16xxDisplay.h>
#include <TM16xxButtons.h>
#include "connection.h"

class TM1638Box {
public:
    TM1638Box(uint8_t dio_pin, uint8_t clk_pin, uint8_t stb_pin, Connection* conn);
    void begin();
    void update();
    void startCountdown(int hours, int minutes, int seconds);
    void updateCountdown();
    void clearDisplay();
    void setDisplayString(const char* text);
    void setLEDs(uint8_t value);

private:
    TM1638* module;
    TM16xxButtons* buttons;
    TM16xxDisplay* display;
    Connection* connection;
    
    unsigned long countdownTarget = 0;
    bool countdownRunning = false;

    static void fnRelease(byte nButton);
    static void fnClick(byte nButton);
    static void fnDoubleclick(byte nButton);
    static void fnLongPressStart(byte nButton);
    static void fnLongPress(byte nButton);
    static void fnLongPressStop(byte nButton);

    static TM1638Box* instance;
    void handleButtons();
};

#endif
