#include "websocket.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

static const char *TAG = "WS_CLIENT";
static esp_websocket_client_handle_t client = NULL;

static void websocket_event_handler(void *handler_args, esp_event_base_t base,
                                    int32_t event_id, void *event_data) {
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch(event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(TAG, "Received: %.*s", data->data_len, (char*)data->data_ptr);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGE(TAG, "WebSocket error");
            break;
        default:
            break;
    }
}

esp_err_t ws_client_init(const char *uri) {
    if(client != NULL) return ESP_OK; // đã init

    esp_websocket_client_config_t cfg = {
        .uri = uri
    };
    client = esp_websocket_client_init(&cfg);
    if(client == NULL) return ESP_FAIL;

    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void*)client);
    return esp_websocket_client_start(client);
}

esp_err_t ws_client_send(const char *msg) {
    if(client == NULL) return ESP_FAIL;
    return esp_websocket_client_send_text(client, msg, strlen(msg), portMAX_DELAY);
}

esp_err_t ws_client_deinit(void) {
    if(client == NULL) return ESP_OK;
    esp_websocket_client_stop(client);
    esp_websocket_client_destroy(client);
    client = NULL;
    return ESP_OK;
}

void socket_send_percent(int percent)
{
    char socket_msg[128];  // local variable
    snprintf(socket_msg, sizeof(socket_msg),
             "{\"progress\": %d, \"stage\": \"stm32_flashing\"}", percent);
    ws_client_send(socket_msg);
}