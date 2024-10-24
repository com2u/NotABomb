#include "KeypadBox.h"

KeypadBox::KeypadBox(TM1638Box& tm, Adafruit_NeoPixel& px, int& r, int& g, int& b, Connection* conn)
    : tm1638(tm), 
      pixels(px), 
      red(r), 
      green(g), 
      blue(b),
      connection(conn),
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
        
        switch(button) {
            case '1': 
                tm1638.setLEDs(1);
                pixels.setPixelColor(1, pixels.Color(red, green, blue));
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Key/Key1", "1");
                    connection->getMQTTClient()->flush();
                    Serial.println("Key1 send via MQTT");
                }
                break;
            case '2':
                tm1638.setLEDs(2);
                pixels.setPixelColor(2, pixels.Color(red, green, blue));
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Key/Key2", "1");
                    connection->getMQTTClient()->flush();
                    Serial.println("Key2 send via MQTT");
                }
                break;
            case '3':
                tm1638.setLEDs(4);
                pixels.setPixelColor(3, pixels.Color(red, green, blue));
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Key/Key3", "1");
                    connection->getMQTTClient()->flush();
                    Serial.println("Key3 send via MQTT");
                }
                break;
            case '4':
                tm1638.setLEDs(8);
                pixels.setPixelColor(4, pixels.Color(red, green, blue));
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Key/Key4", "1");
                    connection->getMQTTClient()->flush();
                    Serial.println("Key4 send via MQTT");
                }
                break;
            case '5':
                tm1638.setLEDs(16);
                pixels.setPixelColor(5, pixels.Color(red, green, blue));
                if (connection && connection->getMQTTClient()) {
                    connection->getMQTTClient()->publish("NotABomb/Key/Key5", "1");
                    connection->getMQTTClient()->flush();
                    Serial.println("Key5 send via MQTT");
                }
                break;
            case '6':
                tm1638.setLEDs(32);
                pixels.setPixelColor(6, pixels.Color(red, green, blue));
                break;
            case '7':
                tm1638.setLEDs(64);
                pixels.setPixelColor(7, pixels.Color(red, green, blue));
                break;
            case '8':
                tm1638.setLEDs(128);
                pixels.setPixelColor(8, pixels.Color(red, green, blue));
                break;
            case 'A':
                red=128; green=0; blue=0;
                break;
            case 'B':
                red=0; green=128; blue=0;
                break;
            case 'C':
                red=0; green=0; blue=128;
                break;
            case 'D':
                red=128; green=128; blue=0;
                break;
            case '.':
                red=0; green=0; blue=0;
                for(int i=0; i<16; i++) {
                    pixels.setPixelColor(i, pixels.Color(red, green, blue));
                }
                break;
            case '#':
                red=128; green=128; blue=128;
                for(int i=0; i<16; i++) {
                    pixels.setPixelColor(i, pixels.Color(red, green, blue));
                }
                break;
        }
        pixels.show();
    }
}
