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
**Change information:**
- Change wifi information in 'Wifi_info.h' 
- Change download link in 'HttpDriveDL.c' in function void 'http_download_file(void)'
- Change cert_pem in 'HttpDriveDL.c' by changing 'google_drive_cert_pem'
- Get new certificate from google drive by command: 'openssl s_client -showcerts -connect drive.google.com:443 </dev/null > certs.txt'

