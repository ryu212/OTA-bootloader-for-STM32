#ifndef NVS_CONTROL
#define NVS_CONTROL
#include <stdbool.h>

void nvs_init();
void write_state_inprogress(bool in_progress);
void write_state_rollback(bool rollback);
bool read_state_inprogress();
bool read_state_rollback();


#endif