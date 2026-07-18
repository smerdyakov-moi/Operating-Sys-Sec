#include "globals.h"
#include "filesystem.h"
#include "auth.h"
#include "audit.h"

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

            // Decryption Process 
            if (files[i].isEncrypted) {
                char temporary_decode_buffer[MAX_CONTENT];
                for (int j = 0; j < files[i].size; j++) {
                    temporary_decode_buffer[j] = files[i].content[j] ^ CIPHER_KEY;
                }
                temporary_decode_buffer[files[i].size] = '\0'; // Terminating the string bounds for clear printing result
                printf("READ SUCCESS (AUTO-DECRYPTED VIA KERNEL): Content of '%s':\n  -->   %s\n", files[i].filename, temporary_decode_buffer);
            } else {
                printf("READ SUCCESS: Content of '%s':\n  -->   %s\n", files[i].filename, files[i].content);
            }

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

            files[i].isEncrypted = false;

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

void file_encrypt(char *filename){
    char active_user[MAX_STR];
    strcpy(active_user,current_active_user());

    for (int i = 0; i < totalFiles; i++) {
        if (strcmp(files[i].filename, filename) == 0) {

            // Only users holding valid write metrics can encrypt
            if (!eval_permissions(i, 'w')) {
                printf("||SECURITY BLOCKED|| Access Denied: Insufficient authorization to encrypt '%s'!\n\n", filename);
                audit_action(active_user, "ENCRYPT DENIED", "FILE ENCRYPT");
                return;
            }

            if (files[i].isEncrypted) {
                printf("Notice: File '%s' is already cryptographically secure.\n", filename);
                return;
            }

            // In-place Bitwise XOR execution processing track loop
            for (int j = 0; j < files[i].size; j++) {
                files[i].content[j] ^= CIPHER_KEY;
            }
            files[i].isEncrypted = true;

            printf("||ENCRYPT SUCCESS|| File '%s' is encrypted inside system array storage.\n", filename);
            audit_action(active_user, "SUCCESS ENCRYPTED", "FILE ENCRYPT");
            return;
        }
    }
    printf("ERROR: File '%s' not found.\n\n", filename);
}

void raw_mem(){
    printf("||DIAGNOSTIC DUMP|| \n");
    if (totalFiles == 0){
        printf("||EMPTY ARRAY||\n");
    }
    for (int i = 0 ; i <totalFiles; i ++){
        printf("Index [%d] || File: '%s' || Encrypt Flag: %s \n",
            i, files[i].filename, files[i].isEncrypted ? "TRUE" : "FALSE");

        printf("-> Raw Array Bytes Content: \"");

        // Looping byte-by-byte to show clean hexadecimal codes for unprintable data

        for(int k = 0 ; k < files[i].size ; k++){
            if (files[i].content[k] < 32 || files[i].content[k] > 126) {
                printf("\\x%02X", (unsigned char)files[i].content[k]);
            } else {
                printf("%c", files[i].content[k]);
            }
        }
        printf("\"\n");
    }
    printf("\n\n\n");
}