#include "LEDMatrix.h"

LEDMatrix::LEDMatrix(uint8_t pin, uint16_t numPixels)
    : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800) {
    // Initialize chain pixels
    for (int i = 0; i < NUM_PIXELS; i++) {
        chain[i].color = {0, 0, 0};
        chain[i].active = false;
    }
    currentChainColor = {0, 0, 0};
}

void LEDMatrix::begin() {
    pixels.begin();
    clear();
}

void LEDMatrix::handle() {
    updateStartupAnimation();
    updateColorChain();
}

void LEDMatrix::clear() {
    for(int i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
}

void LEDMatrix::show() {
    pixels.show();
}

void LEDMatrix::setColor(uint8_t r, uint8_t g, uint8_t b) {
    red = r;
    green = g;
    blue = b;
}

bool LEDMatrix::checkLEDChain(uint8_t r_ref, uint8_t g_ref, uint8_t b_ref) {
    
            // Check all pixels in the matrix for blue color
    for(int i = 0; i < 64; i++) {
        uint32_t color = pixels.getPixelColor(i);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        Serial.print((String) "#"+i+ "rgb"+r+","+g+","+b);
        // Check if pixel is blue (0,0,255)
        if (r > r_ref && g > g_ref && b >= b_ref) {
            return true;
        }
    }
    return false;
}

void LEDMatrix::updatePixelFromChain(int index) {
    if (chain[index].active) {
        pixels.setPixelColor(index, pixels.Color(
            chain[index].color.r,
            chain[index].color.g,
            chain[index].color.b
        ));
    } else {
        pixels.setPixelColor(index, pixels.Color(0, 0, 0));
    }
}

int LEDMatrix::getPixelIndex(int x, int y) {
    return y * 8 + x;
}

void LEDMatrix::updateStartupAnimation() {
    if (startupComplete) return;
    
    if (millis() - lastStartupTime < 50) return;
    lastStartupTime = millis();

    // Clear all pixels
    clear();
    
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
            for(int i = 0; i < pixels.numPixels(); i++) {
                pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));
            }
            break;
        }
        default: {
            startupComplete = true;
            clear();
            break;
        }
    }
    
    pixels.show();
    startupStep++;
}

void LEDMatrix::setRandomColor() {
    int colorChoice = random(4);  // 0-3 for four colors
    switch(colorChoice) {
        case 0:  // Red
            currentChainColor = {255, 0, 0};
            break;
        case 1:  // Green
            currentChainColor = {0, 255, 0};
            break;
        case 2:  // Blue
            currentChainColor = {0, 0, 255};
            break;
        default:  // Yellow
            currentChainColor = {255, 255, 0};
            break;
    }
}

void LEDMatrix::moveChainForward() {
    // Move all pixels forward
    for(int i = NUM_PIXELS - 1; i > 0; i--) {
        chain[i] = chain[i-1];
    }
    
    // Clear the first pixel
    chain[0].color = {0, 0, 0};
    chain[0].active = false;
    
    // Update physical LEDs
    for(int i = 0; i < NUM_PIXELS; i++) {
        updatePixelFromChain(i);
    }
    pixels.show();
}

void LEDMatrix::updateColorChain() {
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
        chain[0].color = currentChainColor;
        chain[0].active = true;
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
        chain[0].color = {0, 0, 0};
        chain[0].active = false;
        moveChainForward();
        remainingDarkPixels--;
    }
}

void LEDMatrix::binaryLEDText(String text) {
    clear();

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
    
    pixels.show();
}
