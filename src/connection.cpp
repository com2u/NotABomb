#include "connection.h"
#include <Arduino.h>

Connection::Connection() : client(espClient) {
}

void Connection::begin() {
    setupWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    
    if (!client.connected()) {
        reconnectMQTT();
    }
}

void Connection::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void Connection::reconnectMQTT() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);
        
        // Attempt to connect
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("NotABomb/Key/init", "Startup");
            // ... and subscribe to topics
            client.subscribe("notabomb/control");
            client.subscribe("NotABomb/CYD/#");
            Serial.println("Subscribed to NotABomb/CYD/#");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void Connection::callback(char* topic, byte* payload, unsigned int length) {
    // Handle incoming messages here
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void Connection::handle() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}

PubSubClient* Connection::getMQTTClient() {
    return &client;
}
