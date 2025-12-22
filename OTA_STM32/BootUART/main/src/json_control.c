#include "json_control.h"

uint32_t dump_json_int(const char* path, const char* key)
{
    FILE* f = fopen(path, "r");
    if (!f) {
        printf("Failed to open file: %s\n", path);
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    cJSON* json = cJSON_Parse(buffer);
    free(buffer);

    if (!json) {
        printf("JSON parse int error in file path: %s\n", path);
        return 0;
    }

    cJSON* item = cJSON_GetObjectItem(json, key);
    if (!cJSON_IsNumber(item)) {
        printf("Key not found or not integer\n");
        cJSON_Delete(json);
        return 0;
    }

    uint32_t value = (uint32_t)item->valuedouble;
    cJSON_Delete(json);
    return value;
}


char* dump_json_str(const char* path, const char* key)
{
    FILE* f = fopen(path, "r");
    if (!f) {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    cJSON* json = cJSON_Parse(buffer);
    free(buffer);

    if (!json) {
        printf("JSON parse string error in file path: %s\n", path);
        return "0";
    }

    cJSON* item = cJSON_GetObjectItem(json, key);
    if (!cJSON_IsString(item)) {
        printf("Key not found or not string\n");
        cJSON_Delete(json);
        return "0";
    }

    char* value = strdup(item->valuestring);  // cấp phát chuỗi
    cJSON_Delete(json);
    return value;
}
