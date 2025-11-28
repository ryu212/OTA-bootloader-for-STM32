#ifndef STATE_CONTROL_H
#define STATE_CONTROL_H
#include <stdint.h>
#include <stdbool.h>
typedef struct
{
    uint8_t start; 
    uint8_t in_progress; 
    uint8_t rollback;
}status;
void update_status(status* status_var);
#endif 