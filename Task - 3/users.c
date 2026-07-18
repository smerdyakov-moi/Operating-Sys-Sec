#include "globals.h"
#include "users.h"
#include "audit.h"
#include "auth.h"

void user_create(char *username, char *password, int gid){ //gid = group id

    // Only roots are allowed to create users
    if (current_uid != 0) {
        printf("||SECURITY BLOCKED|| Access Denied: Only root can provision new accounts!\n\n");
        audit_action(current_active_user(), "UNAUTHORIZED USER ADD ATTEMPT", "CREATE USER");
        return;
    }

    // Buffer Overflow
    if (strlen(username) >= MAX_STR || strlen(password) >= MAX_STR) {
        printf("||BLOCKED|| Input length rejects buffer limits!\n\n");
        return;
    }

    // Maximum user limit reached
    if (totalUsers >= MAX_USERS) {
        printf("ERROR: System user capacity reached!\n\n");
        return;
    }

    // Preventing duplicate users
    for (int i = 0; i < totalUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("||BLOCKED|| Username '%s' already exists in the system database!\n\n", username);
            return;
        }
    }

    // Populating Users
    users[totalUsers].user_id = next_uid; // Monotonically increasing secure UIDs
    next_uid +=1;
    users[totalUsers].group_id = gid;    // Explicitly placed into target group matrix
    strcpy(users[totalUsers].username, username);
    strcpy(users[totalUsers].password, password);

    totalUsers += 1;
    printf("Account '%s' created successfully! \n",username);
    audit_action("root", "SUCCESS", "CREATE USER");
}

void user_delete(char *username) {
    if (current_uid != 0) {
        printf("||SECURITY BLOCKED|| Access Denied: Only root can remove accounts!\n\n");
        audit_action(current_active_user(), "UNAUTHORIZED_USER_REMOVAL_ATTEMPT", "DELETE_USER");
        return;
    }

    if (strcmp(username, "root") == 0) {
        printf("||BLOCKED|| Structural Lockout: The root superuser account cannot be deleted!\n\n");
        return;
    }

    for (int i = 0; i < totalUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            int target_uid = users[i].user_id; // Keep track of the target UID before erasing it

            // Removing User: Normal Left Shift
            for (int j = i; j < totalUsers - 1; j++) {
                users[j] = users[j + 1];
            }
            totalUsers--;

            // Deleting all the files owned by the owner for cleanup
            // Looping backwards to prevent index shifting bugs during left-shiftnig
            int purged_files_count = 0;
            for (int k = totalFiles - 1; k >= 0; k--) {
                if (files[k].owner_id == target_uid) {

                    // Shift subsequent virtual files to the left
                    for (int m = k; m < totalFiles - 1; m++) {
                        files[m] = files[m + 1];
                    }
                    totalFiles--;
                    purged_files_count++;
                }
            }

            printf("||SUCCESS|| Account '%s' permanently erased.\n", username);
            if (purged_files_count > 0) {
                printf("||DISK CLEANUP|| Purged %d orphaned files owned by '%s' from storage.\n\n", purged_files_count, username);
            } else {
                printf("\n");
            }

            audit_action("root", "SUCCESS", "DELETE USER");
            return;
        }
    }
    printf("ERROR: Target user '%s' not found.\n\n", username);
}
