#include "KeypadBox.h"
#include "LEDMatrix.h"

KeypadBox::KeypadBox(TM1638Box& tm, Adafruit_NeoPixel& px, int& r, int& g, int& b, Connection* conn, LEDMatrix& matrix)
    : tm1638(tm), 
      pixels(px), 
      red(r), 
      green(g), 
      blue(b),
      connection(conn),
      ledMatrix(matrix),
      customKeypad(makeKeymap((char*)hexaKeys), rowPins, colPins, ROWS, COLS) {
    
    // Initialize the keypad matrix
    char tempKeys[ROWS][COLS] = {
        { '1', '2', '3', 'A' },
        { '4', '5', '6', 'B' },
        { '7', '8', '9', 'C' },
        { '.', '0', '#', 'D' }
    };
    memcpy(hexaKeys, tempKeys, sizeof(hexaKeys));

    // Initialize pins
    byte tempColPins[ROWS] = { 5, 6, 7, 8 };
    byte tempRowPins[COLS] = { 9, 10, 11, 12 };
    memcpy(colPins, tempColPins, sizeof(colPins));
    memcpy(rowPins, tempRowPins, sizeof(rowPins));

    // Re-initialize keypad with updated values
    customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
}

void KeypadBox::handle() {
    char button = customKeypad.getKey();
    if (button) {
        Serial.print(button);
        char text[17];
        ltoa(button, text, 10);
        tm1638.clearDisplay();   
        tm1638.setDisplayString(text);
        
        if (ledMatrix.getMode() == "Maze") {
            bool moved = false;
            if (button == '2') { // Up
                moved = ledMatrix.movePlayer(MazeDirection::UP);
            } else if (button == '8') { // Down
                moved = ledMatrix.movePlayer(MazeDirection::DOWN);
            } else if (button == '4') { // Left
                moved = ledMatrix.movePlayer(MazeDirection::LEFT);
            } else if (button == '6') { // Right
                moved = ledMatrix.movePlayer(MazeDirection::RIGHT);
            }
            
            // Check if move was successful and if maze is complete
            if (moved && ledMatrix.isMazeComplete()) {
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Challenge", "passed");
                    Serial.println("Maze completed!");
                }
            }
        } else {
            if (connection && connection->getMQTTClient()) {
                switch(button) {
                    case '1': 
                        tm1638.setLEDs(1);
                        pixels.setPixelColor(1, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "1");
                        Serial.println("Key1 send via MQTT");
                        break;
                    case '2':
                        tm1638.setLEDs(2);
                        pixels.setPixelColor(2, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "2");
                        Serial.println("Key2 send via MQTT");
                        break;
                    case '3':
                        tm1638.setLEDs(4);
                        pixels.setPixelColor(3, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "3");
                        Serial.println("Key3 send via MQTT");
                        break;
                    case '4':
                        tm1638.setLEDs(8);
                        pixels.setPixelColor(4, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "4");
                        Serial.println("Key4 send via MQTT");
                        break;
                    case '5':
                        tm1638.setLEDs(16);
                        pixels.setPixelColor(5, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "5");
                        Serial.println("Key5 send via MQTT");
                        break;
                    case '6':
                        tm1638.setLEDs(32);
                        pixels.setPixelColor(6, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "6");
                        break;
                    case '7':
                        tm1638.setLEDs(64);
                        pixels.setPixelColor(7, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "7");
                        break;
                    case '8':
                        tm1638.setLEDs(128);
                        pixels.setPixelColor(8, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "8");
                        break;
                    case '9':
                        pixels.setPixelColor(9, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "9");
                        break;
                    case '0':
                        pixels.setPixelColor(0, pixels.Color(red, green, blue));
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "0");
                        break;
                    case 'A':
                        red=128; green=0; blue=0;
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "A");
                        break;
                    case 'B':
                        red=0; green=128; blue=0;
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "B");
                        break;
                    case 'C':
                        red=0; green=0; blue=128;
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "C");
                        break;
                    case 'D':
                        red=128; green=128; blue=0;
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "D");
                        break;
                    case '.':
                        red=0; green=0; blue=0;
                        for(int i=0; i<16; i++) {
                            pixels.setPixelColor(i, pixels.Color(red, green, blue));
                        }
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "*");
                        break;
                    case '#':
                        red=128; green=128; blue=128;
                        for(int i=0; i<16; i++) {
                            pixels.setPixelColor(i, pixels.Color(red, green, blue));
                        }
                        connection->getMQTTClient()->publish("NotABomb/Key/Keypad", "#");
                        break;
                }
                connection->getMQTTClient()->flush();
            }
        }
        pixels.show();
    }
}
