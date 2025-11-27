    #include "driver/uart.h"
    #include "Uart_command.h"
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_log.h"
    #include "driver/gpio.h"
    #include "esp_system.h"
    #include "esp_log.h"
    #include "string.h"
    #include "freertos/event_groups.h"
    #include "freertos/semphr.h"
    #include <stdint.h>

    #define BLOCK_SIZE 256 * 1024
    #define ACK 0x79
    #define NACK 0x1F

    #define INIT 0x7F
    #define ERASE 0x43
    #define READOUT_UNPROTECT 0x92
    #define WRITE_MEM 0x31

    #define  ACK_BIT  BIT0
    #define  NACK_BIT  BIT1
    #define  ACK1_BIT  BIT0
    #define  NACK1_BIT  BIT1

    #define TXD_PIN 17
    #define RXD_PIN 16
    
    static const int RX_BUF_SIZE = 1024;


    static const char *TX_TASK_TAG = "TX_TASK";
    static const char *RX_TASK_TAG = "RX_TASK";
    static volatile bool stop_tasks = false;
    SemaphoreHandle_t sema = NULL;
    EventGroupHandle_t event_group;
    

    uint8_t calculate_checksum(uint8_t* data, uint8_t n)
    {
        uint8_t checksum = 0;
        for(int i = 0; i < n; i++)
            checksum ^= data[i];
        checksum^= (n-1);
        return checksum;
    }

    void update_payload_data(int n)
    {
        
    }
    uint8_t checksum_addr(int32_t addr)
    {
        uint8_t* bytes = (uint8_t*)&addr;
        uint8_t checksum = 0;
        for (int i = 0; i < 4; i++) {
            checksum ^= bytes[i];
        }
        return (uint8_t)checksum; // Ép kiểu về uint8_t nếu cần
    }
    void init_uart()
    {
        uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_EVEN,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };
        // We won't use a buffer for sending data.
        uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
        uart_param_config(UART_NUM_2, &uart_config);
        uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        gpio_set_pull_mode(RXD_PIN, GPIO_PULLUP_ONLY);
        event_group = xEventGroupCreate();
    }
    
    int sendDataBytes(const char* logName, const uint8_t* data, uint8_t n)
    {
    //const int len = sizeof(data);
        if (data == NULL) {
            ESP_LOGE(logName, "Error: Data pointer is NULL");
            return -1;
        }
        if (n <= 0) {
            ESP_LOGE(logName, "Error: Invalid number of bytes to send: %d", n);
            return -1;
        }

        // Log dữ liệu trước khi gửi (cho gỡ lỗi)
        ESP_LOGI(logName, "Preparing to send %d bytes", n);
        if (n > 0) {
            ESP_LOGI(logName, "First 4 bytes: %02x %02x %02x %02x", 
                    data[0], data[1], data[2], data[3]);
        }
        const int txBytes = uart_write_bytes(UART_NUM_2, data, n);
        ESP_LOGI(logName, "Wrote %d bytes", txBytes);
        return txBytes;
    }
    
    void sendCommand(uint8_t command)
    {
        uint8_t *sending_data = (uint8_t *)malloc(sizeof(uint8_t) * 2);
        sending_data[0] = command;
        sending_data[1] = ~command;
        sendDataBytes(TX_TASK_TAG, sending_data, 2);
        free(sending_data);
    }

    
    static void tx_task(void *arg) {
        esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
        uint8_t sending_data = 0x7F;
        while (!stop_tasks) {
            sendDataBytes(TX_TASK_TAG, &sending_data, 1);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI("TX_TASK", "tx_task exiting...");
        vTaskDelete(NULL);
    }

    static void rx_task(void *arg) {
        esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
        uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
        if (data == NULL) {
            ESP_LOGE(RX_TASK_TAG, "Failed to allocate memory");
            vTaskDelete(NULL);
            return;
        }
        while (!stop_tasks) {
            const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
            if (rxBytes > 0 && rxBytes <= RX_BUF_SIZE) {
                data[rxBytes] = 0;
                ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
                ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
                switch (data[0]) {
                    case ACK:
                        ESP_LOGI("RX", "ACK");
                        xEventGroupSetBits(event_group, ACK_BIT);
                        xSemaphoreGive(sema);
                        break;
                    case NACK:
                        ESP_LOGI("RX", "NACK");
                        xEventGroupSetBits(event_group, NACK_BIT);
                        xSemaphoreGive(sema);
                        break;
                    default:
                        break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            if (rxBytes > 1 && rxBytes <= RX_BUF_SIZE) {
                data[rxBytes] = 0;
                ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
                ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
                switch (data[1]) {
                    case ACK:
                        ESP_LOGI("RX", "ACK1");
                        xEventGroupSetBits(event_group, ACK1_BIT);
                        xSemaphoreGive(sema);
                        break;
                    case NACK:
                        ESP_LOGI("RX", "NACK1");
                        xEventGroupSetBits(event_group, NACK1_BIT);
                        xSemaphoreGive(sema);
                        break;
                    default:
                        break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }

        }
        ESP_LOGI("RX_TASK", "rx_task exiting...");
        free(data);
        vTaskDelete(NULL);
    }

    int init_line() {
        //init_uart();
        stop_tasks = false;
        sema = xSemaphoreCreateBinary();
        if (sema == NULL) {
            ESP_LOGE("INIT_LINE", "Failed to create semaphore");
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(7000)); // Consider reducing this delay
    
        TaskHandle_t rx_task_handle, tx_task_handle;
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        xTaskCreate(tx_task, "uart_tx_task", 4096, NULL, configMAX_PRIORITIES - 2, &tx_task_handle);
    
        int result = 0;
        EventBits_t bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("INIT_LINE", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("INIT_LINE", "Timeout waiting for response");
            result = -1;
        }
        stop_tasks = true;
        vTaskDelay(pdMS_TO_TICKS(500)); // Allow tasks to exit cleanly
    
        if (sema != NULL) {
            vSemaphoreDelete(sema);
            sema = NULL;
        }
        return result;
    }
    int erase()
    {
        int result = 0;
        stop_tasks = false;
        sema = xSemaphoreCreateBinary();
        if (sema == NULL) {
            ESP_LOGE("READOUT_UNPROTECT", "Failed to create semaphore");
            return -1;
        }
        TaskHandle_t rx_task_handle;
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        sendCommand(ERASE);
        EventBits_t bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("ERASE", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK1");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK1");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("ERASE", "Timeout waiting for response");
            result = -1;
        }
        sendCommand(0xff);
        bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("ERASE_GLOBAL", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK1");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK1");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("ERASE_GLOBAL", "Timeout waiting for response");
            result = -1;
        }
        stop_tasks = true;
        vTaskDelay(pdMS_TO_TICKS(500)); // Allow tasks to exit cleanly
        
        if (sema != NULL) {
            vSemaphoreDelete(sema);
            sema = NULL;
        }
        return result;
    }
    int readout_unprotect()
    {
        int result = 0;
        stop_tasks = false;
        sema = xSemaphoreCreateBinary();
        if (sema == NULL) {
            ESP_LOGE("READOUT_UNPROTECT", "Failed to create semaphore");
            return -1;
        }
        TaskHandle_t rx_task_handle;
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        sendCommand(READOUT_UNPROTECT);
        EventBits_t bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("READOUT_UNPROTECT", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK1");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK1");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("READOUT_UNPROTECT", "Timeout waiting for response");
            result = -1;
        }

        bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("READOUT_UNPROTECT", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK2");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK2");
                xEventGroupClearBits(event_group, ACK_BIT);
            }
        } else {
            ESP_LOGE("READOUT_UNPROTECT", "Timeout waiting for response");
            result = -1;
        }
        stop_tasks = true;
        vTaskDelay(pdMS_TO_TICKS(500)); // Allow tasks to exit cleanly
        
        if (sema != NULL) {
            vSemaphoreDelete(sema);
            sema = NULL;
        }
        return result;
    }
    int flash_firmware(char* path)
    {
        FILE* f = fopen(path, "rb");
        if (!f) {
            ESP_LOGE("FILE", "Failed to open file!");
        } else {
            ESP_LOGI("FILE", "File opened OK!");
            //fclose(f);
        }
        int32_t start = 0x08000000;
        //int32_t pos = 0; 
        int bytes_read = 0;
        uint8_t BLOCK[256];
        while ((bytes_read = fread(BLOCK, 1, 252, f)) > 0) 
        {
            printf("read %d bytes from file\n", bytes_read);
            printf("write at %ld\n", start);
            write_flash(start, bytes_read, BLOCK);
            start+= bytes_read;
        }
        printf("Done flash!!!!!!!!!!!!!\n");
        fclose(f);
            //free(BLOCK);
        return 0;
    }
    int write_flash(int32_t addr, uint8_t number, uint8_t* data)
    {
        int result = 0;
        stop_tasks = false;
        sema = xSemaphoreCreateBinary();
        if (sema == NULL) {
            ESP_LOGE("WRITE_MEM", "Failed to create semaphore");
            return -1;
        }
        TaskHandle_t rx_task_handle;
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        sendCommand(WRITE_MEM);
        EventBits_t bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("WRITE_MEM", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("WRITE_MEM", "Timeout waiting for response");
            result = -1;
        }
        uint8_t* address = (uint8_t*) malloc(5 * sizeof(uint8_t)); //MSB first
        uint8_t* address_bytes = (uint8_t*)&addr;
        for(int i = 0;i < 4; i++)address[i] = address_bytes[3-i];
        address[4] = checksum_addr(addr);
        sendDataBytes("WRITE MEM",address,5);
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        //sendCommand(WRITE_MEM);
        bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("WRITE_MEM", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK2");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK2");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("WRITE_MEM", "Timeout waiting for response");
            result = -1;
        }
        uint8_t checksum  = calculate_checksum(data, number);
        uint8_t number_clone = number -1;
        sendDataBytes("NUMBER", &number_clone, 1);
        sendDataBytes("FIRMWARE", data, number);
        sendDataBytes("CHECKSUM", &checksum, 1);
        xTaskCreate(rx_task, "uart_rx_task", 4096, NULL, configMAX_PRIORITIES - 1, &rx_task_handle);
        //
        // sendCommand(WRITE_MEM);
        bits = xEventGroupWaitBits(event_group,
            ACK_BIT | NACK_BIT,
            pdTRUE,  // clear on exit
            pdFALSE, // bất kỳ bit nào cũng được
            pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(sema, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI("WRITE_MEM", "Received response, line initialized");
            if (bits & ACK_BIT) {
                ESP_LOGI("TX_TASK", "Got ACK3");
                xEventGroupClearBits(event_group, ACK_BIT);
            } else if (bits & NACK_BIT) {
                ESP_LOGW("TX_TASK", "Got NACK3");
                xEventGroupClearBits(event_group, NACK_BIT);
            }
        } else {
            ESP_LOGE("WRITE_MEM", "Timeout waiting for response");
            result = -1;
        }
        stop_tasks = true;
        vTaskDelay(pdMS_TO_TICKS(500)); // Allow tasks to exit cleanly
        
        if (sema != NULL) {
            vSemaphoreDelete(sema);
            sema = NULL;
        }
        return result;
    }


