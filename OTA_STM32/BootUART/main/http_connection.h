#ifndef HTTP_DRIVE_H
#define HTTP_DRIVE_H
#include "esp_err.h"
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"


esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void http_download_file(void);

#endif