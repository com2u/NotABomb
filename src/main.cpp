#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "TM1638Box.h"
#include "KeypadBox.h"
#include "KeyBox.h"
#include "connection.h"

#define RXD2 16
#define TXD2 17
HardwareSerial Serial2(2);

#define PIN_STB 2
#define PIN_CLK 3
#define PIN_DIO 4

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        13 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 64 // 8x8 LED Matrix

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int red=0;
int green=0;
int blue=0;

// Startup animation state
int startupStep = 0;
unsigned long lastStartupTime = 0;
bool startupComplete = false;

TM1638Box tm1638(PIN_DIO, PIN_CLK, PIN_STB, nullptr);  // Initialize with nullptr, will set connection later
KeypadBox* keypad;
KeyBox* keybox;
Connection* connection;

// Global variables for colorchain state
unsigned long lastMoveTime = 0;
int remainingColorPixels = 0;
int remainingDarkPixels = 0;
bool isColorPhase = true;

// Arrays to store the chain state
uint8_t chainRed[64] = {0};
uint8_t chainGreen[64] = {0};
uint8_t chainBlue[64] = {0};
bool chainActive[64] = {false};

// Current color values
uint8_t currentRed = 0;
uint8_t currentGreen = 0;
uint8_t currentBlue = 0;

// Function to get pixel index from x,y coordinates (0-7)
int getPixelIndex(int x, int y) {
    return y * 8 + x;
}

void updateStartupAnimation() {
    if (startupComplete) return;
    
    if (millis() - lastStartupTime < 50) return;
    lastStartupTime = millis();

    // Clear all pixels
    for(int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    switch(startupStep) {
     case 0 ... 7: { // Top row, left to right (8 LEDs)
            int x = startupStep;
            pixels.setPixelColor(getPixelIndex(x, 0), pixels.Color(255, 0, 0));
            break;
        }
        case 8 ... 14: { // Right column, top to bottom (7 LEDs)
            int y = startupStep - 7;
            pixels.setPixelColor(getPixelIndex(7, y), pixels.Color(0, 255, 0));
            break;
        }
        case 15 ... 21: { // Bottom row, right to left (7 LEDs)
            int x = 7 - (startupStep - 14);
            pixels.setPixelColor(getPixelIndex(x, 7), pixels.Color(0, 0, 255));
            break;
        }
        case 22 ... 27: { // Left column, bottom to top (6 LEDs)
            int y = 7 - (startupStep - 21);
            pixels.setPixelColor(getPixelIndex(0, y), pixels.Color(255, 255, 0));
            break;
        }

        // Second layer - 6x6
        case 28 ... 33: { // Top row (6 LEDs)
            int x = startupStep - 27;
            pixels.setPixelColor(getPixelIndex(x, 1), pixels.Color(255, 0, 255));
            break;
        }
        case 34 ... 38: { // Right column (5 LEDs)
            int y = (startupStep - 33) + 1;
            pixels.setPixelColor(getPixelIndex(6, y), pixels.Color(0, 255, 255));
            break;
        }
        case 39 ... 43: { // Bottom row (5 LEDs)
            int x = 6 - (startupStep - 38);
            pixels.setPixelColor(getPixelIndex(x, 6), pixels.Color(128, 0, 255));
            break;
        }
        case 44 ... 47: { // Left column (4 LEDs)
            int y = 6 - (startupStep - 43);
            pixels.setPixelColor(getPixelIndex(1, y), pixels.Color(255, 128, 0));
            break;
        }

        // Third layer - 4x4
        case 48 ... 51: { // Top row (4 LEDs)
            int x = (startupStep - 47);
            pixels.setPixelColor(getPixelIndex(x + 1, 2), pixels.Color(128, 255, 0));
            break;
        }
        case 52 ... 54: { // Right column (3 LEDs)
            int y = (startupStep - 51) + 2;
            pixels.setPixelColor(getPixelIndex(5, y), pixels.Color(0, 128, 255));
            break;
        }
        case 55 ... 57: { // Bottom row (3 LEDs)
            int x = 5 - (startupStep - 54);
            pixels.setPixelColor(getPixelIndex(x, 5), pixels.Color(255, 0, 128));
            break;
        }
        case 58 ... 59: { // Left column (2 LEDs)
            int y = 5 - (startupStep - 57);
            pixels.setPixelColor(getPixelIndex(2, y), pixels.Color(128, 255, 128));
            break;
        }

        // Fourth (innermost) layer - 2x2
        case 60: { // Top row (1 LED)
            pixels.setPixelColor(getPixelIndex(3, 3), pixels.Color(255, 128, 128));
            break;
        }
        case 61: { // Right column (1 LED)
            pixels.setPixelColor(getPixelIndex(4, 3), pixels.Color(128, 128, 255));
            break;
        }
        case 62: { // Bottom row (1 LED)
            pixels.setPixelColor(getPixelIndex(4, 4), pixels.Color(255, 255, 128));
            break;
        }
        case 63: { // Left column (1 LED) - completes the pattern
            pixels.setPixelColor(getPixelIndex(3, 4), pixels.Color(128, 255, 255));
            break;
        }
       
        case 64 ... 79: { // Final pulse effect
            int brightness = (startupStep - 64) < 8 ? 
                           (startupStep - 64) * 32 : 
                           (15 - (startupStep - 64)) * 32;
            for(int i = 0; i < NUMPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));
            }
            break;
        }
        default: {
            startupComplete = true;
            // Clear all pixels for the next animation
            for(int i = 0; i < NUMPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            }
            break;
        }
    }
    
    pixels.show();
    startupStep++;
}

void setRandomColor() {
    int colorChoice = random(4);  // 0-3 for four colors
    switch(colorChoice) {
        case 0:  // Red
            currentRed = 255;
            currentGreen = 0;
            currentBlue = 0;
            break;
        case 1:  // Green
            currentRed = 0;
            currentGreen = 255;
            currentBlue = 0;
            break;
        case 2:  // Blue
            currentRed = 0;
            currentGreen = 0;
            currentBlue = 255;
            break;
        default:  // Yellow
            currentRed = 255;
            currentGreen = 255;
            currentBlue = 0;
            break;
    }
}

void moveChainForward() {
    // Move all pixels forward
    for(int i = NUMPIXELS - 1; i > 0; i--) {
        chainRed[i] = chainRed[i-1];
        chainGreen[i] = chainGreen[i-1];
        chainBlue[i] = chainBlue[i-1];
        chainActive[i] = chainActive[i-1];
    }
    
    // Clear the first pixel
    chainRed[0] = 0;
    chainGreen[0] = 0;
    chainBlue[0] = 0;
    chainActive[0] = false;
    
    // Update physical LEDs
    for(int i = 0; i < NUMPIXELS; i++) {
        if(chainActive[i]) {
            pixels.setPixelColor(i, pixels.Color(chainRed[i], chainGreen[i], chainBlue[i]));
        } else {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
    }
    pixels.show();
}

void updateColorChain() {
    if (!startupComplete) return;  // Don't start chain until startup animation is done
    
    if (millis() - lastMoveTime < 50) {
        return;  // Not time to move yet
    }
    
    lastMoveTime = millis();

    if (isColorPhase) {
        if (remainingColorPixels == 0) {
            // Start new color sequence
            setRandomColor();
            remainingColorPixels = random(5, 16);  // 5-15
        }
        
        // Add a colored pixel
        chainRed[0] = currentRed;
        chainGreen[0] = currentGreen;
        chainBlue[0] = currentBlue;
        chainActive[0] = true;
        moveChainForward();
        remainingColorPixels--;
        
        if (remainingColorPixels == 0) {
            isColorPhase = false;
            remainingDarkPixels = random(16, 35);  // 16-34
        }
    } else {
        if (remainingDarkPixels == 0) {
            isColorPhase = true;
            return;
        }
        
        // Add a dark pixel
        chainRed[0] = 0;
        chainGreen[0] = 0;
        chainBlue[0] = 0;
        chainActive[0] = false;
        moveChainForward();
        remainingDarkPixels--;
    }
}

void binaryLEDText(String text) {
    // Clear all pixels first
    for(int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }

    // Process up to 8 characters
    int maxChars = (text.length() < 8) ? text.length() : 8;
    
    // For each character
    for(int charPos = 0; charPos < maxChars; charPos++) {
        char c = text.charAt(charPos);
        
        // Get binary representation of ASCII value
        for(int bit = 0; bit < 8; bit++) {
            // Check if bit is set (1) or not (0)
            bool isSet = (c & (1 << bit)) != 0;
            
            if(isSet) {
                // Light up corresponding LED
                // charPos is the x coordinate (0-7)
                // bit is the y coordinate (0-7)
                int pixelIndex = getPixelIndex(charPos, bit);
                pixels.setPixelColor(pixelIndex, pixels.Color(red, green, blue));
            }
        }
    }
    
    // Update the LED matrix
    pixels.show();
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    pixels.begin();
    randomSeed(analogRead(0));  // Initialize random number generator
    
    connection = new Connection(pixels);
    connection->begin();

    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif

    tm1638.begin();
    tm1638.startCountdown(1, 0, 0);

    keypad = new KeypadBox(tm1638, pixels, red, green, blue, connection);
    keybox = new KeyBox();
    keybox->init();

    // Initialize all pixels to off
    for(int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();

    Serial.println(F("Setup done"));
}

void loop() {
    connection->handle();
    
    tm1638.update();
    keypad->handle();
    
    if(Serial2.available()){
        Serial.write(".");
        Serial.write(Serial2.read());  
    }
    
    keybox->handle();
    
    updateStartupAnimation();  // Run startup animation first
    updateColorChain();       // Then run color chain when startup is complete
    
    delay(10); 
}
