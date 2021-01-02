#include "common.h"
#include "op_codes.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"

int connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int timeout = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - timeout > 300000) {
            return -1;
        }
    }

    return 0;
}


void op_gpio_out_handle(uint8_t op) {
    uint8_t pin = (op & 15) >> 1;
    uint8_t state = op & 1;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
    Serial.write(LNC_ERROR_OK);
}


void op_gpio_in_handle(uint8_t op) {
    uint8_t pin = (op & 15) >> 1;
    pinMode(pin, INPUT);
    uint8_t state = (uint8_t)digitalRead(pin);
    Serial.write(state);
}


void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    pinMode(5, INPUT);
    connect_wifi();

}

void loop() {

    if (WiFi.status() !=  WL_CONNECTED) {
        if (connect_wifi() != 0) {
            return;
        }
    }

    if (!Serial.available()) {
        return;
    }

    uint8_t ins = (uint8_t)Serial.read();

    uint8_t op = ins >> 4;

    switch (op) {

        case LNC_OP_GPIO_OUT:
            op_gpio_out_handle(ins);
            break;

        case LNC_OP_GPIO_IN:
            op_gpio_in_handle(ins);
            break;

        default:
            break;
    }
}