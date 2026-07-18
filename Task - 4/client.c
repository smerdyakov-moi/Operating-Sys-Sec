#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // BYTE FORMATTING LIBBRARIES

#define PORT 4000

int main(){
    int sock_fd = 0; // File Descriptor tracking local client socket channel
    struct sockaddr_in serv_addr; 
    char *message = "Initial mock message";

    printf("Initializing Client Network.....\n\n");

    // Allocating client socket file descriptor interface

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket endpoint allocation failure");
        exit(EXIT_FAILURE);
    }
    printf("Client Tracking Socket successfully created. (Client FD: %d)\n", sock_fd);

    // Populating target operational settings variables
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    
}