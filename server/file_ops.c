#include "common.h"

int list_files(int sock, const char* username) {
    char path[512];
    snprintf(path, sizeof(path), "storage/%s", username);

    DIR* dir = opendir(path);
    if (!dir) {
        Message msg = {0};
        send_message(sock, &msg); // Send empty message to indicate no files
        return -1;
    }

    struct dirent* entry;
    char file_list[BUFFER_SIZE] = "";
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            strcat(file_list, entry->d_name);
            strcat(file_list, "\n");
        }
    }
    closedir(dir);

    Message msg = {0};
    strcpy(msg.filename, file_list);
    send_message(sock, &msg);
    return 0;
}

int upload_file(int sock, const char* username, const char* filename, size_t size) {
    char user_path[512];
    snprintf(user_path, sizeof(user_path), "storage/%s", username);

    char temp_file[512];
    snprintf(temp_file, sizeof(temp_file), "%s/%s", user_path, filename);

    if (recv_file(sock, temp_file, size) != 0) {
        return -1;
    }

    return 0;
}

int download_file(int sock, const char* username, const char* filename) {
    char user_path[512];
    snprintf(user_path, sizeof(user_path), "storage/%s", username);

    char enc_file[512];
    snprintf(enc_file, sizeof(enc_file), "%s/%s", user_path, filename);

    if (access(enc_file, F_OK) != 0) {
        return -1; // File not found
    }

    struct stat st;
    stat(enc_file, &st);
    Message msg = {0};
    msg.file_size = st.st_size;
    send_message(sock, &msg);

    if (send_file(sock, enc_file) != 0) {
        return -1;
    }

    return 0;
}

int delete_file(const char* username, const char* filename) {
    char enc_file[512];
    snprintf(enc_file, sizeof(enc_file), "storage/%s/%s", username, filename);
    return remove(enc_file);
}