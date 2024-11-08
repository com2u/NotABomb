#ifndef CONNECTION_H
#define CONNECTION_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "LEDMatrix.h"
//#include <Credentials.h>

class Connection {
    public:
        Connection(Adafruit_NeoPixel& px, LEDMatrix& matrix);
        void begin();
        void handle();
        PubSubClient* getMQTTClient();
        LEDMatrix& getLEDMatrix() { return ledMatrix; }

    private:
        // WIFI Networks configuration
        /*
        const char* networks[16] = {
            "Com2u.de.WLAN2", "Com2u.de.WLAN", "Com2uRedmi11", "KPMS-Openhouse",
            "Vodafone-BE2C", "RobsTest", "HessCom2u", "HHLink",
            "muccc.legacy-2.4GHz", "muenchen.freifunk.net", "Cafeteria", "Free_WIFI",
            "WLANESP", "muenchen.freifunk.net/muc_cty", "KPMS-Openhouse", "KPMS-Cafeteria"
        };
        const char* passwords[16] = {
            "SOMMERREGEN05", "SOMMERREGEN05", "SOMMERREGEN05", "OpenHouse",
            "q49adKnc4bPka7bp", "Schiller12", "SOMMERREGEN05", "SOMMERREGEN05",
            "haileris", "", "Cafeteria", "",
            "Schiller", "", "OpenHouse", "KPMS-Cafeteria-2022"
        };
        static const int NUM_NETWORKS = 16;
        */

        const char* mqttserver = "116.203.60.216";
        const char* mqtt_user = "MQTTiot";
        const char* mqtt_password = "iot6812";
        const int mqtt_port = 1883;

        
        WiFiClient espClient;
        PubSubClient client;
        Adafruit_NeoPixel& pixels;
        LEDMatrix& ledMatrix;
        
        void setupWiFi();
        void reconnectMQTT();
        void callback(char* topic, byte* payload, unsigned int length);
        static void staticCallback(char* topic, byte* payload, unsigned int length);
        static Connection* instance;
};

#endif
