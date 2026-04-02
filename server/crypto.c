#include "common.h"

int derive_key(const char* password, const unsigned char* salt, unsigned char* key) {
    // Dummy
    memcpy(key, password, strlen(password));
    return 0;
}

int encrypt_file(const char* input_file, const char* output_file, const unsigned char* key) {
    // Just copy for demo
    FILE* in = fopen(input_file, "rb");
    if (!in) return -1;
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        fclose(in);
        return -1;
    }
    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        fwrite(buffer, 1, bytes, out);
    }
    fclose(in);
    fclose(out);
    return 0;
}

int decrypt_file(const char* input_file, const char* output_file, const unsigned char* key) {
    // Just copy
    return encrypt_file(input_file, output_file, key);
}

int send_message(int sock, Message* msg) {
    return send(sock, (char*)msg, sizeof(Message), 0);
}

int recv_message(int sock, Message* msg) {
    return recv(sock, (char*)msg, sizeof(Message), 0);
}

int send_file(int sock, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return -1;

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) != bytes_read) {
            fclose(file);
            return -1;
        }
    }
    fclose(file);
    return 0;
}

int recv_file(int sock, const char* filename, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    char buffer[BUFFER_SIZE];
    size_t total_received = 0;
    int bytes_read;
    while (total_received < size) {
        bytes_read = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            fclose(file);
            return -1;
        }
        fwrite(buffer, 1, bytes_read, file);
        total_received += bytes_read;
    }
    fclose(file);
    return 0;
}