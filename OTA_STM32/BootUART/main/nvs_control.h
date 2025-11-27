#ifndef NVS_CONTROL
#define NVS_CONTROL
#include <stdbool.h>

void nvs_init();
void write_state(bool in_progress);
bool read_state();


#endif