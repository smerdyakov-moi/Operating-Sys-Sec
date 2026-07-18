#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CONTENT 512
#define MAX_USERS 10
#define MAX_FILES 20
#define MAX_STR 32
#define CIPHER_KEY 0x5A

typedef struct{
    bool read;
    bool write;
    bool execute;
}Permission;

typedef struct{
    Permission owner;
    Permission group;
    Permission others;
}FilePermission;

typedef struct{
    int user_id;
    int group_id;
    char username[MAX_STR];
    char password[MAX_STR];
}User;

typedef struct{
    char filename[MAX_STR];
    char content[MAX_CONTENT];
    int size;
    int owner_id;
    int group_id;
    FilePermission perm;
    bool isEncrypted;
}VirtualFile;

extern int next_uid;

extern User users[MAX_USERS];
extern int totalUsers;

extern VirtualFile files[MAX_FILES];
extern int totalFiles;

extern int current_uid;
extern int current_groupid;

#endif