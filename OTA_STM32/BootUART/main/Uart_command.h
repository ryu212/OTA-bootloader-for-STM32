#ifndef UART_CONFIG_H
#define UART_CONFIG_H
uint8_t calculate_checksum(uint8_t* data,uint8_t n);
void update_payload_data(int n);
uint8_t checksum_addr(int32_t addr);
void init_uart();
int sendDataBytes(const char* logName, const uint8_t* data, uint8_t n);
void sendCommand(uint8_t command);
int init_line();
int erase();
int readout_unprotect();
int write_flash(int32_t addr, uint8_t number, uint8_t* data);
int flash_firmware(char* path);
#endif