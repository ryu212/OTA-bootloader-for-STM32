#ifndef WS_H
#define WS_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t ws_client_init(const char *uri);
esp_err_t ws_client_send(const char *msg);
esp_err_t ws_client_deinit(void);
void socket_send_percent(int percent);


#endif