#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

extern int s_retry_num;
extern EventGroupHandle_t s_wifi_event_group;
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_sta_init(void);

#endif