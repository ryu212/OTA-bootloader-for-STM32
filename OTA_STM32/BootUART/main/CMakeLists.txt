idf_component_register(SRCS "wifi_setup.c" "uart_command.c" "setup_SPIFFS.c" "http_connection.c" "main.c" "control.c" 
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)
