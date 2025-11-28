#include "nvs_control.h"
#include <stdbool.h>
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"



#define NVS_NAMESPACE "storage"
#define NVS_KEY       "in_progress"
#define TAG "nvs_state"
#define NVS_KEY1       "rollback"
void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void write_state_inprogress(bool in_progress)
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
        return;
    }

    uint8_t val = in_progress ? 1 : 0;
    err = nvs_set_u8(handle, NVS_KEY, val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u8 failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }

    nvs_close(handle);

    ESP_LOGI(TAG, "Wrote in_progress=%d to NVS", val);
}


bool read_state_inprogress()
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
void write_state_rollback(bool rollback)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
        return;
    }

    uint8_t val = rollback ? 1 : 0;
    err = nvs_set_u8(handle, NVS_KEY1, val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_set_u8(rollback) failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        return;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS commit failed: %s", esp_err_to_name(err));
    }

    nvs_close(handle);
    ESP_LOGI(TAG, "Wrote rollback=%d to NVS", val);
}

bool read_state_rollback()
{
    nvs_handle_t handle;
    esp_err_t err;
    uint8_t val = 0;

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open failed: %s", esp_err_to_name(err));
        return false; // default
    }

    err = nvs_get_u8(handle, NVS_KEY1, &val);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Rollback flag not found, defaulting to false");
        val = 0;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_get_u8 failed: %s", esp_err_to_name(err));
        val = 0;
    }

    nvs_close(handle);
    return (val == 1);
}



