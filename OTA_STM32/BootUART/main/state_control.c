#include "state_control.h"
#include "http_connection.h"
#include "nvs_control.h"
void update_status(status* status_var)
{
    status_var -> start = (uint8_t)new_version();
    status_var -> in_progress = (uint8_t)read_state_inprogress();
    status_var -> rollback = (uint8_t)read_state_rollback();
}