#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct ChainPixel {
    Color color;
    bool active;
};

class LEDMatrix {
public:
    LEDMatrix(uint8_t pin = 13, uint16_t numPixels = 64);
    void begin();
    void handle();
    void clear();
    void show();
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void binaryLEDText(String text);
    bool isStartupComplete() const { return startupComplete; }
    Adafruit_NeoPixel& getPixels() { return pixels; }
    int& getRed() { return red; }
    int& getGreen() { return green; }
    int& getBlue() { return blue; }

private:
    static const int DELAYVAL = 500;
    static const int NUM_PIXELS = 64;
    
    Adafruit_NeoPixel pixels;
    
    // Color variables for external use (KeypadBox)
    int red = 0;
    int green = 0;
    int blue = 0;

    // Startup animation state
    int startupStep = 0;
    unsigned long lastStartupTime = 0;
    bool startupComplete = false;

    // Color chain state
    unsigned long lastMoveTime = 0;
    int remainingColorPixels = 0;
    int remainingDarkPixels = 0;
    bool isColorPhase = true;

    // Chain state using the new structures
    ChainPixel chain[NUM_PIXELS];
    Color currentChainColor;

    // Private methods
    int getPixelIndex(int x, int y);
    void updateStartupAnimation();
    void setRandomColor();
    void moveChainForward();
    void updateColorChain();
    void updatePixelFromChain(int index);
};

#endif
