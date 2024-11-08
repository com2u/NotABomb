#include "connection.h"
#include <Arduino.h>

// Define the WiFi networks
const char* ssid[] =     { "Com2u.de.WLAN2","Com2u.de.WLAN","Com2uRedmi11","KPMS-Openhouse", "Vodafone-BE2C",  "RobsTest","HessCom2u", "HHLink","muccc.legacy-2.4GHz", "muenchen.freifunk.net",  "Cafeteria","Free_WIFI","WLANESP","muenchen.freifunk.net/muc_cty" ,"KPMS-Openhouse","KPMS-Cafeteria"};
const char* password[] = { "SOMMERREGEN05","SOMMERREGEN05", "SOMMERREGEN05", "OpenHouse", "q49adKnc4bPka7bp", "Schiller12","SOMMERREGEN05",  "SOMMERREGEN05","haileris"  , ""               ,  "Cafeteria","",       "Schiller", "","OpenHouse", "KPMS-Cafeteria-2022"};

Connection* Connection::instance = nullptr;

Connection::Connection(Adafruit_NeoPixel& px, LEDMatrix& matrix) 
    : client(espClient), 
      pixels(px),
      ledMatrix(matrix) {
    instance = this;
}

void Connection::begin() {
    setupWiFi();
    client.setServer(mqttserver, mqtt_port);
    client.setCallback(staticCallback);
    
    if (!client.connected()) {
        reconnectMQTT();
    }
}

String getFingerprint(const uint8_t* mac){
  String result;
  for (int i = 4; i < 6; ++i) {
    result += String(mac[i], 16);
  }
  return result;
}

boolean connectWIFI(const char* wifi_ssid, const char* password, int retries ){
  Serial.print("Connecting ");
  Serial.println(wifi_ssid);
  delay(100);
  WiFi.begin(wifi_ssid, password);
  String clientName =  "NotABomb";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += getFingerprint(mac);
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


void scanWIFINetwork() {
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
      int ssidSize = 0;
      int networkNo = 0;
      while (ssidSize < sizeof(ssid) ) {
        ssidSize += sizeof(ssid[networkNo]);
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
  int ssidSize = 0;
  int retries = 12;
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    // Try several WIFI Networks
    while (networkNo < sizeof(ssid)/sizeof(ssid[0])) {
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
