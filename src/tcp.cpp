#include "common.h"
#include "op_codes.h"
#include "tcp.h"
#include "WiFiClient.h"

tcp_state_t tcp_state = TCP_STATE_IDLE;

WiFiClient client;

char host[MAX_IP_LEN + 1];

uint8_t host_len = 0;

void op_tcp_handle(uint8_t ins) {
    uint8_t op = ins >> 4;

    switch (op) {
        case LNC_OP_TCP_START:
            op_tcp_start_handle(ins);
            break;

        case LNC_OP_TCP_STOP:
            op_tcp_stop_handle(ins);
            break;

        case LNC_OP_TCP_HOST:
            op_tcp_host_handle(ins);
            break;

        case LNC_OP_TCP_PORT:
            op_tcp_port_handle(ins);
            break;

        case LNC_OP_TCP_CONN:
            op_tcp_connect_handle(ins);
            break;

        case LNC_OP_TCP_AVAILABLE:
            op_tcp_port_handle(ins);
            break;

        case LNC_OP_TCP_SEND:
            op_tcp_send_handle(ins);
            break;

        case LNC_OP_TCP_RECV:
            op_tcp_recv_handle(ins);
            break;

        default:
            break;
    }
}



void op_tcp_host_handle(uint8_t ins) {
    if (tcp_state != TCP_STATE_START) {
        Serial.write(LNC_ERROR_KO);
        return;
    }

    host_len = ins & 15;
    if (host_len == 0 || host_len  > MAX_IP_LEN) {
        Serial.write(LNC_ERROR_KO);
        return;
    }

    unsigned long timeout = millis();
    memset(host, 0, MAX_IP_LEN);

    uint8_t c = 0;
    while (c < host_len) {
        if (millis() - timeout) {
            Serial.write(LNC_ERROR_KO);
            return;
        }
        if (Serial.available()) {
            host[c] = (char)Serial.read();
        }
    }

    host[host_len] = '\0';

    Serial.write(LNC_ERROR_OK);

}