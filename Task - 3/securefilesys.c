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

// Masking byte - Binary Pattern to scramble characters via bitwise XOR operations
#define CIPHER_KEY 0x5A;

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

// Helper function to track user logs (In append mode so as to keep track of older history logs)
void audit_action(char *username, char *status, char *action){
    FILE *logfile = fopen("audit.log","a"); //APPEND MODE
    if (logfile == NULL){
        printf("Failed to access audit log! \n");
        return;
    }

    //Grabing current system time
    time_t raw_time = time(NULL);
    char *time_str = ctime(&raw_time);
    time_str[strlen(time_str) - 1] = '\0';

    //Printing formatted security string into the audit log/file
    fprintf(logfile, "[%s] USER: %s || ACTION: %s || STATUS: %s \n",time_str,username,action,status);

    fclose(logfile);
}

void login_user(char *username, char *password){
    // If the input strings are too long (i.e over the limit of MAX_STR), it's rejected immediately
    if(strlen(username)>=MAX_STR || strlen(password)>=MAX_STR){
        printf("||BLOCKED|| Input size exceeds limits! Rejected incoming request! \n");
        audit_action("RISK","INPUT TOO LENGTHY","LOGIN");
        return;
    }

    //Looping through the user database (array) to check whether username/pw combo exists or not
    for(int i = 0 ; i < totalUsers ; i ++){
        if(strcmp(users[i].username,username) == 0) {
            if(strcmp(users[i].password,password) == 0){
                current_uid = users[i].user_id;
                current_groupid = users[i].group_id;

                printf("Welcome back: %s. Logged In successfully with UID: %d \n", username,current_uid);
                audit_action(username,"SUCCESS","LOGIN");
                return;
            }
        }
    }
    printf("Incorrect username/password credentials! \n");
    audit_action(username,"INCORRECT CREDS", "LOGIN");
}

// Helper function to trace active user
char* current_active_user(){
    for (int i = 0; i < totalUsers ; i++ ){
        if(users[i].user_id == current_uid){
            return users[i].username;
        }
    }
    return "GUEST"; // Default state if no user is currently logged in
}

void logout_user(){
    if(current_uid == -1){
        printf("No active user session! \n");
        return;
    }

    char active_user[MAX_STR]="DEFAULT";
    strcpy(active_user, current_active_user()); // FIX: Added function execution parentheses ()

    audit_action(active_user,"SUCCESS","LOGOUT");

    //Resetting the current user/group id after logging out
    current_uid = -1;
    current_groupid = -1;

    printf("Logged out successfully! \n");
}

bool eval_permissions(int file_index, char mode){

    // Root bypasses all the permission restrictions
    if (current_uid == 0 ){
        return true;
    }

    // Evaluating whether logged in user is the owner of the file
    if (current_uid == files[file_index].owner_id){
        if (mode == 'r'){ return files[file_index].perm.owner.read;}
        if (mode == 'w'){ return files[file_index].perm.owner.write;}
        if (mode == 'x'){ return files[file_index].perm.owner.execute;}
    }

    // Evaluating whether user belongs to the file's assigned group or not
    if (current_groupid == files[file_index].group_id){
        if (mode == 'r'){ return files[file_index].perm.group.read;}
        if (mode == 'w'){ return files[file_index].perm.group.write;}
        if (mode == 'x'){ return files[file_index].perm.group.execute;}
    }

    // Evaluating for all other users
    if (mode == 'r') return files[file_index].perm.others.read;
    if (mode == 'w') return files[file_index].perm.others.write;
    if (mode == 'x') return files[file_index].perm.others.execute;

    return false;
}

void file_creation(char *filename, char *content, FilePermission perm){

    // Only with an active user session can someone successfully create files
    if (current_uid == -1){
        printf("||ERROR|| File Creation denied! Not logged in! \n");
        audit_action("GUEST","INVALID","CREATE_FILE");
        return;
    }

    if (strlen(filename) >= MAX_STR || strlen(content) >= MAX_CONTENT) {
        printf("||BLOCKED|| Input size exceeds system storage limits! File creation rejected.\n\n");
        audit_action("SYSTEM", "INPUT TOO LENGTHY", "CREATE FILE");
        return;
    }

    // Check whether the file memory array is full or not
    if (totalFiles >= MAX_FILES){
        printf("Virtual disk capacity reached! \n");
        return;
    }

    strcpy(files[totalFiles].filename, filename);
    strcpy(files[totalFiles].content, content);
    files[totalFiles].size = strlen(content);
    files[totalFiles].owner_id = current_uid;
    files[totalFiles].group_id = current_groupid;
    files[totalFiles].perm = perm;
    files[totalFiles].isEncrypted = false; // Initially, the status is unencrypted plaintext

    totalFiles+=1;

    char active_user[MAX_STR]="DEFAULT";
    strcpy(active_user, current_active_user()); 

    printf("||SUCCESS|| File: '%s' created by UID: %d \n",filename,current_uid);
    audit_action(active_user,"SUCCESS","CREATE FILE");
}

void file_read(char *filename){
    char active_user[MAX_STR]="DEFAULT";
    strcpy(active_user, current_active_user()); 
                    
    for (int i = 0; i < totalFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {

            // Check if user is authorized to read this specific file
            if (!eval_permissions(i, 'r')) {
                printf("||SECURITY BLOCKED|| Access Denied: You do not have READ privileges for '%s'!\n", filename);
                audit_action(active_user,"ACCESS DENIED", "READ FILE");
                return;
            }

            printf("READ SUCCESS: Content of '%s':\n  -->   %s\n", files[i].filename, files[i].content);
            audit_action(active_user, "SUCCESS", "READ FILE");
            return;
        }
    }
    printf("ERROR: File '%s' not found in system storage.\n", filename);
}

void file_write(char *filename, char *new_content){
    if (strlen(new_content) >= MAX_CONTENT) {
        printf("||BLOCKED|| Content length exceeds allocation buffer limit!\n");
        return;
    }

    char active_user[MAX_STR]="DEFAULT";
    strcpy(active_user, current_active_user());

    for (int i = 0; i < totalFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {
            // Check if user is authorized to write to this file
            if (!eval_permissions(i, 'w')) {
                printf("||SECURITY BLOCKED|| You do not have WRITE privileges for '%s'!\n", filename);
                audit_action(active_user, "ACCESS DENIED", "WRITE FILE");
                return;
            }

            strcpy(files[i].content, new_content);
            files[i].size = strlen(new_content);
            printf("WRITE SUCCESS: File '%s' has been updated.\n", filename);
            audit_action(active_user, "SUCCESS", "WRITE FILE");
            return;
        }
    }
    printf("ERROR: File '%s' not found.\n", filename);
}


void file_delete(char *filename){
    char active_user[MAX_STR]="DEFAULT";
    strcpy(active_user, current_active_user()); 

    for (int i = 0; i < totalFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {
            // Rule: Only the owner or root can completely delete a file
            if (current_uid != 0 && current_uid != files[i].owner_id) {
                printf("||SECURITY BLOCKED|| Deletion Denied: Only the owner or root can delete '%s'!\n\n", filename);
                audit_action(active_user, "DELETE DENIED", "FILE DELETE");
                return;
            }

            // Shift all subsequent files left to fill the empty array slot gap cleanly
            for (int j = i; j < totalFiles - 1; j++) {
                files[j] = files[j + 1];
            }
            totalFiles--;

            printf("DELETE SUCCESS: File '%s' has been permanently erased.\n", filename);
            audit_action(active_user, "SUCCESS", "FILE DELETE");
            return;
        }
    }
    printf("ERROR: File '%s' not found.\n", filename);
}

int main(){

    //Booting up mock environment data structures
    initialize_system();

    // Defining basic permissions
    // Owner/root can read/write whereas group can read only and others don't have any priviliges.
    FilePermission restricted_mask;
    restricted_mask.owner.read = true;   restricted_mask.owner.write = true;   restricted_mask.owner.execute = false;
    restricted_mask.group.read = true;   restricted_mask.group.write = false;  restricted_mask.group.execute = false;
    restricted_mask.others.read = false;  restricted_mask.others.write = false; restricted_mask.others.execute = false;

    if(current_uid == -1){
        printf("NO USER CURRENTLY LOGGED IN! \n");
    }

    // Running Sample Tests for initial mock setup
    
    // Anonymous Unauthenticated Breaches
    printf("--- [TEST A] Attempting actions as an unauthenticated guest ---\n");
    file_creation("hack.txt", "Malicious inject payload data.", restricted_mask); // Should be denied!
    printf("\n");

    // Admin Setup Operations
    printf("--- [TEST B] Authenticating as Administrator ---\n");
    login_user("root", "root123");
    file_creation("clearance_secrecy.txt", "CONFIDENTIAL: Financial ledger records 2026.", restricted_mask);
    file_read("clearance_secrecy.txt"); // Should grant full access via root privileges
    
    logout_user();

    // Unauthorized User Access Privilege Breach
    printf("--- [TEST C] Authenticating as Restricted User ---\n");
    login_user("guest", "guest123");
    
    printf("[Attempting Read Action...]\n");
    file_read("clearance_secrecy.txt"); // Security Blocked triggered

    printf("[Attempting Write-Corrupt Action...]\n");
    file_write("clearance_secrecy.txt", "WIPING OUT SYSTEM LOG DATA!"); // Security Blocked triggered

    printf("[Attempting Deletion Sabotage...]\n");
    file_delete("clearance_secrecy.txt"); // Security Blocked triggered
    
    logout_user();

    return 0;
}