#include "common.h"
#include "ESP8266WiFi.h"
#include "op_codes.h"
#include "tcp.h"
#include "gpio.h"


int connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int timeout = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - timeout > 300000) {
            return -1;
        }
        delay(100);
    }

    return 0;
}


void setup() {

    Serial.begin(115200);

    setup_gpio_pins();
    
    connect_wifi();
}

void loop() {

    if (WiFi.status() !=  WL_CONNECTED) {
        if (connect_wifi() != 0) {
            return;
        }
    }

    if (!Serial.available()) {
        delay(10);
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
            op_tcp_state_machine(ins);
            break;
    }

}

