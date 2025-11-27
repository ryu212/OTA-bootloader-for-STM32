import serial
import time
import os


ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=115200,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_EVEN,  
    stopbits=serial.STOPBITS_ONE,
    timeout=1
)

def calc_checksum(data: bytes) -> int:
    checksum = 0
    for b in data:
        checksum ^= b
    return checksum
def init_command():
    ser.write(b'\x7F')
    ack = ser.read(1)
    cmd = 0x00 
    cmd_pl = cmd ^ 0xFF 
    print(ack.hex())
    if ack == b'\x79':
        ser.write(bytes([cmd, cmd_pl]))
    
        ack = ser.read(1)
        print(ack.hex())
        if ack == b'\x79':
            n_byte = ser.read(1)
            print(n_byte)
            int_value = n_byte[0]
            for i in range (int_value + 1):
                ack = ser.read(1)
                print(ack.hex())
            ack = ack = ser.read(1)
            print("ack: ",ack.hex(), sep = "lmao == ")
init_command()
def erase():
    cmd  = 0x43
    cmd_pl = 0x43 ^ 0xFF
    ser.write(bytes([cmd, cmd_pl]))
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("command not work")
        return 1
    cmd  = 0xFF
    cmd_pl = cmd ^ 0xFF
    ser.write(bytes([cmd, cmd_pl]))
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("erasing is not finished")
        return 1
    print("erasing flash done")
    return 
erase()

def writeUnprotect():
    cmd  = 0x73
    cmd_pl = 0x73 ^ 0xFF
    ser.write(bytes([cmd, cmd_pl]))
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("command not work")
        return 1
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("command not work")
        return 1
    print("Unprotected done!!")
    return 0

#writeUnprotect()
def readoutUnprotect():
    cmd  = 0x92
    cmd_pl = 0x92 ^ 0xFF
    ser.write(bytes([cmd, cmd_pl]))
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("command not work 1")
        return 1
    ack = ser.read(1)
    if (ack != b'\x79'):
        print("command not work 2")
        return 1
    print("readout Unprotected done!!")
    return 0
readoutUnprotect()
time.sleep(2)
init_command()
def WriteMemory(path_file: str):
    with open(path_file, 'rb') as f:
        data = f.read()
    file_size = os.path.getsize(path_file)
    
    start = 0x08000000
    MAX_BLOCK = 100
    data_len = len(data)
    pos = 0
    print(f"File size: {len(data)} bytes")
    while pos < data_len:
        cmd = 0x31
        cmd_pl = 0x31 ^ 0xFF
        adr = start + pos
        adr_bytes  = adr.to_bytes(4, byteorder='big')
        checksumm = calc_checksum(adr_bytes)

        ser.write(bytes([cmd, cmd_pl]))
        ack = ser.read(1)
        if (ack != b'\x79'):
            print("command not work 1")
            return 1
        else:
            print("ACK 1")
        
        ser.write(adr_bytes + bytes([checksumm]))
        if (ack != b'\x79'):
            print("command not work 2")
            return 1
        else:
            print("ACK 2")
        if pos+MAX_BLOCK <= data_len:
            block = data[pos:pos + MAX_BLOCK]
        else:
            block = data[pos:]
        n = len(block) - 1  # Gửi N = Số byte - 1
        payload = bytes([n]) + block
        checksum = calc_checksum(payload)
        print(checksum)
        ser.write(payload + bytes([checksum]))
        ack = ser.read(1)
        if ack != b'\x79':
            print(f"command not work 3 (write data at pos {pos})")
            return 1
        print(f"write done block {pos/256}")
        pos += len(block)
        time.sleep(1)
    print("Write done!!")
    # data_bytes = data.to_bytes(file_size, byteorder='big')
    # checksumm = calc_checksum(data_bytes)
    # ser.write(bytes[file_size, data_bytes, checksumm])
    # if (ack != b'\x79'):
    #     print("command not work 3")
    #     return 1
    # print("Write done!!")
    return 0

WriteMemory('myprogram.bin')


ser.close()
    