**Pin configuration:**
| ESP32      | STM32      | 
|:-----------|:----------:|
|GPIO32 | BOOT0 |
|GPIO33 | BOOT1 |
|GPIO5 | RST    |
|P16(RX) | TX(PA9)|
|P17(TX) | RX(PA10)|
|GND| GND |
| 3v3 | 3v3|
------------

## Quick start: 
```bash
bash web/start_hotspot.sh
```
to turn on local wifi on server, then the Gateway will connect to the server (config wifi information in OTA_STM32/BootUART/main/inc/wifi_setup.h) 
```bash
bash web/start_server.sh
``` 
to start server

## demo: 
https://drive.google.com/drive/folders/1CsEFW0RzOwExHHpZ4V7I5nMbyGhRj8m9?usp=sharing

