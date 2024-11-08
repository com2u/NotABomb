#ifndef KEYBOX_H
#define KEYBOX_H

#include <Arduino.h>
#include "connection.h"

class KeyBox {
private:
    const int RC_POLICE_LIGHT_PIN;
    const int Key_Green_PIN;
    const int Key_Red_PIN;
    const int Key_White1_PIN;
    const int Key_White2_PIN;
    const int toggle_switch4_PIN;
    const int toggle_switch3_PIN;
    const int toggle_switch2_PIN;
    const int toggle_switch1_PIN;
    const int black_switch1_PIN;
    const int black_switch2_PIN;
    
    uint8_t Key_Green;
    uint8_t Key_Red;
    uint8_t Key_White1;
    uint8_t Key_White2;
    uint8_t toggle_switch4;
    uint8_t toggle_switch3;
    uint8_t toggle_switch2;
    uint8_t toggle_switch1;
    uint8_t black_switch1;
    uint8_t black_switch2;
    uint8_t motor;

    void activateRC_PoliceLight();
    Connection* connection;

public:
    KeyBox(Connection* conn);
    void init();
    void handle();
};

#endif
