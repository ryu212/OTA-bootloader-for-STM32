#include "BOOT_RESET.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


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


