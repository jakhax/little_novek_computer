#ifndef LNC_OP_CODES_H
#define LNC_OP_CODES_H

#include "common.h"

typedef enum lnc_ops {
    LNC_OP_GPIO_OUT = 0,
    LNC_OP_GPIO_IN,
    LNC_OP_SLEEP,
    LNC_OP_TCP_START,
    LNC_OP_TCP_STOP,
    LNC_OP_TCP_HOST,
    LNC_OP_TCP_PORT,
    LNC_OP_TCP_CONN,
    LNC_OP_TCP_SEND,
    LNC_OP_TCP_AVAILABLE,
    LNC_OP_TCP_RECV,
    LNC_OP_POLL,

} lnc_ops_t;

typedef enum lnc_op_err {
    LNC_ERROR_OK = 0,
    LNC_ERROR_KO = 255,

} lnc_error_t;


#endif