#include "nvs_control.h"
#include <stdbool.h>
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"



#define NVS_NAMESPACE "storage"
#define NVS_KEY       "in_progress"
#define TAG "nvs_state"
void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void write_state(bool in_progress)
{
    nvs_handle_t handle;
    esp_err_t err;

    // Mở namespace (NVS_READWRITE để có thể ghi)
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
        return;
    }

    // Lưu boolean dưới dạng u8 (0 hoặc 1)
    uint8_t val = in_progress ? 1 : 0;
    err = nvs_set_u8(handle, NVS_KEY, val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u8 failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }

    // Commit để chắc chắn dữ liệu được ghi vào flash
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_commit failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }

    ESP_LOGI(TAG, "Wrote %s=%d to NVS", NVS_KEY, val);

    nvs_close(handle);
}

bool read_state()
{
    nvs_handle_t handle;
    esp_err_t err;
    uint8_t value = 0;       // mặc định là false

    // Mở namespace
    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW("nvs_state", "Namespace not found, using default false");
        return false;
    } else if (err != ESP_OK) {
        ESP_LOGE("nvs_state", "Error opening NVS (%s)", esp_err_to_name(err));
        return false;
    }

    // Đọc giá trị
    err = nvs_get_u8(handle, "in_progress", &value);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW("nvs_state", "Key not found, using default false");
        nvs_close(handle);
        return false;
    } else if (err != ESP_OK) {
        ESP_LOGE("nvs_state", "Error reading state (%s)", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    nvs_close(handle);
    return value != 0;  // convert u8 → bool
}

