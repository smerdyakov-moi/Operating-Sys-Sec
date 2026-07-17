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
struct Permission{
    bool read;
    bool write;
    bool execute;
};

//Grouping permissions into 3 identities: Owner (Primarily root priviliges), Group and Others   
struct FilePermission{
    struct Permission owner;
    struct Permission group;
    struct Permission others;
};

// Holding user account data
struct User{
    int user_id;
    int group_id;
    char username[MAX_STR];
    char password[MAX_STR];
};


int main(){

    return 0;
}
