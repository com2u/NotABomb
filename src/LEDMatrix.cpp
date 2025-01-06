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

String LEDMatrix::getMode() const {
    switch (currentMode) {
        case MatrixMode::COLOR_CHAIN:
            return "ColorChain";
        case MatrixMode::SIMON_SAYS:
            return "SimonSays";
        case MatrixMode::MAZE:
            return "Maze";
        case MatrixMode::STARTUP:
            return "Startup";
        default:
            return "Unknown";
    }
}

void LEDMatrix::begin() {
    pixels.begin();
    clear();
}

void LEDMatrix::handle() {
    switch (currentMode) {
        case MatrixMode::STARTUP:
            updateStartupAnimation();
            break;
        case MatrixMode::COLOR_CHAIN:
            if (startupComplete) {
                updateColorChain();
            }
            break;
        case MatrixMode::SIMON_SAYS:
            // Simon Says mode just displays the pattern, no animation updates needed
            break;
        case MatrixMode::MAZE:
            updateMazeDisplay();
            break;
    }
}

void LEDMatrix::setMode(MatrixMode newMode) {
    if (newMode == currentMode) return;
    
    currentMode = newMode;
    clear(); // Clear display when changing modes
    
    if (newMode == MatrixMode::SIMON_SAYS) {
        Serial.println((String) "LEDMatrix::setMode initSimonSays"); 
        initSimonSays();
    } else if (newMode == MatrixMode::COLOR_CHAIN) {
        Serial.println((String) "LEDMatrix::setMode ColorChain"); 
        updateColorChain();
    } else if (newMode == MatrixMode::MAZE) {
        Serial.println((String) "LEDMatrix::setMode Maze"); 
        initMaze();
    }
}

void LEDMatrix::initSimonSays() {
    String colorcode = generateRandomQuadrantColors();
    displaySimonSaysPattern();
}

String LEDMatrix::generateRandomQuadrantColors() {
    // Define the four possible colors
    const Color colors[] = {
        {255, 0, 0},    // Red
        {0, 255, 0},    // Green
        {255, 255, 0},  // Yellow
        {0, 0, 255}     // Blue
    };
    
    // Create a temporary array to track used colors
    bool usedColors[4] = {false, false, false, false};
    String colorcode = "";
    // Assign random unique colors to each quadrant
    for (int i = 0; i < 4; i++) {
        int colorIndex;
        do {
            colorIndex = random(4);
        } while (usedColors[colorIndex]);
        Serial.println((String) "SinonSays Color:"+colorIndex); 
        colorcode = String(colorcode + char(49+colorIndex));
        Serial.println((String) "SinonSays code:"+colorcode); 
        usedColors[colorIndex] = true;
        quadrantColors[i] = colors[colorIndex];
    }
    return colorcode;
}

void LEDMatrix::setQuadrantColor(int quadrant, const Color& color) {
    int startX = (quadrant % 2) * 4;
    int startY = (quadrant / 2) * 4;
    
    // Fill the 4x4 quadrant with the specified color
    for (int y = startY; y < startY + 4; y++) {
        for (int x = startX; x < startX + 4; x++) {
            pixels.setPixelColor(getPixelIndex(x, y), pixels.Color(color.r, color.g, color.b));
        }
    }
}

void LEDMatrix::displaySimonSaysPattern() {
    clear();
    
    // Set each quadrant's color
    for (int i = 0; i < 4; i++) {
        setQuadrantColor(i, quadrantColors[i]);
    }
    
    show();
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
            currentMode = MatrixMode::COLOR_CHAIN;
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

// New Maze mode methods
void LEDMatrix::initMaze() {
    generateMazePoints();
    updateMazeDisplay();
}

void LEDMatrix::generateMazePoints() {
    // Generate random start position
    playerPos.x = random(MATRIX_SIZE);
    playerPos.y = random(MATRIX_SIZE);
    
    // Generate random target position (different from start)
    do {
        targetPos.x = random(MATRIX_SIZE);
        targetPos.y = random(MATRIX_SIZE);
    } while (targetPos.x == playerPos.x && targetPos.y == playerPos.y);
    
    playerVisible = true;
    lastBlinkTime = millis();
}

bool LEDMatrix::isValidPosition(int x, int y) const {
    return x >= 0 && x < MATRIX_SIZE && y >= 0 && y < MATRIX_SIZE;
}

void LEDMatrix::updateMazeDisplay() {
    if (currentMode != MatrixMode::MAZE) return;
    
    // Handle player blinking
    if (millis() - lastBlinkTime >= 500) { // Blink every 500ms
        playerVisible = !playerVisible;
        lastBlinkTime = millis();
    }
    
    // Clear the display
    clear();
    
    // Draw target (blue)
    pixels.setPixelColor(getPixelIndex(targetPos.x, targetPos.y), pixels.Color(0, 0, 255));
    
    // Draw player (blinking green)
    if (playerVisible) {
        pixels.setPixelColor(getPixelIndex(playerPos.x, playerPos.y), pixels.Color(0, 255, 0));
    }
    
    pixels.show();
}

bool LEDMatrix::movePlayer(MazeDirection direction) {
    if (currentMode != MatrixMode::MAZE) return false;
    
    int newX = playerPos.x;
    int newY = playerPos.y;
    
    switch (direction) {
        case MazeDirection::UP:
            newY--;
            break;
        case MazeDirection::DOWN:
            newY++;
            break;
        case MazeDirection::LEFT:
            newX--;
            break;
        case MazeDirection::RIGHT:
            newX++;
            break;
    }
    
    if (!isValidPosition(newX, newY)) {
        return false;
    }
    
    playerPos.x = newX;
    playerPos.y = newY;
    playerVisible = true; // Make player visible immediately after move
    lastBlinkTime = millis();
    updateMazeDisplay();
    
    return true;
}

bool LEDMatrix::isMazeComplete() const {
    return playerPos.x == targetPos.x && playerPos.y == targetPos.y;
}
