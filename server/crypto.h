#ifndef CRYPTO_H
#define CRYPTO_H

#include "common.h"

int derive_key(const char* password, const unsigned char* salt, unsigned char* key);
int encrypt_file(const char* input_file, const char* output_file, const unsigned char* key);
int decrypt_file(const char* input_file, const char* output_file, const unsigned char* key);
int send_message(int sock, Message* msg);
int recv_message(int sock, Message* msg);
int send_file(int sock, const char* filename);
int recv_file(int sock, const char* filename, size_t size);

#endif