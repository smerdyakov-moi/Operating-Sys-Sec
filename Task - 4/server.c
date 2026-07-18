#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4000
#define BUFFER_SIZE 512 //setting the buffer size for incoming messages

int main(){
    
    struct sockaddr_in address; // Used sockaddr_in because it's specifically designed for handling IPV4
    // addresses and ports
    int server_fd; // File descriptor tracking server's LISTENING master socket
    int client_socket; // File descriptor tracking nearly spawned socket for a connected client
    int opt = 1;
    socklen_t addrlen = sizeof(address); 
    char buffer[BUFFER_SIZE] = {0}; // Array buffer to read incoming network information

    printf("1. Initializing Server Network....\n\n");

    /*
        socket() asks the kernel to build an 'endpoint' for network communication.
        AF_INET: IPV4 standard system 4-octet IP address
        SOCK_STREAM: Provides reliale sequenced **2-way** connection based byte streams

    */

    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0){
        perror("Socket creation failed! \n\n");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully created inside OS kernel (FD number: %d) !\n\n",server_fd);

    

}