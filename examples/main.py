"""
This example has two functions that demontrate how you can
use the LNC for TCP comms and controlling GPIO
see 
- http_get_ip()  - gets public IP of LNC through icanhazip.com
- toggle_built_in_led() - toggle built in LED for T milliseconds
"""

import serial, time

import urllib3
from io import BytesIO
from http.client import HTTPResponse

ser = serial.Serial("/dev/ttyUSB0", 115200,
        serial.EIGHTBITS, serial.PARITY_NONE,
        serial.STOPBITS_ONE
    )

#opcodes
LNC_OP_GPIO_OUT = 0
LNC_OP_GPIO_IN = 1
LNC_OP_TCP_START = 2
LNC_OP_TCP_STOP = 3
LNC_OP_TCP_HOST = 4
LNC_OP_TCP_PORT = 5
LNC_OP_TCP_CONN = 6
LNC_OP_TCP_SEND = 7
LNC_OP_TCP_AVAILABLE = 8
LNC_OP_TCP_RECV = 9

#error codes
LNC_ERROR_OK = 0
LNC_ERROR_INVALID_INPUT = 201
LNC_ERROR_INVALID_STATE = 202
LNC_ERROR_INVALID_GPIO_PIN = 203
LNC_ERROR_INVALID_GPIO_STATE = 204
LNC_ERROR_GPIO_OUT = 205 
LNC_ERROR_GPIO_IN = 206
LNC_ERROR_TCP_CONN =  207
LNC_ERROR_TCP_SEND = 208
LNC_ERROR_TCP_RECV = 209
LNC_ERROR_TCP_INVALID_PORT = 210
LNC_ERROR_TIMEOUT = 211
LNC_ERROR_INVALID_LEN = 212
LNC_ERROR_KO = 254

MAX_TCP_AVAILABLE = 15
MAX_TCP_SEND = 15


BUILT_IN_LED = 2


def lnc_err_to_txt(err):
    msg = "UNKNOWN"
    if err == LNC_ERROR_OK:
        msg = "LNC_ERROR_OK"
    elif err == LNC_ERROR_INVALID_INPUT:
        msg = "LNC_ERROR_INVALID_INPUT"
    elif err == LNC_ERROR_INVALID_STATE:
        msg = "LNC_ERROR_INVALID_STATE"
    elif err == LNC_ERROR_INVALID_GPIO_PIN:
        msg = "LNC_ERROR_INVALID_GPIO_PIN"
    elif err == LNC_ERROR_INVALID_GPIO_STATE:
        msg = "LNC_ERROR_INVALID_GPIO_STATE"
    elif err == LNC_ERROR_GPIO_OUT:
        msg = "LNC_ERROR_GPIO_OUT"
    elif err == LNC_ERROR_GPIO_IN:
        msg = "LNC_ERROR_GPIO_IN"
    elif err == LNC_ERROR_TCP_CONN:
        msg = "LNC_ERROR_TCP_CONN"
    elif err == LNC_ERROR_TCP_SEND:
        msg = "LNC_ERROR_TCP_SEND"
    elif err == LNC_ERROR_TCP_RECV:
        msg = "LNC_ERROR_TCP_RECV"
    elif err == LNC_ERROR_TCP_INVALID_PORT:
        msg = "LNC_ERROR_TCP_INVALID_PORT"
    elif err == LNC_ERROR_TIMEOUT:
        msg = "LNC_ERROR_TIMEOUT"
    elif err == LNC_ERROR_INVALID_LEN:
        msg = "LNC_ERROR_INVALID_LEN"
    elif err == LNC_ERROR_KO:
        msg = "LNC_ERROR_KO"
    elif err > 0 and err <= 200: #non errors
        msg = "LNC_RET"
 
    return msg

def lnc_op_to_txt(op):
    msg = "UNKNOWN_OP"
    if op == LNC_OP_GPIO_OUT:
        msg = "LNC_OP_GPIO_OUT"
    elif op == LNC_OP_GPIO_IN:
        msg = "LNC_OP_GPIO_IN"
    elif op == LNC_OP_TCP_START:
        msg = "LNC_OP_TCP_START"
    elif op == LNC_OP_TCP_STOP:
        msg = "LNC_OP_TCP_STOP"
    elif op == LNC_OP_TCP_STOP:
        msg = "LNC_OP_TCP_STOP"
    elif op == LNC_OP_TCP_HOST:
        msg = "LNC_OP_TCP_HOST"
    elif op == LNC_OP_TCP_PORT:
        msg = "LNC_OP_TCP_PORT"
    elif op == LNC_OP_TCP_CONN:
        msg = "LNC_OP_TCP_CONN"
    elif op == LNC_OP_TCP_SEND:
        msg = "LNC_OP_TCP_SEND"
    elif op == LNC_OP_TCP_AVAILABLE:
        msg = "LNC_OP_TCP_AVAILABLE"
    elif op == LNC_OP_TCP_RECV:
        msg = "LNC_OP_TCP_RECV"
    return msg

def ser_write_ins(ins):
    ins_op = ord(ins)
    op = ins_op >> 4
    msg = lnc_op_to_txt(op)

    print("[+] -> %s _(%d)"%(msg, ins_op))
    ser.write(ins.encode())

def ser_write_data(op, data):
    msg = lnc_op_to_txt(op)
    print("[+] ->  {} _(data truncated...)".format(msg))
    ser.write(data.encode())

def ser_read_with_timeout(timeout = 5000, n = None):
    t = time.time()
    while True:
        if time.time() - t > timeout/1000:
            print("[-] serial read timeout")
            return None
        if not n:
            d = ser.read_all()
        else:
            #block
            ser.timeout = None
            d = ser.read(n)
        if not d:
            continue
        return d

def get_err_from_response(r):
    try:
        err = int.from_bytes(r, 'little')
    except ValueError as e:
        print("[-] %s"%e.__str__())
        return None
    
    msg = lnc_err_to_txt(err)
    if err == LNC_ERROR_OK:
        print("[+] <- %s _(%d)"%(msg, err))
    else:
        print("[-] <- %s _(%d)"%(msg, err))

    return err

def dump_serial():
    time.sleep(0.5)
    print(ser.read_all())


def lnc_gpio_out(p, s):
    print("[+] lnc_gpio_out(%d, %d)"%(p,s))

    i = LNC_OP_GPIO_OUT
    i = i << 3
    i += p
    i = i << 1
    i += s
    i = chr(i)

    ser_write_ins(i)
    data = ser_read_with_timeout()
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK


def lnc_gpio_in(p):
    print("[+] lnc_gpio_in(%d)"%p)
    i = LNC_OP_GPIO_IN
    i = i << 3
    i += p
    i = i << 1
    i = chr(i)

    ser_write_ins(i)
    data = ser_read_with_timeout()
    if not data:
        return None

    ret = get_err_from_response(data)
    if ret not in [0,1]:
        return None
    return ret


def lnc_tcp_start():
    print("[+] lnc_tcp_start()")

    i = chr(LNC_OP_TCP_START << 4)
    ser_write_ins(i)
    data = ser_read_with_timeout()
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK

def lnc_tcp_set_host(host):
    print("[+] lnc_tcp_set_host({})".format(host))

    i = LNC_OP_TCP_HOST << 4
    i = chr( i + len(host))
    ser_write_ins(i)
    
    ser_write_data(LNC_OP_TCP_HOST, host)
    data = ser_read_with_timeout()
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK

def lnc_tcp_set_port(port):
    print("[+] lnc_tcp_set_port({})".format(port))

    i = LNC_OP_TCP_PORT << 4
    i = chr(i + len(str(port)))

    ser_write_ins(i)
    
    ser_write_data(LNC_OP_TCP_PORT, str(port))
    data = ser_read_with_timeout()
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK

def lnc_tcp_connect():
    print("[+] lnc_tcp_connect()")

    i = chr(LNC_OP_TCP_CONN << 4)
    ser_write_ins(i)

    data = ser_read_with_timeout(5000)
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK

def lnc_tcp_send(data, timeout=None):
    print("[+]lnc_tcp_send(len={}, timeout={})".format(len(data), timeout))

    chunk_size = MAX_TCP_SEND
    count = 0
    size = len(data)
    t = time.time()

    while count <  len(data):
        if timeout and (time.time() - t) >= timeout/1000:
            print("[+] send timed out")
            return None

        to_send = chunk_size if size - count >= chunk_size else count+(size%chunk_size)
        chunk = data[count:count+to_send]
        i = LNC_OP_TCP_SEND << 4
        i = chr(i + len(chunk))
        ser_write_ins(i)

        ser_write_data(LNC_OP_TCP_SEND, chunk)

        recv = ser_read_with_timeout(30000)
        if not recv:
            return None

        sent = get_err_from_response(recv)
        if sent > MAX_TCP_SEND:
            return None
        
        count += sent
        print("[+] sent %d bytes to server"%(sent))

    return count


def lnc_tcp_recv(timeout = None):
    print("[+]lnc_tcp_recv(timeout={})".format(timeout))

    t = time.time()

    response = b''
    while True:
        if timeout and (time.time() - t) >= timeout/100:
            print("[-] recv timed out!")
            return None

        i = LNC_OP_TCP_RECV << 4
        i = chr(i + MAX_TCP_AVAILABLE)
        ser_write_ins(i)

        data = ser_read_with_timeout(30000, 1)
        if not data:
            return None
        r = get_err_from_response(data)
        if r > MAX_TCP_AVAILABLE:
            return None

        if r == 0:
            print("[+] No more data to read")
            break

        data = ser_read_with_timeout(30000, r)
        if not data:
            return None

        response += data
        print("[+] received %d bytes"%(len(data)))

    return response

def lnc_tcp_stop():
    print("[+]lnc_tcp_stop()")
    i = chr(LNC_OP_TCP_STOP << 4)

    ser_write_ins(i)

    data = ser_read_with_timeout(5000)
    if not data:
        return None

    if get_err_from_response(data) !=  LNC_ERROR_OK:
        return None

    return LNC_ERROR_OK


class BytesIOSocket:
    def __init__(self, content):
        self.handle = BytesIO(content)

    def makefile(self, mode):
        return self.handle

def response_from_bytes(data):
    sock = BytesIOSocket(data)

    response = HTTPResponse(sock)
    response.begin()

    return urllib3.HTTPResponse.from_httplib(response)

def http_get_ip():
    print("[+] http_get_ip()")

    HOST = "147.75.47.199" #icanhazip.com, ip may change
    PORT = 80
    PAYLOAD = "GET / HTTP/1.1\r\nHOST: %s\r\n\r\n"%HOST

    print("\r\n ++++ HTTP REQUEST ++++")
    print(PAYLOAD)
    print("---- HTTP REQUEST ----\r\n")

    if lnc_tcp_start() != LNC_ERROR_OK:
        print("[-] unable to start tcp session")
        return
    print("[+] tcp session started")

    if lnc_tcp_set_host(HOST) != LNC_ERROR_OK:
        print("[-] unable to set tcp host")
        return
    print("[+] tcp host set")

    if lnc_tcp_set_port(PORT) != LNC_ERROR_OK:
        print("[-] unable to set tcp port")
        return
    print("[+] tcp port set")

    if lnc_tcp_connect():
        print("[-] unable to open tcp connection")
        return
    print("[+] tcp connection opened")

    if lnc_tcp_send(PAYLOAD) !=  len(PAYLOAD):
        print("[-] error sending http payload")
        return
    print("[+] http payload sent")

    response = lnc_tcp_recv()
    if not response:
        print("[-] unable to receive response")
        return
    print("[+] receive total %d bytes"%(len(response)))

    ip = None

    try:
        http_res = response_from_bytes(response)
        print("\r\n+++++ HTTP HEADERS +++++")
        print("\n".join(["%s:%s"%(k,v) for k,v in http_res.headers.items()]))
        print("----- HTTP HEADERS -----\r\n")
        print("\r\n+++++ HTTP BODY +++++")
        body = http_res.data.decode("UTF-8")
        print(body)
        print("----- HTTP BODY -----\r\n")
        ip = body

    except Exception as e:
        print(e)
        print(response)

    if lnc_tcp_stop() != LNC_ERROR_OK:
        print("[-] unable to stop tcp session")
    print("[+] stopped TCP session")
    
    return ip

def toggle_built_in_led(timeout = 5000):
   
    s = 0
    i = s
    t = time.time()
    while (time.time() - t) <= timeout/1000:
        lnc_gpio_out(BUILT_IN_LED, s)
        time.sleep(0.5)
        s = 1 if s == 0 else 0
    #back to init state
    lnc_gpio_out(BUILT_IN_LED, i)

toggle_built_in_led()

ip = http_get_ip()
if ip:
    print("IP: {}".format(ip))

    

    