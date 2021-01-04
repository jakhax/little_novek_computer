#ifndef LNC_TCP_H_
#define LNC_TCP_H_

#include "common.h"

#define TCP_HOST_SEND_TIMEOUT 1000
#define TCP_PORT_SEND_TIMEOUT 1000
#define TCP_SEND_TIMEOUT 1000

#define MAX_IP_LEN 15
#define MAX_PORT_LEN 5
#define MIN_PORT 1
#define MAX_PORT 65535

#define MAX_TCP_AVAILABLE 15
#define MAX_TCP_SEND 15
#define MAX_TCP_READ MAX_TCP_AVAILABLE

typedef enum tcp_states {
    TCP_STATE_IDLE = 0, //not connected
    TCP_STATE_START,
    TCP_STATE_HOST, //has set host
    TCP_STATE_PORT, //has set port
    TCP_STATE_CONNECTED, //connected to tcp server
} tcp_state_t;

/**
 * handles TCP op codes
 */
void op_tcp_state_machine(uint8_t ins);

/**
 * starts a TCP session
 * on success LNC_ERROR_OK is written
 * on error appropriate error is written
 */
void op_tcp_start_handle(uint8_t ins);

/**
 * sets TCP session host
 * on success LNC_ERROR_OK is written
 * on error appropriate error is written
 */
void op_tcp_host_handle(uint8_t ins);

/**
 * sets TCP port
 * on success LNC_ERROR_OK is written
 * on error appropriate error is written
 */
void op_tcp_port_handle(uint8_t ins);

/**
 * connects to TCP server
 * on success LNC_ERROR_OK is written
 * on error appropriate error is written
 */
void op_tcp_connect_handle(uint8_t ins);

/**
 * checks if theres any data to read from server
 * on success number of available bytes to read is written
 * returns a maximum of MAX_TCP_AVAILABLE
 * on error appropriate error is written
 */
void op_tcp_available_handle(uint8_t ins);

/**
 * sends data to connected tcp server
 * can send a maximum of MAX_TCP_SEND bytes per call
 * on success writes back the number of bytes sent
 * on error appropriate error is written
 */
void op_tcp_send_handle(uint8_t ins);


/**
 * receives data from a tcp server
 * can receive a maximum of MAX_TCP_READ bytes per call
 * if not data is available for receiving it will send a 0 and exit
 * if an error occurs during reading it will send the error instead
 */
void op_tcp_recv_handle(uint8_t ins);

/**
 * disconnect from TCP server and end TCP session
 */
void op_tcp_stop_handle(uint8_t ins);

extern tcp_state_t tcp_state;

#endif