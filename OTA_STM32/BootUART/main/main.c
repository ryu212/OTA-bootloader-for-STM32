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
#include "state_control.h"
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
    nvs_init();

    ESP_LOGI("WIFI", "ESP_WIFI_MODE_STA");
    wifi_sta_init();
    init_uart();
    status system_status = {0,0,0};
    while(1)
    {
        update_status(&system_status);
        printf("system status:\n");
        printf("start = %d\n in_progress = %d \n rollback = %d \n", system_status.start, system_status.in_progress, system_status.rollback);
        if(system_status.start)
            uart_boot_new_firmware();
        else if(system_status.in_progress)
            uart_boot_firmware();
        else if(system_status.rollback)
            roll_back();
        vTaskDelay(5000/ portTICK_PERIOD_MS);
    }

    

    
}