#include "gpio.h"

const uint8_t GPIO_IN_COUNT = 3;

const uint8_t GPIO_OUT_COUNT = 3;

//only pins 2, 4, 5, 12, 13, 14

uint8_t gpio_in_pins[GPIO_IN_COUNT] = {12, 13, 14};

uint8_t gpio_out_pins[GPIO_OUT_COUNT] = {2, 4, 5};

void setup_gpio_pins() {

    for (uint8_t i = 0; i < GPIO_IN_COUNT; i++) {
        pinMode(gpio_in_pins[i], INPUT);
    }

    for (uint8_t j = 0; j < GPIO_OUT_COUNT; j++) {
        pinMode(gpio_out_pins[j], OUTPUT);
    }
}


static bool validate_pin(uint8_t pin, uint8_t mode) {
    if (mode == INPUT) {
        for (uint8_t i = 0; i < GPIO_IN_COUNT; i++) {
            if (gpio_in_pins[i] == pin) {
                return true;
            }
        }
    } else if (mode == OUTPUT) {
        for (uint8_t i = 0; i < GPIO_OUT_COUNT; i++) {
            if (gpio_out_pins[i] == pin) {
                return true;
            }
        }
    }

    return false;
}

void op_gpio_out_handle(uint8_t op) {

    uint8_t pin = (op & LEFT_4_BITS_MAP) >> 1;

    if (!validate_pin(pin, OUTPUT)) {
        Serial.write(LNC_ERROR_INVALID_GPIO_PIN);
        return;
    }

    uint8_t state = op & 1;

    digitalWrite(pin, state);
    Serial.write(LNC_ERROR_OK);
}

void op_gpio_in_handle(uint8_t op) {
    uint8_t pin = (op & 15) >> 1;

    if (!validate_pin(pin, INPUT)) {
        Serial.write(LNC_ERROR_INVALID_GPIO_PIN);
        return;
    }

    uint8_t state = (uint8_t)digitalRead(pin);
    Serial.write(state);
}
