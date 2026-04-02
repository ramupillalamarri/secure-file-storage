#include "common.h"

#define USERS_FILE "users.dat"

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} SimpleUser;

int load_users(SimpleUser** users, int* count) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (!file) {
        *users = NULL;
        *count = 0;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *count = size / sizeof(SimpleUser);
    *users = malloc(*count * sizeof(SimpleUser));
    fread(*users, sizeof(SimpleUser), *count, file);
    fclose(file);
    return 0;
}

int save_users(SimpleUser* users, int count) {
    FILE* file = fopen(USERS_FILE, "wb");
    if (!file) return -1;

    fwrite(users, sizeof(SimpleUser), count, file);
    fclose(file);
    return 0;
}

int authenticate_user(const char* username, const char* password, SimpleUser* users, int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

int register_user(const char* username, const char* password, SimpleUser** users, int* count) {
    for (int i = 0; i < *count; i++) {
        if (strcmp((*users)[i].username, username) == 0) {
            return -1; // User exists
        }
    }

    *users = realloc(*users, (*count + 1) * sizeof(SimpleUser));
    strcpy((*users)[*count].username, username);
    strcpy((*users)[*count].password, password);

    (*count)++;
    save_users(*users, *count);
    return 0;
}

void create_user_directory(const char* username) {
    char path[512];
    snprintf(path, sizeof(path), "storage/%s", username);
    mkdir(path);
}