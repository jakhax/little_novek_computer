#include "common.h"
#include <inttypes.h>
#include "op_codes.h"
#include "tcp.h"
#include "WiFiClient.h"
#include "Client.h"

tcp_state_t tcp_state = TCP_STATE_IDLE;

WiFiClient client;

char host[MAX_IP_LEN + 1];

uint16_t port = 0;

uint8_t host_len = 0;

void op_tcp_state_machine(uint8_t ins) {
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
            op_tcp_available_handle(ins);
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


void op_tcp_start_handle(uint8_t ins) {
    //should we check previous state?
    tcp_state = TCP_STATE_START;

    Serial.write(LNC_ERROR_OK);

}


void op_tcp_host_handle(uint8_t ins) {

    if (tcp_state != TCP_STATE_START) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    host_len = ins & LEFT_4_BITS_MAP;
    if (host_len == 0 || host_len  > MAX_IP_LEN) {
        Serial.write(LNC_ERROR_INVALID_LEN);
        return;
    }


    unsigned long timeout = millis();
    memset(host, 0, MAX_IP_LEN);

    uint8_t count = 0;
    while (count < host_len) {
        if (millis() - timeout >=  TCP_HOST_SEND_TIMEOUT) {
            Serial.write(LNC_ERROR_TIMEOUT);
            return;
        }
        if (Serial.available()) {
            host[count] = (char)Serial.read();
            count++;
        }
    }

    host[host_len] = '\0';

    Serial.write(LNC_ERROR_OK);

    tcp_state = TCP_STATE_HOST;

}


void op_tcp_port_handle(uint8_t ins) {

    if (tcp_state !=  TCP_STATE_HOST) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    uint8_t port_len = ins & LEFT_4_BITS_MAP;

    if (port_len  > MAX_PORT_LEN) {
        Serial.write(LNC_ERROR_INVALID_LEN);
        return;
    }

    char port_c[port_len + 1];

    uint8_t count = 0;
    unsigned long timeout = millis();
    while (count < port_len) {
        if (millis() - timeout >=  TCP_PORT_SEND_TIMEOUT) {
            Serial.write(LNC_ERROR_TIMEOUT);
            return;
        }

        if (Serial.available()) {
            port_c[count] = (char)Serial.read();
            count++;
        }
    }


    if (sscanf(port_c, "%" SCNd16 "", &port) <= 0) {
        Serial.write(LNC_ERROR_TCP_INVALID_PORT);
        return;
    }

    if (port < MIN_PORT || port > MAX_PORT) {
        Serial.write(LNC_ERROR_TCP_INVALID_PORT);
        return;
    }

    Serial.write(LNC_ERROR_OK);

    tcp_state = TCP_STATE_PORT;

}


void op_tcp_connect_handle(uint8_t ins) {

    if (tcp_state !=  TCP_STATE_PORT) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    if (client.connect((const char*)host, port)) {
        Serial.write(LNC_ERROR_OK);
        tcp_state = TCP_STATE_CONNECTED;
        return;
    }

    Serial.write(LNC_ERROR_TCP_CONN);

}

void op_tcp_available_handle(uint8_t ins) {
    if (tcp_state !=  TCP_STATE_CONNECTED) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    if (!client.connected()) {
        Serial.write(LNC_ERROR_TCP_CONN);
        return;
    }

    int av = client.available();

    if (av < 0) {
        Serial.write(LNC_ERROR_KO);
        return;
    }

    if (av > MAX_TCP_AVAILABLE) {
        Serial.write((uint8_t)MAX_TCP_AVAILABLE);
        return;
    }

    Serial.write((uint8_t)av);

}


void op_tcp_send_handle(uint8_t ins) {

    if (tcp_state !=  TCP_STATE_CONNECTED) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    if (!client.connected()) {
        Serial.write(LNC_ERROR_TCP_CONN);
        return;
    }

    uint8_t send_len = ins & LEFT_4_BITS_MAP;

    if (send_len  > MAX_TCP_SEND) {
        Serial.write(LNC_ERROR_INVALID_LEN);
        return;
    }

    char send_buffer[send_len + 1];

    uint8_t count = 0;
    unsigned long timeout = millis();
    while (count < send_len) {
        if (millis() - timeout >= TCP_SEND_TIMEOUT) {
            Serial.write(LNC_ERROR_TIMEOUT);
            return;
        }

        if (Serial.available()) {
            send_buffer[count] = Serial.read();
            count++;
        }
    }

    size_t sent = client.write((const char*)send_buffer, (size_t)send_len);
    client.flush();

    if (sent == 0 || sent > (size_t)send_len) {
        Serial.write(LNC_ERROR_TCP_SEND);
        return;
    }

    Serial.write((uint8_t)sent);
    return;

}



void op_tcp_recv_handle(uint8_t ins) {


    if (tcp_state !=  TCP_STATE_CONNECTED) {
        Serial.write(LNC_ERROR_INVALID_STATE);
        return;
    }

    // we dont check for connection as some servers may have already
    // terminated the connection


    uint8_t read_len = ins & LEFT_4_BITS_MAP;

    if (read_len  > MAX_TCP_AVAILABLE) {
        Serial.write(LNC_ERROR_INVALID_LEN);
        return;
    }

    int av = client.available();

    //no data to read
    if (av <= 0) {
        Serial.write(0);
        return;
    }

    if (av > MAX_TCP_AVAILABLE) {
        av = MAX_TCP_AVAILABLE;
    }

    if (read_len > av) {
        read_len = av;
    }

    char recv_buffer[read_len + 1];

    int read = client.read((uint8_t*)recv_buffer, (size_t)read_len);

    if (read > read_len || read < 0) {
        Serial.write(LNC_ERROR_TCP_RECV);
        return;
    }

    if ((uint8_t)read !=  read_len) {
        read_len = (uint8_t)read;
    }

    Serial.write(read_len); //write length read first
    Serial.write((uint8_t*)recv_buffer, (size_t)read_len);

}

void op_tcp_stop_handle(uint8_t ins) {

    client.stop();

    Serial.write(LNC_ERROR_OK);

    tcp_state = TCP_STATE_START;
}