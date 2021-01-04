#ifndef LNC_OP_CODES_H
#define LNC_OP_CODES_H

#include "common.h"

#define LEFT_4_BITS_MAP 15

typedef enum lnc_op {
    LNC_OP_GPIO_OUT = 0,
    LNC_OP_GPIO_IN,
    LNC_OP_TCP_START,
    LNC_OP_TCP_STOP,
    LNC_OP_TCP_HOST,
    LNC_OP_TCP_PORT,
    LNC_OP_TCP_CONN,
    LNC_OP_TCP_SEND,
    LNC_OP_TCP_AVAILABLE,
    LNC_OP_TCP_RECV,
    //used to poll the connected device for instructions
    LNC_OP_POLL = 255,

} lnc_op_t;

//return values 201 -254 reserved for error messages

typedef enum lnc_op_err {
    //no error
    LNC_ERROR_OK = 0,
    //invalid input
    LNC_ERROR_INVALID_INPUT = 201,
    //machine not in expected state
    LNC_ERROR_INVALID_STATE,
    //invalid gpio pin
    LNC_ERROR_INVALID_GPIO_PIN,
    //invalid gpio state
    LNC_ERROR_INVALID_GPIO_STATE,
    //gpio OUT error
    LNC_ERROR_GPIO_OUT,
    //gpio IN read error
    LNC_ERROR_GPIO_IN,
    //tcp conn error (also used when client is not connected)
    LNC_ERROR_TCP_CONN,
    //tcp send error
    LNC_ERROR_TCP_SEND,
    //tcp recv error
    LNC_ERROR_TCP_RECV,
    //invalid port
    LNC_ERROR_TCP_INVALID_PORT,
    //uart read timeout
    LNC_ERROR_TIMEOUT,
    //sent wrong len
    LNC_ERROR_INVALID_LEN,

    //unknown error
    LNC_ERROR_KO = 254,

} lnc_error_t;


#endif