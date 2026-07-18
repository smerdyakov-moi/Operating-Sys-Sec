#include<stdio.h>
#include "globals.h"
#include "auth.h"
#include "filesystem.h"
#include "users.h"
#include "audit.h"


int main(){

    // Booting up backend structures
    initialize_system();

    // Registering terminal inputs
    char command[MAX_STR]; 
    char arg1[MAX_STR]; // username
    char arg2[MAX_CONTENT]; //password
    int int_arg; //group id

    printf("<------------------------------------------------------->\n\n");
    printf("                     VIRTUAL OS ENVIRONMENT              \n\n");
    printf("    Commands: login,logout,adduser,create,read,write     \n\n");
    printf("          delete,encrypt,dump,removeuser,exit            \n\n");
    printf("<------------------------------------------------------->\n\n");
    
    while (true){
        printf("[%s@virtualOS]",current_active_user());

        if (scanf("%s",command) == EOF) break;

        if (strcmp(command, "login") == 0) {
            printf("Enter Username: ");
            scanf("%s", arg1);
            printf("Enter Password: ");
            scanf("%s", arg2);
            login_user(arg1, arg2);

        } else if (strcmp(command, "logout") == 0) {
            logout_user();

        } else if (strcmp(command, "adduser") == 0) {
            printf("Enter New Username: ");
            scanf("%s", arg1);
            printf("Enter New Password: ");
            scanf("%s", arg2);
            printf("Assign Group ID (Integer): ");
            scanf("%d", &int_arg);
            user_create(arg1, arg2, int_arg);

        } else if (strcmp(command, "create") == 0) {
            printf("Enter Filename: ");
            scanf("%s", arg1);
            printf("Enter Content: ");
            scanf(" %[^\n]", arg2);

            // Default safe permissions applied to all CLI creations
            // Owner R/W, Group Read, Others Blocked
            FilePermission dynamic_mask;
            dynamic_mask.owner.read = true;   dynamic_mask.owner.write = true;   dynamic_mask.owner.execute = false;
            dynamic_mask.group.read = true;   dynamic_mask.group.write = false;  dynamic_mask.group.execute = false;
            dynamic_mask.others.read = false; dynamic_mask.others.write = false; dynamic_mask.others.execute = false;

            file_creation(arg1, arg2, dynamic_mask);

        } else if (strcmp(command, "read") == 0) {
            printf("Enter Filename: ");
            scanf("%s", arg1);
            file_read(arg1);

        } else if (strcmp(command, "write") == 0) {
            printf("Enter Filename: ");
            scanf("%s", arg1);
            printf("Enter New Content (no spaces): ");
            scanf(" %[^\n]", arg2);
            file_write(arg1, arg2);

        } else if (strcmp(command, "delete") == 0) {
            printf("Enter Filename: ");
            scanf("%s", arg1);
            file_delete(arg1);

        } else if (strcmp(command, "encrypt") == 0) {
            printf("Enter Filename: ");
            scanf("%s", arg1);
            file_encrypt(arg1);

        } else if (strcmp(command, "dump") == 0) {

            if(current_uid!=0){
                printf("||SECURITY BLOCKED|| Root level priviliges! \n");
                audit_action(current_active_user(),"UNAUTHORIZED DUMP ATTEMPT", "SYSTEM DUMP");
            }else{
                raw_mem(); // Show raw memory to prove encryption worked
            }

        } else if (strcmp(command, "exit") == 0) {
            printf("Shutting down environemnt! \n");
            break;

        }else if (strcmp(command,"removeuser") == 0){
            printf("Enter Username to remove: ");
            scanf("%s", arg1);
            user_delete(arg1);
        }
         else {
            printf("Command not found! Try: login, logout, adduser, create, read, write, delete, encrypt, dump, exit\n\n");
        }
    }

    return 0;
}