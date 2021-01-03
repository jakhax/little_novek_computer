#ifndef LNC_GPIO_H_
#define LNC_GPIO_H_

#include "common.h"
#include "op_codes.h"

/**
 * handles gpio out (digitalWrite())
 * on success write LNC_ERROR_OK on success
 * on error approprite error is written
 */
void op_gpio_out_handle(uint8_t op);

/**
 * handles gpio in (digitalRead())
 * on success writes 0 or 1 depending on state of pin
 * on error approprite error is written
 */
void op_gpio_in_handle(uint8_t op);

/**
 * setups input and output gpio pins
 */
void setup_gpio_pins();



#endif