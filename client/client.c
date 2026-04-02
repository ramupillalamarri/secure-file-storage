#include "common.h"

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

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int authenticated = 0;

    printf("Secure File Storage Client\n");
    printf("Commands: register <user> <pass>, login <user> <pass>, upload <file>, download <file>, list, delete <file>, logout, quit\n");

    while (1) {
        printf("> ");
        char command[256];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        char* token = strtok(command, " ");
        if (!token) continue;

        Message msg = {0};

        if (strcmp(token, "register") == 0) {
            char* user = strtok(NULL, " ");
            char* pass = strtok(NULL, " ");
            if (user && pass) {
                msg.command = CMD_REGISTER;
                strcpy(msg.username, user);
                strcpy(msg.password, pass);
                send_message(sock, &msg);
                recv_message(sock, &msg);
                printf("Response: %s\n", msg.username);
            }
        } else if (strcmp(token, "login") == 0) {
            char* user = strtok(NULL, " ");
            char* pass = strtok(NULL, " ");
            if (user && pass) {
                msg.command = CMD_LOGIN;
                strcpy(msg.username, user);
                strcpy(msg.password, pass);
                strcpy(username, user);
                strcpy(password, pass);
                send_message(sock, &msg);
                recv_message(sock, &msg);
                if (strcmp(msg.username, "OK") == 0) {
                    authenticated = 1;
                    printf("Logged in\n");
                } else {
                    printf("Login failed\n");
                }
            }
        } else if (authenticated) {
            if (strcmp(token, "upload") == 0) {
                char* file = strtok(NULL, " ");
                if (file) {
                    struct stat st;
                    if (stat(file, &st) == 0) {
                        msg.command = CMD_UPLOAD;
                        strcpy(msg.filename, file);
                        msg.file_size = st.st_size;
                        send_message(sock, &msg);
                        send_file(sock, file);
                        recv_message(sock, &msg);
                        printf("Response: %s\n", msg.username);
                    } else {
                        printf("File not found\n");
                    }
                }
            } else if (strcmp(token, "download") == 0) {
                char* file = strtok(NULL, " ");
                if (file) {
                    msg.command = CMD_DOWNLOAD;
                    strcpy(msg.filename, file);
                    send_message(sock, &msg);
                    recv_message(sock, &msg);
                    if (msg.file_size > 0) {
                        recv_file(sock, file, msg.file_size);
                        printf("Downloaded %s\n", file);
                    } else {
                        printf("File not found\n");
                    }
                }
            } else if (strcmp(token, "list") == 0) {
                msg.command = CMD_LIST;
                send_message(sock, &msg);
                recv_message(sock, &msg);
                printf("Files:\n%s", msg.filename);
            } else if (strcmp(token, "delete") == 0) {
                char* file = strtok(NULL, " ");
                if (file) {
                    msg.command = CMD_DELETE;
                    strcpy(msg.filename, file);
                    send_message(sock, &msg);
                    recv_message(sock, &msg);
                    printf("Response: %s\n", msg.username);
                }
            } else if (strcmp(token, "delete") == 0) {
                char* file = strtok(NULL, " ");
                if (file) {
                    msg.command = CMD_DELETE;
                    strcpy(msg.filename, file);
                    send_message(sock, &msg);
                    recv_message(sock, &msg);
                    printf("Response: %s\n", msg.username);
                }
            } else if (strcmp(token, "logout") == 0) {
                msg.command = CMD_LOGOUT;
                send_message(sock, &msg);
                recv_message(sock, &msg);
                authenticated = 0;
                printf("Logged out\n");
            } else if (strcmp(token, "quit") == 0) {
                break;
            } else {
                printf("Please login first\n");
            }
    }

    close(sock);
    return 0;
}