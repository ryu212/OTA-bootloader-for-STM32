#include "setup_SPIFFS.h"
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
#include <stdio.h>
void setupSPIFFS()
{
    esp_vfs_spiffs_conf_t config = 
    {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
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
    create_file_if_not_exists("/spiffs/OTA0/firmware.bin");
    create_file_if_not_exists("/spiffs/OTA1/rollback.bin");
    create_file_if_not_exists("/spiffs/version.json");
}

void create_file_if_not_exists(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f) {
        // File đã tồn tại
        fclose(f);
        return;
    }

    // File chưa tồn tại → tạo mới
    f = fopen(path, "w");
    if (!f) {
        printf("Failed to create file: %s\n", path);
        return;
    }

    printf("File created: %s\n", path);
    fclose(f);
}
void copy_file(const char* dst, const char* src)
{
    FILE *f_src = fopen(src, "rb");
    if (!f_src) {
        printf("Failed to open source: %s\n", src);
        return;
    }

    FILE *f_dst = fopen(dst, "wb");
    if (!f_dst) {
        printf("Failed to open destination: %s\n", dst);
        fclose(f_src);
        return;
    }

    // Buffer trung gian (tối ưu: 1KB)
    uint8_t buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), f_src)) > 0) {
        fwrite(buffer, 1, bytes, f_dst);
    }

    fclose(f_src);
    fclose(f_dst);

    printf("Copied file from %s -> %s\n", src, dst);
}