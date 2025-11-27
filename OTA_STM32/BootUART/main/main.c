#include <stdio.h>
#include "esp_spiffs.h"
#include "driver/uart.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

//-----------USER Lib ----------------------------
#include "control.h"
#include "uart_command.h"
#include "setup_SPIFFS.h"
#include "wifi_setup.h"
#include "http_connection.h"
#include "nvs_control.h"
//--------------------------------------------
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
    
    nvs_init();

    ESP_LOGI("WIFI", "ESP_WIFI_MODE_STA");
    wifi_sta_init();
    init_uart();

    uart_boot_new_firmware();
    
}