#ifndef BOOT_RESET_H
#define BOOT_RESET_H

#define FLASHBOOT 0
#define MEMBOOT 1


void init_gpio_reset_boot();
void bootSet(int bootMode);
void reset();

#endif