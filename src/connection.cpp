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
    client.setServer(mqtt_server.c_str(), mqttPort);
    client.setCallback(staticCallback);
    
    if (!client.connected()) {
        reconnectMQTT();
    }
}

boolean Connection::connectWIFI(const char* wifi_ssid, const char* password, int retries ){
  Serial.print("Connecting ");
  Serial.println(wifi_ssid);
  delay(100);
  WiFi.begin(wifi_ssid, password);
  String clientName =  "NotABomb";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String result;
  for (int i = 4; i < 6; ++i) {
    result += String(mac[i], 16);
  }
  clientName += result;
  char host[clientName.length() + 1];
  clientName.toCharArray(host, clientName.length() + 1);
  for (int retry = 0; retry < retries; retry++) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.hostname(host);
      break;
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected as ");
    Serial.println(host);
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  return false;
}

void Connection::scanWIFINetwork() {
  Serial.println((String) "WIFI scan start "+millis());

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println((String) "WIFI scan done "+millis());
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      int networkNo = 0;
      while (networkNo < NUM_NETWORKS) {
        if (WiFi.SSID(i) == ssid[networkNo]){
            Serial.print(" Home Network found: ");
            if (connectWIFI(ssid[networkNo], password[networkNo], 10)){
              return;
            }
        }
        networkNo++;
      }
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(500);
}

void Connection::setupWiFi() {
  //connect to WiFi
  int networkNo = 0;
  int retries = 12;
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    // Try several WIFI Networks
    while (networkNo < NUM_NETWORKS) {
      if (connectWIFI(ssid[networkNo], password[networkNo], retries)){
        break;
      }
      networkNo++;
    }
  }
}

void Connection::reconnectMQTT() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "NotABomb-";
        clientId += String(random(0xffff), HEX);
        
        // Attempt to connect
        if (client.connect(clientId.c_str(), MQTTUser.c_str(), MQTTPassword.c_str())) {
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
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    // Create a string from the payload
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Check if this is a mode change message
    if (String(topic).indexOf("NotABomb/CYD/Mode") != -1) {
        if (message.indexOf("SimonSay") != -1) {
            ledMatrix.setMode(MatrixMode::SIMON_SAYS);
            Serial.println("Switching to Simon Says mode");
        } else if (message.indexOf("ColorChai") != -1) {
            ledMatrix.setMode(MatrixMode::COLOR_CHAIN);
            Serial.println("Switching to Color Chain mode");
        } else if (message.indexOf("Maz") != -1) {
            ledMatrix.setMode(MatrixMode::MAZE);
            Serial.println("Switching to Maze mode");
        }
    }
    delay(500);
    // Set LED color when message received (keep existing functionality)
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
