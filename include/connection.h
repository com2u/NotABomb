#ifndef CONNECTION_H
#define CONNECTION_H

#include <WiFi.h>
#include <PubSubClient.h>

class Connection {
    public:
        Connection();
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
        
        void setupWiFi();
        void reconnectMQTT();
        static void callback(char* topic, byte* payload, unsigned int length);
};

#endif
