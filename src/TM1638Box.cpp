#include "TM1638Box.h"

TM1638Box* TM1638Box::instance = nullptr;

TM1638Box::TM1638Box(uint8_t dio_pin, uint8_t clk_pin, uint8_t stb_pin, Connection* conn) 
    : connection(conn) {
    module = new TM1638(dio_pin, clk_pin, stb_pin);
    buttons = new TM16xxButtons(module);
    display = new TM16xxDisplay(module, 8);
    instance = this;
}

void TM1638Box::begin() {
    module->clearDisplay();
    module->setupDisplay(true, 7);
    
    buttons->attachRelease(fnRelease);
    buttons->attachClick(fnClick);
    buttons->attachDoubleClick(fnDoubleclick);
    buttons->attachLongPressStart(fnLongPressStart);
    buttons->attachLongPressStop(fnLongPressStop);
    buttons->attachDuringLongPress(fnLongPress);

    // Initial display test
    module->setDisplayToString("88888888");
    delay(400);
    module->setDisplayToString("00000000");
    delay(400);
    module->clearDisplay();
}

void TM1638Box::update() {
    updateCountdown();
    handleButtons();
}

void TM1638Box::startCountdown(int hours, int minutes, int seconds) {
    unsigned long totalSeconds = hours * 3600UL + minutes * 60UL + seconds;
    countdownTarget = millis() + (totalSeconds * 1000UL);
    countdownRunning = true;
}

void TM1638Box::updateCountdown() {
    if (!countdownRunning) return;

    unsigned long currentTime = millis();
    if (currentTime >= countdownTarget) {
        module->setDisplayToString("00-00-00");
        countdownRunning = false;
        return;
    }

    unsigned long remainingSeconds = (countdownTarget - currentTime) / 1000UL;
    int hours = remainingSeconds / 3600;
    int minutes = (remainingSeconds % 3600) / 60;
    int seconds = remainingSeconds % 60;

    char countdownStr[9];
    snprintf(countdownStr, sizeof(countdownStr), "%02d-%02d-%02d", hours, minutes, seconds);
    module->setDisplayToString(countdownStr);
}

void TM1638Box::handleButtons() {
    static unsigned long ulTime = millis();
    uint32_t dwButtons = buttons->tick();

    if (dwButtons) {
        display->setDisplayToHexNumber(dwButtons, 0, false);
    } else if (millis() - ulTime > 100) {
        ulTime = millis();
    }
}

void TM1638Box::clearDisplay() {
    module->clearDisplay();
}

void TM1638Box::setDisplayString(const char* text) {
    module->setDisplayToString(text);
}

void TM1638Box::setLEDs(uint8_t value) {
    module->setLEDs(value);
}

// Static callback functions
void TM1638Box::fnRelease(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" release."));
        instance->display->println(F("rels  "));
    }
}

void TM1638Box::fnClick(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" click."));
        instance->display->println(F("sclk  "));

        // Send MQTT message on button click
        if (instance->connection && instance->connection->getMQTTClient()) {
            instance->connection->getMQTTClient()->publish("NotABomb/Key/ButtonClick", "1");
        }

        switch (nButton) {
            case 16: instance->module->setLEDs(1); break;
            case 18: instance->module->setLEDs(2); break;
            case 20: instance->module->setLEDs(4); break;
            case 22: instance->module->setLEDs(8); break;
            case 17: instance->module->setLEDs(16); break;
            case 19: instance->module->setLEDs(32); break;
            case 21: instance->module->setLEDs(64); break;
            case 23: instance->module->setLEDs(128); break;
        }

        instance->module->clearDisplay();
        char text[17];
        ltoa(nButton, text, 10);
        instance->module->setDisplayToString(text);
    }
}

void TM1638Box::fnDoubleclick(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" doubleclick."));
        instance->display->println(F("dclk  "));
    }
}

void TM1638Box::fnLongPressStart(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" longPress start"));
        instance->display->println(F("strt  "));
    }
}

void TM1638Box::fnLongPress(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" longPress..."));
    }
}

void TM1638Box::fnLongPressStop(byte nButton) {
    if (instance) {
        Serial.print(F("Button "));
        Serial.print(nButton);
        Serial.println(F(" longPress stop"));
        instance->display->println(F("stop  "));
    }
}
