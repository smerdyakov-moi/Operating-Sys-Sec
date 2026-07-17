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

// Simulating arrays as our database

User users[MAX_USERS]; //
int totalUsers = 0; //Initially set up with 0 users

VirtualFile files[MAX_FILES];
int totalFiles = 0;

//Security variables for tracking the currently logged in user/group (-1 refers to no active login)
int current_uid = -1;
int current_groupid = -1;

//Initializing the system 
void initialize_system() {
    
    // Establishing the absolute root administrator account (UID 0, Group 0)
    users[totalUsers].user_id = 0;
    users[totalUsers].group_id = 0;
    strcpy(users[totalUsers].username, "root");
    strcpy(users[totalUsers].password, "root123");
    totalUsers++;

    // Establishing a standard unprivileged user account for access control testing (UID 1001, Group 1001)
    users[totalUsers].user_id = 1001;
    users[totalUsers].group_id = 1001;
    strcpy(users[totalUsers].username, "guest");
    strcpy(users[totalUsers].password, "guest123");
    totalUsers++;

    printf("||SYSTEM INITIALIZATION|| Mock database created successfully.\n");
    printf("||SYSTEM INITIALIZATION|| Accounts built: 'root' (ID 0) and 'guest' (ID 1001).\n\n");
}

void login_user(char *username, char *password){
    // If the input strings are too long (i.e over the limit of MAX_STR), it's rejected immediately
    if(strlen(username)>=MAX_STR || strlen(password)>=MAX_STR){
        printf("||BLOCKED|| Input size exceeds limits! Rejected incoming request! \n");
    }

    //Looping through the user database (array) to check whether username/pw combo exists or not
    for(int i = 0 ; i < totalUsers ; i ++){
        if(strcmp(users[i].username,username) == 0) {
            if(strcmp(users[i].password,password) == 0){
                current_uid = users[i].user_id;
                current_groupid = users[i].group_id;

                printf("Welcome back: %s. Logged In successfully with UID: %d \n", username,current_uid);
                return;
            }
        }
    }
    printf("Incorrect username/password credentials! \n");
}

void logout_user(){
    if(current_uid == -1){
        printf("No active user session! \n");
        return;
    }


    current_uid = -1;
    current_groupid = -1;

    printf("Logged out successfully! \n");
}

int main(){

    //Booting up mock environment data structures
    initialize_system();

    if(current_uid == -1){
        printf("NO USER CURRENTLY LOGGED IN! \n");
    }

    return 0;
}
