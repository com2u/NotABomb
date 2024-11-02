#include "connection.h"
#include <Arduino.h>

Connection* Connection::instance = nullptr;

Connection::Connection(Adafruit_NeoPixel& px, LEDMatrix& matrix) 
    : client(espClient), 
      pixels(px),
      ledMatrix(matrix) {
    instance = this;
}

void Connection::begin() {
    setupWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(staticCallback);
    
    if (!client.connected()) {
        reconnectMQTT();
    }
}

void Connection::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print((String) "Connecting to WiFi "+ssid);
    
    while (WiFi.status() != WL_CONNECTED) {
        ledMatrix.handle(); // This will call updateStartupAnimation
        delay(50); // Reduced delay to allow more frequent animation updates
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
        String clientId = "NotABomb-";
        clientId += String(random(0xffff), HEX);
        
        // Attempt to connect
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("NotABomb/Key/init", "Startup");
            // ... and subscribe to topics
            client.subscribe("NotABomb/Key/#");
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

void Connection::staticCallback(char* topic, byte* payload, unsigned int length) {
    if (instance) {
        instance->callback(topic, payload, length);
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

    // Set LED color when message received
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.show();
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
