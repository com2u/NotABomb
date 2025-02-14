#ifndef CONNECTION_H
#define CONNECTION_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "LEDMatrix.h"
#include <Credentials.h>

class Connection {
    public:
        Connection(Adafruit_NeoPixel& px, LEDMatrix& matrix);
        void begin();
        void handle();
        PubSubClient* getMQTTClient();
        LEDMatrix& getLEDMatrix() { return ledMatrix; }

    private:        
        WiFiClient espClient;
        PubSubClient client;
        Adafruit_NeoPixel& pixels;
        LEDMatrix& ledMatrix;
        
        void setupWiFi();
        static void scanWIFINetwork();
        static boolean connectWIFI(const char* wifi_ssid, const char* password, int retries);
        void reconnectMQTT();
        void callback(char* topic, byte* payload, unsigned int length);
        static void staticCallback(char* topic, byte* payload, unsigned int length);
        static Connection* instance;
};

#endif
