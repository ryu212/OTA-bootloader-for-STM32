#include <stdio.h>
#include "esp_spiffs.h"
#include "driver/uart.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "BOOT_RESET.h"
#include "Uart_command.h"
#include "SetupSPIFFS.h"
#include "WifiSetup.h"
#include "HttpDriveDL.h"

#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sys.h"

void app_main(void)
{
    setupSPIFFS();
    //Kiem tra NVS flash sau do bat wifi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI("WIFI", "ESP_WIFI_MODE_STA");
    wifi_sta_init();

    //Tai file 
    http_download_file();
    //Thuc hien boot UART
    init_gpio_reset_boot();
    init_uart();
    vTaskDelay(3000/ portTICK_PERIOD_MS);
    bootSet(MEMBOOT);
    reset();
    init_line();
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    readout_unprotect();
    printf("reset again!!!\n");
    vTaskDelay(3000/ portTICK_PERIOD_MS);
    init_line();
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    erase();
    flash_firmware("/storage/myprogram.bin");
    bootSet(FLASHBOOT);
    reset();
    printf("Done kkk\n");
}