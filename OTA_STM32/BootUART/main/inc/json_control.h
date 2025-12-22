#ifndef JSON_CONTROL_H
#define JSON_CONTROL_H
#include <stdio.h>
#include "esp_vfs.h"      // <== nếu dùng file system SPIFFS/LittleFS
#include "cJSON.h"
#include <stdint.h>
uint32_t dump_json_int(const char* path, const char* key);
char* dump_json_str(const char* path, const char* key);

#endif
