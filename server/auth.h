#ifndef AUTH_H
#define AUTH_H

#include "common.h"

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
} SimpleUser;

int load_users(SimpleUser** users, int* count);
int save_users(SimpleUser* users, int count);
int authenticate_user(const char* username, const char* password, SimpleUser* users, int count);
int register_user(const char* username, const char* password, SimpleUser** users, int* count);
void create_user_directory(const char* username);

#endif