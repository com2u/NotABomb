#include "KeyBox.h"

KeyBox::KeyBox(Connection* conn):connection(conn),
      RC_POLICE_LIGHT_PIN(40),
      Key_Green_PIN(39),
      Key_Red_PIN(38),
      Key_White1_PIN(37),
      Key_White2_PIN(36),
      toggle_switch4_PIN(35),
      toggle_switch3_PIN(34),
      toggle_switch2_PIN(33),
      toggle_switch1_PIN(21),
      black_switch1_PIN(18),
      black_switch2_PIN(15),
      Key_Green(true),
      Key_Red(true),
      Key_White1(true),
      Key_White2(true),
      toggle_switch4(true),
      toggle_switch3(true),
      toggle_switch2(true),
      toggle_switch1(true),
      black_switch1(true),
      black_switch2(true),
      motor(14) {
}

void KeyBox::activateRC_PoliceLight() {
    Serial.println("Testing slow pulsing pattern");
    for (int i = 0; i < 255; i++) {
        analogWrite(RC_POLICE_LIGHT_PIN, i);
        delay(10);
    }
    for (int i = 255; i >= 0; i--) {
        analogWrite(RC_POLICE_LIGHT_PIN, i);
        delay(10);
    }
}

void KeyBox::init() {
    pinMode(RC_POLICE_LIGHT_PIN, OUTPUT);
    pinMode(Key_Green_PIN, INPUT_PULLUP);
    pinMode(Key_Red_PIN, INPUT_PULLUP);
    pinMode(Key_White1_PIN, INPUT_PULLUP);
    pinMode(Key_White2_PIN, INPUT_PULLUP);
    pinMode(toggle_switch4_PIN, INPUT_PULLUP);
    pinMode(toggle_switch3_PIN, INPUT_PULLUP);
    pinMode(toggle_switch2_PIN, INPUT_PULLUP);
    pinMode(toggle_switch1_PIN, INPUT_PULLUP);
    pinMode(black_switch1_PIN, INPUT_PULLUP);
    pinMode(black_switch2_PIN, INPUT_PULLUP);
    pinMode(black_switch2_PIN, INPUT_PULLUP);
    pinMode(motor, OUTPUT);  
    digitalWrite(motor, HIGH); // Switch Off Motor

}

void KeyBox::handle() {
    if(Key_Green != digitalRead(Key_Green_PIN)){
        Key_Green = digitalRead(Key_Green_PIN);
        digitalWrite(motor, LOW); // Switch On Motor
        Serial.println((String) "Key_Green:"+Key_Green); 
        activateRC_PoliceLight();
        delay(5000);
        digitalWrite(motor, HIGH); // Switch Off Motor
        
    }
    if(Key_Red != digitalRead(Key_Red_PIN)){
        Key_Red = digitalRead(Key_Red_PIN);
        Serial.println((String) "Key_Red:"+Key_Red); 
        if (Key_Red == 0) {
            if (connection && connection->getMQTTClient()) {
                connection->getMQTTClient()->publish("NotABomb/Key/Command", "EXIT");
                connection->getMQTTClient()->flush();
                Serial.println("Send Exit to Quizz");
            }
        }
    }
    if(Key_White1 != digitalRead(Key_White1_PIN)){
        Key_White1 = digitalRead(Key_White1_PIN);
        Serial.println((String) "Key_White1:"+Key_White1); 
        // Only check for blue pixels when Key_White1 is pressed and color chain is active
        if (Key_White1 == 0 && connection && connection->getMQTTClient() && 
            connection->getLEDMatrix().isColorChainActive()) {
            
            // Send appropriate MQTT message
            if (connection->getLEDMatrix().checkLEDChain(0, 0, 32)) {
                //connection->getMQTTClient()->publish("NotABomb/CYD/LEDChain", "Passed");
                connection->getMQTTClient()->publish("NotABomb/Challenge", "passed");
                Serial.println((String) "NotABomb/Challenge"+ "passed"); 
            } else {
                //connection->getMQTTClient()->publish("NotABomb/CYD/LEDChain", "Failed");
                connection->getMQTTClient()->publish("NotABomb/Challenge", "failed");
                Serial.println((String) "NotABomb/Challenge"+ "failed"); 
            }
            connection->getMQTTClient()->flush();
        }
    }
    if(Key_White2 != digitalRead(Key_White2_PIN)){
        Key_White2 = digitalRead(Key_White2_PIN);
        Serial.println((String) "Key_White2:"+Key_White2); 
        if (connection->getLEDMatrix().getMode() == "ColorChain") {
            connection->getMQTTClient()->publish("NotABomb/CYD/Mode", "SimonSays");
        } else if (connection->getLEDMatrix().getMode() == "SimonSays") {
            connection->getMQTTClient()->publish("NotABomb/CYD/Mode", "Maze");
        } else if (connection->getLEDMatrix().getMode() == "Maze") {
            connection->getMQTTClient()->publish("NotABomb/CYD/Mode", "ColorChain");
        }
        delay(60); // debounce
    }
    if(toggle_switch4 != digitalRead(toggle_switch4_PIN)){
        toggle_switch4 = digitalRead(toggle_switch4_PIN);
        Serial.println((String) "toggle_switch4:"+toggle_switch4); 
    }
    if(toggle_switch3 != digitalRead(toggle_switch3_PIN)){
        toggle_switch3 = digitalRead(toggle_switch3_PIN);
        Serial.println((String) "toggle_switch3:"+toggle_switch3); 
    }
    if(toggle_switch2 != digitalRead(toggle_switch2_PIN)){
        toggle_switch2 = digitalRead(toggle_switch2_PIN);
        Serial.println((String) "toggle_switch2:"+toggle_switch2); 
    }
    if(toggle_switch1 != digitalRead(toggle_switch1_PIN)){
        toggle_switch1 = digitalRead(toggle_switch1_PIN);
        Serial.println((String) "toggle_switch1:"+toggle_switch1); 
    }
    if(black_switch1 != digitalRead(black_switch1_PIN)){
        black_switch1 = digitalRead(black_switch1_PIN);
        Serial.println((String) "black_switch1:"+black_switch1); 
    }
    if(black_switch2 != digitalRead(black_switch2_PIN)){
        black_switch2 = digitalRead(black_switch2_PIN);
        Serial.println((String) "black_switch2:"+black_switch2); 
    }
    // Reset display when all keys are pressed
    if ((Key_Green == 0) && (Key_Red == 0) && (Key_White1 == 0) && (Key_White2 == 0)){       
        if (connection && connection->getMQTTClient()) {
            connection->getMQTTClient()->publish("NotABomb/Key/Command", "RESTART");
            connection->getMQTTClient()->flush();
            Serial.println("Send Restart to Display");
        }
    }
    
}
