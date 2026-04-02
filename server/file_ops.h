#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "common.h"

int list_files(int sock, const char* username);
int upload_file(int sock, const char* username, const char* filename, size_t size);
int download_file(int sock, const char* username, const char* filename);
int delete_file(const char* username, const char* filename);

#endif