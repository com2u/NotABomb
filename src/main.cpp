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
#define NUMPIXELS 16 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

int red=0;
int green=0;
int blue=0;

TM1638Box tm1638(PIN_DIO, PIN_CLK, PIN_STB, nullptr);  // Initialize with nullptr, will set connection later
KeypadBox* keypad;
KeyBox* keybox;
Connection* connection;

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    pixels.begin();
    
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
    delay(10); 
}
