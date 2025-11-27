#ifndef SPIFFS_SETUP_H
#define SPIFFS_SETUP_H

void setupSPIFFS();
void create_file_if_not_exists(const char *path);
void copy_file(const char* dst, const char* src);
#endif