#include "SetupSPIFFS.h"
#include <stdio.h>
#include "esp_spiffs.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

void setupSPIFFS()
{
    esp_vfs_spiffs_conf_t config = 
    {
        .base_path = "/storage",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_err_t result = esp_vfs_spiffs_register(&config);

    if(result != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "Fail to initialize SPIFFS %s", esp_err_to_name(result));
        return;
    }
    size_t total, used;
    result = esp_spiffs_info(config.partition_label, &total, &used);
    if(result != ESP_OK)
    {
        ESP_LOGE("SPIFFS", "Fail to initalize partition info %s", esp_err_to_name(result));
        return;
    }
    else 
    {
        ESP_LOGI("SPIFFS", "Partion info:  \n total: %d \n used: %d", total, used);
    }
    
}