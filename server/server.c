#include "common.h"
#include "auth.h"
#include "crypto.h"
#include "file_ops.h"

#include <ws2tcpip.h>
#include <windows.h>
#include <stdint.h>

SimpleUser* users = NULL;
int user_count = 0;

void handle_client(SOCKET client_sock) {
    char logged_in_user[MAX_USERNAME] = "";
    int authenticated = 0;

    while (1) {
        Message msg;
        if (recv_message(client_sock, &msg) <= 0) break;

        switch (msg.command) {
            case CMD_REGISTER: {
                if (register_user(msg.username, msg.password, &users, &user_count) == 0) {
                    create_user_directory(msg.username);
                    Message response = {CMD_REGISTER, "OK"};
                    send_message(client_sock, &response);
                    printf("User '%s' registered successfully.\n", msg.username);
                } else {
                    Message response = {CMD_REGISTER, "FAIL"};
                    send_message(client_sock, &response);
                    printf("Registration failed for user '%s' (username may already exist).\n", msg.username);
                }
                break;
            }
            case CMD_LOGIN: {
                if (authenticate_user(msg.username, msg.password, users, user_count)) {
                    strcpy(logged_in_user, msg.username);
                    authenticated = 1;
                    Message response = {CMD_LOGIN, "OK"};
                    send_message(client_sock, &response);
                    printf("User '%s' logged in successfully.\n", msg.username);
                } else {
                    Message response = {CMD_LOGIN, "FAIL"};
                    send_message(client_sock, &response);
                    printf("Login failed for user '%s' (invalid credentials).\n", msg.username);
                }
                break;
            }
            case CMD_UPLOAD: {
                if (!authenticated) break;
                if (upload_file(client_sock, logged_in_user, msg.filename, msg.file_size) == 0) {
                    Message response = {CMD_UPLOAD, "OK"};
                    send_message(client_sock, &response);
                    printf("User '%s' uploaded file '%s' (%lld bytes).\n", logged_in_user, msg.filename, (long long)msg.file_size);
                } else {
                    Message response = {CMD_UPLOAD, "FAIL"};
                    send_message(client_sock, &response);
                    printf("Upload failed for user '%s' on file '%s'.\n", logged_in_user, msg.filename);
                }
                break;
            }
            case CMD_DOWNLOAD: {
                if (!authenticated) break;
                if (download_file(client_sock, logged_in_user, msg.filename) == 0) {
                    Message response = {CMD_DOWNLOAD, "OK"};
                    send_message(client_sock, &response);
                    printf("User '%s' downloaded file '%s'.\n", logged_in_user, msg.filename);
                } else {
                    Message response = {CMD_DOWNLOAD, "FAIL"};
                    send_message(client_sock, &response);
                    printf("Download failed for user '%s' on file '%s' (file not found).\n", logged_in_user, msg.filename);
                }
                break;
            }
            case CMD_LIST: {
                if (!authenticated) break;
                list_files(client_sock, logged_in_user);
                printf("User '%s' requested file list.\n", logged_in_user);
                break;
            }
            case CMD_DELETE: {
                if (!authenticated) break;
                if (delete_file(logged_in_user, msg.filename) == 0) {
                    Message response = {CMD_DELETE, "OK"};
                    send_message(client_sock, &response);
                    printf("User '%s' deleted file '%s'.\n", logged_in_user, msg.filename);
                } else {
                    Message response = {CMD_DELETE, "FAIL"};
                    send_message(client_sock, &response);
                    printf("Delete failed for user '%s' on file '%s' (file not found).\n", logged_in_user, msg.filename);
                }
                break;
            }
            case CMD_LOGOUT: {
                authenticated = 0;
                strcpy(logged_in_user, "");
                Message response = {CMD_LOGOUT, "OK"};
                send_message(client_sock, &response);
                printf("User logged out.\n");
                break;
            }
            default:
                break;
        }
    }

    printf("Client disconnected.\n");
    closesocket(client_sock);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif

    load_users(&users, &user_count);

    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        int err = WSAGetLastError();
        fprintf(stderr, "Socket creation failed: %d\n", err);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %d\n", err);
        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        fprintf(stderr, "Bind failed: %d\n", err);
        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 10) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        fprintf(stderr, "Listen failed: %d\n", err);
        closesocket(server_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        SOCKET client_sock = accept(server_sock, NULL, NULL);
        if (client_sock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            fprintf(stderr, "Accept failed: %d\n", err);
            continue;
        }

        printf("New client connected.\n");
        handle_client(client_sock);
    }

    closesocket(server_sock);
    free(users);
    WSACleanup();
    return 0;
}