#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#undef close
#undef mkdir
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_FILENAME 256
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define SALT_SIZE 16
#define KEY_SIZE 32
#define IV_SIZE 16

// Command codes
#define CMD_REGISTER 1
#define CMD_LOGIN 2
#define CMD_UPLOAD 3
#define CMD_DOWNLOAD 4
#define CMD_LIST 5
#define CMD_DELETE 6
#define CMD_LOGOUT 7

typedef struct {
    int command;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char filename[MAX_FILENAME];
    size_t file_size;
} Message;

int send_message(int sock, Message* msg);
int recv_message(int sock, Message* msg);
int send_file(int sock, const char* filename);
int recv_file(int sock, const char* filename, size_t size);

#endif