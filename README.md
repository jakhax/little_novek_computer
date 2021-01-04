# Little Novek Computer (LNC1.0)

## Description

LNC is a simple virtual machine i designed to:

- Help interns learn a few things about computers.
- Interview problem for embedded engineers prospect.

The virtual machine runs on an ESP8266, instructions are sent to the VM at runtime via UART.

## Examples / Solutions

- [python HTTP get IP](./examples/main.py)

## LNC Specifications

>> TODO

```cpp
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
    //used to poll the connected device for instructions (NOT SUPPORTED YET)
    LNC_OP_POLL = 255,

} lnc_op_t;
```

