#ifndef CONNECTION_H
#define CONNECTION_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

class Connection {
    public:
        Connection(Adafruit_NeoPixel& px);
        void begin();
        void handle();
        PubSubClient* getMQTTClient();

    private:
        const char* ssid = "Com2u.de.WLAN2";
        const char* password = "SOMMERREGEN05";
        const char* mqtt_server = "116.203.60.216";
        const char* mqtt_user = "MQTTiot";
        const char* mqtt_password = "iot6812";
        const int mqtt_port = 1883;

        WiFiClient espClient;
        PubSubClient client;
        Adafruit_NeoPixel& pixels;
        
        void setupWiFi();
        void reconnectMQTT();
        void callback(char* topic, byte* payload, unsigned int length);
        static void staticCallback(char* topic, byte* payload, unsigned int length);
        static Connection* instance;
};

#endif
