#ifndef BOOT_RESET_H
#define BOOT_RESET_H

#define FLASHBOOT 0
#define MEMBOOT 1
#include <stdbool.h>

void init_gpio_reset_boot();
void bootSet(int bootMode);
void reset();
void uart_boot_firmware();
void uart_boot_new_firmware();
void roll_back();
#endif