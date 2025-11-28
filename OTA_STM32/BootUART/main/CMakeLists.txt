idf_component_register(SRCS "state_control.c" "json_control.c" "wifi_setup.c" "uart_command.c" "setup_SPIFFS.c" "http_connection.c" "main.c" "control.c" "nvs_control.c"
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(spiffs ../partition FLASH_IN_PROJECT)
