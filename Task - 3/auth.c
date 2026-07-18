#include "globals.h"
#include "audit.h"
#include "auth.h"

//Initializing the system 
void initialize_system() {
    
    // Establishing the absolute root administrator account (UID 0, Group 0)
    users[totalUsers].user_id = 0;
    users[totalUsers].group_id = 0;
    strcpy(users[totalUsers].username, "root");
    strcpy(users[totalUsers].password, "root123");
    totalUsers+=1;

    printf("||SYSTEM INITIALIZATION|| Mock database created successfully.\n");
    printf("||SYSTEM INITIALIZATION|| Accounts built: 'root' (ID 0).\n\n");
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