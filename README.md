# Little Novek Computer (LNC1.0)

## Description

LNC is a simple [virtual machine](https://en.wikipedia.org/wiki/Virtual_machine) that runs on an [ESP8266](https://lastminuteengineers.com/esp8266-nodemcu-arduino-tutorial/), instructions are sent to the VM at runtime via UART.


## LNC Specifications

### Instruction Set
An instruction is a command which tells the LNC to do some fundamental task, such as controlling state of a GPIO pin . 

Instructions have both an opcode which indicates the kind of task to perform and a set of parameters which provide inputs to the task being performed.

Each opcode represents one task that the LNC "knows" how to do. There are just 10 opcodes in the LNC. 

Each instruction is 8 bits long, with the left 4 bits storing the opcode. The rest of the 4 bits are used to store the parameters. 

|  op code| parameters  |
|--|--|
| 0000 | 0000 |

### Op Codes
Below is the list of the op codes that the LNC supports

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
} lnc_op_t;
```

### Return Values

After executing an instruction , the LNC  sends a response over uart depending on the result of the execution. Each op code has an expected return value and possible error codes

Below is a list of all the instructions return values

```cpp
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
```

### Procedure
1. Send Instruction over UART.
2. LNC executes instruction and writes back appropriate 8 bit response or error over UART.

#### LNC_OP_GPIO_OUT
Used to output a digital signal through a GPIO pin on the LNC, the opcode value is `0` .

The return value for this instruction is `0` on success and and error in case of failure

For example to pull pin 1 to low on the LNC you would send the following instruction over UART `00000010`

| opcode | pin  | state |
|--|--|--|
| 0000 | 010 | 0 |


```
[TX]-> 00000100 //pull pin 2 to low
[RX]<- 0000000 //0 - LNC_ERROR_OK
```

The LNC has only 3 output pins: `{2, 4, 5};`

In case of an invalid pin , the error `LNC_ERROR_INVALID_GPIO_PIN` is sent back like in the example below

```
[TX]-> 00000110 //pull pin 3 to low
[RX]<- 11001011 // 203 - LNC_ERROR_INVALID_GPIO_PIN
```

#### LNC_OP_GPIO_IN

Used to read the state of a digital GPIO pin on the LNC.  The opcode value is `1`.

The return value is `0` or `1` on success or an appropriate error incase of an invalid instruction
|  opcode | pin  |
|--|--|
| 0001 | 1100 |

```
[TX]-> 00011100 //read state of pin 12
[RX]<- 00000001 //pin state 

[TX]-> 00011101 //read state of pin 13
[RX]<- 00000000 //pin state 
```

The LNC has only 3 input digital pins `{12, 13, 14}`

In case of an invalid pin , the error `LNC_ERROR_INVALID_GPIO_PIN` is sent back like in the example below

```
[TX]-> 00000110 //read state of pin 3
[RX]<- 11001011 // 203 - LNC_ERROR_INVALID_GPIO_PIN
```

## Task 

Write a piece of code for the LNC that will toggle LEDs connected to the 3 input pins at the interval shown in the Pseudocode below

```cpp
int main(){
	while(1){
		//turn 
		digitalWrite(2, 1);
		sleep(1);
		digitalWrite(4, 1);
		sleep(1);
		digitalWrite(5, 1);
		sleep(3);
		
		digitalWrite(2, 0);
		sleep(1);
		digitalWrite(4, 0);
		sleep(1);
		digitalWrite(5, 0);
		sleep(2);
	}
}
```

You are allowed to use any personal microcontroller, or the ones provided to you (Arduino Mega, Nucleo STM32F429zi)  or serial communication directly from your PC to solve the problem. Do not alter the provided circuit, this task 

Feel free to ask questions or ask to be provided with materials
