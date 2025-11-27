import serial
import time
import os

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=115200,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_EVEN,  
    stopbits=serial.STOPBITS_ONE,
    timeout=10
)


while(1):
    data = ser.read(1)
    if data == b'\x7F':
        cmd = 0x79
        ser.write(bytes([cmd]))

    print("Received:", data.hex())

