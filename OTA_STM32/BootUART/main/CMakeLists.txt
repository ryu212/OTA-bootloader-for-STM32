idf_component_register(SRCS "BootUART.c" "SetupSPIFFS.c" "HttpDriveDL.c" "WifiSetup.c" "BOOT_RESET.c" "Uart_command.c" 
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)
