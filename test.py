import serial, time

ser = serial.Serial("/dev/ttyUSB0", 115200)

OP_GPIO_OUT=0
OP_GPIO_IN=1
OP_SLEEP=1
PIN=2

def write_pin(p, s):
    print("[+] write_pin(%d, %d)"%(p,s))
    i = OP_GPIO_OUT
    i = i << 3
    i += p
    i = i << 1
    i += s
    i = chr(i)
    ser.write(i.encode())
    time.sleep(0.001)
    ret = ser.read_all()
    if ret:
        ret = ord(ret.decode("UTF-8")[0])
        print("[+] write_pin ret %d"%ret)

def read_pin(p):
    print("[+] read_pin(%d)"%p)
    i = OP_GPIO_IN
    i = i << 3
    i += p
    i = i << 1
    i = chr(i)
    ser.write(i.encode())

    timeout = 0
    while True:
        ret = ser.read_all()
        if not ret:
            time.sleep(0.001)
            timeout += 1
            if timeout > 5000:
                print("[-] read_pin timed out!")
                return -1
            continue
        ret = ord(ret.decode("UTF-8")[0])
        if ret != 0 and ret != 1:
            print("[-] read_pin invalid res %d"%ret)
            return -1
        print("[+] read_pin ret %d"%ret)
        return ret


s = 0
while True:
    write_pin(PIN, s)
    time.sleep(1)
    s = 1 if s == 0 else 0
    # read_pin(5)
    # time.sleep(1)
    

    