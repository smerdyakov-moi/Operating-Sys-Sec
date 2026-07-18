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
    printf("Client Tracking Socket successfully created. (Client FD: %d)\n\n", sock_fd);

    // Populating target operational settings variables
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    // inet_proton() converts string formats into raw binary bytes structures
    // because human readable string IP addresses can't be processed by network cards.

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Provided network destination address format is not working");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Successfull translation to binary structural notation. \n\n");

    /*
        connect() communicates with the local network card to execute a standard 3-way TCP handshake
        targeting config listed in serv_address strucutre.
    */

    printf("Knocking on port %d for initializing network  synchronization...\n\n",PORT);
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Network handshake connction failed!");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Communication pipeline successfully setup between server and client \n\n");

    printf("Transmitting raw  data bytes payload down the pipeline...\n\n");
    send(sock_fd, message, strlen(message), 0);
    printf("Mesasge succesful transmission \n\n");

    close(sock_fd);
    printf("Socket file descriptor cleaned succesfully! Session terminated \n\n");
    return 0;
}