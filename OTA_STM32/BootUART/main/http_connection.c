#include "http_connection.h"
#include "esp_crc.h"
#include "json_control.h"
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>

// Thêm vào đầu file

/*
static const char *google_drive_cert_pem = \
"-----BEGIN CERTIFICATE-----\n"
"MIIFYjCCBEqgAwIBAgIQd70NbNs2+RrqIQ/E8FjTDTANBgkqhkiG9w0BAQsFADBX\n"
"MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\n"
"CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIwMDYx\n"
"OTAwMDA0MloXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\n"
"GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFIx\n"
"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAthECix7joXebO9y/lD63\n"
"ladAPKH9gvl9MgaCcfb2jH/76Nu8ai6Xl6OMS/kr9rH5zoQdsfnFl97vufKj6bwS\n"
"iV6nqlKr+CMny6SxnGPb15l+8Ape62im9MZaRw1NEDPjTrETo8gYbEvs/AmQ351k\n"
"KSUjB6G00j0uYODP0gmHu81I8E3CwnqIiru6z1kZ1q+PsAewnjHxgsHA3y6mbWwZ\n"
"DrXYfiYaRQM9sHmklCitD38m5agI/pboPGiUU+6DOogrFZYJsuB6jC511pzrp1Zk\n"
"j5ZPaK49l8KEj8C8QMALXL32h7M1bKwYUH+E4EzNktMg6TO8UpmvMrUpsyUqtEj5\n"
"cuHKZPfmghCN6J3Cioj6OGaK/GP5Afl4/Xtcd/p2h/rs37EOeZVXtL0m79YB0esW\n"
"CruOC7XFxYpVq9Os6pFLKcwZpDIlTirxZUTQAs6qzkm06p98g7BAe+dDq6dso499\n"
"iYH6TKX/1Y7DzkvgtdizjkXPdsDtQCv9Uw+wp9U7DbGKogPeMa3Md+pvez7W35Ei\n"
"Eua++tgy/BBjFFFy3l3WFpO9KWgz7zpm7AeKJt8T11dleCfeXkkUAKIAf5qoIbap\n"
"sZWwpbkNFhHax2xIPEDgfg1azVY80ZcFuctL7TlLnMQ/0lUTbiSw1nH69MG6zO0b\n"
"9f6BQdgAmD06yK56mDcYBZUCAwEAAaOCATgwggE0MA4GA1UdDwEB/wQEAwIBhjAP\n"
"BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTkrysmcRorSCeFL1JmLO/wiRNxPjAf\n"
"BgNVHSMEGDAWgBRge2YaRQ2XyolQL30EzTSo//z9SzBgBggrBgEFBQcBAQRUMFIw\n"
"JQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnBraS5nb29nL2dzcjEwKQYIKwYBBQUH\n"
"MAKGHWh0dHA6Ly9wa2kuZ29vZy9nc3IxL2dzcjEuY3J0MDIGA1UdHwQrMCkwJ6Al\n"
"oCOGIWh0dHA6Ly9jcmwucGtpLmdvb2cvZ3NyMS9nc3IxLmNybDA7BgNVHSAENDAy\n"
"MAgGBmeBDAECATAIBgZngQwBAgIwDQYLKwYBBAHWeQIFAwIwDQYLKwYBBAHWeQIF\n"
"AwMwDQYJKoZIhvcNAQELBQADggEBADSkHrEoo9C0dhemMXoh6dFSPsjbdBZBiLg9\n"
"NR3t5P+T4Vxfq7vqfM/b5A3Ri1fyJm9bvhdGaJQ3b2t6yMAYN/olUazsaL+yyEn9\n"
"WprKASOshIArAoyZl+tJaox118fessmXn1hIVw41oeQa1v1vg4Fv74zPl6/AhSrw\n"
"9U5pCZEt4Wi4wStz6dTZ/CLANx8LZh1J7QJVj2fhMtfTJr9w4z30Z209fOU0iOMy\n"
"+qduBmpvvYuR7hZL6Dupszfnw0Skfths18dG9ZKb59UhvmaSGZRVbNQpsg3BZlvi\n"
"d0lIKO2d1xozclOzgjXPYovJJIultzkMu34qQb9Sz/yilrbCgj8=\n"
"-----END CERTIFICATE-----\n";
*/
char* current_ver = NULL;
char* update_ver = NULL; 
uint32_t checksum_cal = 0; 
uint32_t checksum_get = 0;
esp_err_t _http_event_handler_firmware(esp_http_client_event_t *evt)
{
    static FILE *file = NULL;
    static int total_len = 0xffffffff;

    switch (evt->event_id) {
        case HTTP_EVENT_ON_CONNECTED:
            checksum_cal = 0xffffffff;
            ESP_LOGI("HTTP", "HTTP Connected, opening file for writing");
            file = fopen("/spiffs/OTA0/firmware.bin", "w+b");
            if (!file) {
                ESP_LOGE("HTTP", "Failed to open file for writing");
                return ESP_FAIL;
            }
            else 
                ESP_LOGI("HTTP", "Open file for writing successfully!!!");
            total_len = 0;
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI("HTTP", "HTTP data received, len=%d", evt->data_len);
            checksum_cal = esp_crc32_le(checksum_cal, evt->data, evt->data_len);
            if (file) {
                fwrite(evt->data, 1, evt->data_len, file);
                total_len += evt->data_len;
            }
            break;
        
        case HTTP_EVENT_DISCONNECTED:
            // checksum_cal &= 0xffffffff;
            checksum_cal ^= 0xFFFFFFFF;
            ESP_LOGI("CHECKSUM", "final checksum cal: 0x%" PRIX32, checksum_cal);
            ESP_LOGI("HTTP", "HTTP Disconnected, closing file");
            if (file) {
                fclose(file);
                file = NULL;
                ESP_LOGI("HTTP", "File downloaded, total length=%d", total_len);
            }

            break;
        default:
            break;
    }
    return ESP_OK;
}


int http_download_firmware(void) {
    http_download_version();
    
    esp_http_client_config_t config = {
        .url = "http://10.42.0.1:5000/firmware/latest.bin", // Thay ID neu doi file 
        .method = HTTP_METHOD_GET, 
        .event_handler = _http_event_handler_firmware, 
        // .cert_pem = google_drive_cert_pem, 
        
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("HTTP", "HTTP Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE("HTTP", "HTTP request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    if((checksum_cal == checksum_get))
    {
        ESP_LOGI("CHECKSUM", "CRC32 check good");
        ESP_LOGI("CHECKSUM", "final checksum cal: 0x%" PRIX32, checksum_cal);
        ESP_LOGI("CHECKSUM", "checksum get: 0x%" PRIX32, checksum_get);
        return 0;
    }
    
    ESP_LOGW("CHECKSUM", "CRC32 check bad");
    ESP_LOGI("CHECKSUM", "final checksum cal: 0x%" PRIX32, checksum_cal);
    ESP_LOGI("CHECKSUM", "checksum get: 0x%" PRIX32, checksum_get);
    return -1;
}


esp_err_t _http_event_handler_version(esp_http_client_event_t *evt)
{
    // if(update_ver)free(update_ver); 
    static FILE *file = NULL;
    static int total_len = 0;
    switch (evt->event_id) {
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("HTTP", "HTTP Connected, opening file for writing");
            file = fopen("/spiffs/version.json", "w+b");
            if (!file) {
                ESP_LOGE("HTTP", "Failed to open file for writing");
                return ESP_FAIL;
            }
            else 
                ESP_LOGI("HTTP", "Open file for writing successfully!!!");
            total_len = 0;
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI("HTTP", "HTTP data received, len=%d", evt->data_len);
            if (file) {
                fwrite(evt->data, 1, evt->data_len, file);
                total_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_DISCONNECTED:

            ESP_LOGI("HTTP", "HTTP Disconnected, closing file");
            if (file) {
                fclose(file);
                file = NULL;
                ESP_LOGI("HTTP", "File downloaded, total length=%d", total_len);
            }
            update_ver = dump_json_str("/spiffs/version.json", "version");
            checksum_get = dump_json_int("/spiffs/version.json", "checksum");
            break;

        default:
            break;
    }
    return ESP_OK;
}


void http_download_version(void) {
    esp_http_client_config_t config = {
        .url = "http://10.42.0.1:5000/firmware/version.json", // Thay ID neu doi file 
        .method = HTTP_METHOD_GET, 
        .event_handler = _http_event_handler_version, 
        // .cert_pem = google_drive_cert_pem, 
        
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI("HTTP", "HTTP Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE("HTTP", "HTTP request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}


bool file_exists(const char *path)
{
    struct stat st;
    return (stat(path, &st) == 0);
}
bool new_version()
{
    if(!file_exists("/spiffs/version.json"))
        current_ver = "";
    else
        current_ver = dump_json_str("/spiffs/version.json", "version");

    if (!current_ver)
        current_ver = "";

    http_download_version();
    
    printf("current ver: %s, update ver: %s\n", current_ver, update_ver);
    if(!current_ver)return 1;
    return (strcmp(update_ver, current_ver) != 0); 
}