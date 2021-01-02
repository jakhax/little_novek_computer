#ifndef LNC_TCP_H_
#define LNC_TCP_H_

#include "common.h"

#define HOST_SEND_TIMEOUT 1000
#define PORT_SEND_TIMEOUT 1000

#define MAX_IP_LEN 15
#define MAX_PORT_LEN 5
#define MAX_PORT 65535

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
void op_tcp_handle(uint8_t ins);

/**
 * starts a TCP session
 */
void op_tcp_start_handle(uint8_t ins);

/**
 * sets TCP session host
 */
void op_tcp_host_handle(uint8_t ins);

/**
 * sets TCP port
 */
void op_tcp_port_handle(uint8_t ins);

/**
 * connects to TCP server
 */
void op_tcp_connect_handle(uint8_t ins);

/**
 * sends data to connected tcp server
 * can send a maximum of 255 bytes per call
 */
void op_tcp_send_handle(uint8_t ins);

/**
 * checks if theres any data to read from server
 * returns a maximum of 254
 */
void op_tcp_available_handle(uint8_t ins);

/**
 * receives data from a tcp server
 * can receive a maximum of 254 bytes per call
 *
 */
void op_tcp_recv_handle(uint8_t ins);

/**
 * disconnect from TCP server and end TCP session
 */
void op_tcp_stop_handle(uint8_t ins);

extern tcp_state_t tcp_state;

#endif