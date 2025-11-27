PA9(TX)(device) - RX(Host)
PA10(RX)(device) - TX(Host)
- Config BOOT[0:1] = 10 for uart bootloader
- Config uart line: baudrate 1200 - 115200, 8 bit, 1 stop bit, parity even
- Reset and send 0x7F to change bootloader into UART mode
 ACK = 0x79, NACK = 0x1f
- Erase before flash
- Readout unprotection, after that system will be reset, we should resend the byte 0x7F
- Write mem: payload should be under 256Kb, if the payload is larger than 256Kb we should devide it into smaller payload send them, a write mem command should have a little delay when it is finished
- Eventually, turn the BOOT[1:0] = 00 to using flash code, and reset 
testUART: 
P17(TX) - RX
P16(RX) - TX