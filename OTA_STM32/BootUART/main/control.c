#include "control.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_connection.h"
#include "uart_command.h"
#include "setup_SPIFFS.h"
#include "nvs_control.h"
#define BOOT0 GPIO_NUM_32
#define BOOT1 GPIO_NUM_33
#define RST GPIO_NUM_5
void init_gpio_reset_boot()
{
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << GPIO_NUM_32) | (1ULL << GPIO_NUM_33) | (1ULL << RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
        };
    gpio_config(&config);
    gpio_set_pull_mode(RST, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(BOOT0, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(BOOT1, GPIO_PULLDOWN_ONLY);
}

void bootSet(int bootMode)
{
    if(bootMode == FLASHBOOT)
    {
        gpio_set_level(BOOT0, 0);
        gpio_set_level(BOOT1, 0);
        return;
    }
    if(bootMode == MEMBOOT)
    {
        gpio_set_level(BOOT0, 1);
        gpio_set_level(BOOT1, 0);
        return;
    }
}
void reset()
{
    gpio_set_level(RST, 0);
    vTaskDelay(100/ portTICK_PERIOD_MS);
    gpio_set_level(RST, 1);
    vTaskDelay(100/ portTICK_PERIOD_MS);

}
void uart_boot_firmware()
{
    copy_file("/spiffs/OTA1/rollback.bin","/spiffs/OTA0/firmware.bin");
    write_state(true);
    init_gpio_reset_boot();
    vTaskDelay(300/ portTICK_PERIOD_MS);
    bootSet(MEMBOOT);
    reset();
    while(init_line() == -1)
    {vTaskDelay(100/ portTICK_PERIOD_MS);}
    vTaskDelay(100/ portTICK_PERIOD_MS);
    while(readout_unprotect() == -1)
    {vTaskDelay(100/ portTICK_PERIOD_MS);}
    printf("reset again!!!\n");
    vTaskDelay(300/ portTICK_PERIOD_MS);
    while(init_line() == -1)
    {vTaskDelay(100/ portTICK_PERIOD_MS);}
    vTaskDelay(100/ portTICK_PERIOD_MS);
    while(erase() == -1)
    {vTaskDelay(100/ portTICK_PERIOD_MS);};
    while(flash_firmware("/spiffs/OTA0/firmware.bin") == -1)
    {vTaskDelay(100/ portTICK_PERIOD_MS);};
    bootSet(FLASHBOOT);
    reset();
    printf("Done kkk\n");
    write_state(false);
}

void uart_boot_new_firmware()
{
    http_download_firmware();
    uart_boot_firmware();
}

