#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>

// Limiting string sizes to prevent input overflow
#define MAX_CONTENT 512
#define MAX_USERS 10
#define MAX_FILES 20
#define MAX_STR  32

//Structure detailing permissions holding basic true/false flags for file access
typedef struct {

    bool read;
    bool write;
    bool execute;

} Permission;

//Grouping permissions into 3 identities: Owner (Primarily root priviliges), Group and Others   
typedef struct {

    Permission owner;
    Permission group;
    Permission others;

} FilePermission;

// Holding user account data
typedef struct {

    int user_id;
    int group_id;
    char username[MAX_STR];
    char password[MAX_STR];

} User;

typedef struct {

    char filename[MAX_STR];
    char content[MAX_CONTENT];
    int size;
    int owner_id;
    int group_id;
    FilePermission perm;
    bool isEncrypted; // Flagging true only if the file's encrypted

} VirtualFile;

int main(){

    return 0;
}
