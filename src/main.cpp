#include <Arduino.h>
#include "TM1638Box.h"
#include "KeypadBox.h"
#include "KeyBox.h"
#include "connection.h"
#include "LEDMatrix.h"

#define RXD2 16
#define TXD2 17
HardwareSerial Serial2(2);

#define PIN_STB 2
#define PIN_CLK 3
#define PIN_DIO 4

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

LEDMatrix ledMatrix;
TM1638Box tm1638(PIN_DIO, PIN_CLK, PIN_STB, nullptr);  // Initialize with nullptr, will set connection later
KeypadBox* keypad;
KeyBox* keybox;
Connection* connection;

// Store color variables
int ledRed = 0;
int ledGreen = 0;
int ledBlue = 0;

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    ledMatrix.begin();
    randomSeed(analogRead(0));  // Initialize random number generator
    
    connection = new Connection(ledMatrix.getPixels(), ledMatrix);
    connection->begin();

    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
    #endif

    tm1638.begin();
    tm1638.startCountdown(1, 0, 0);

    keypad = new KeypadBox(tm1638, ledMatrix.getPixels(), ledRed, ledGreen, ledBlue, connection);
    keybox = new KeyBox(connection);
    keybox->init();

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
    ledMatrix.handle();
    
    delay(10); 
}
