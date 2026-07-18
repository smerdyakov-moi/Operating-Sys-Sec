#include "globals.h"
#include "audit.h"

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