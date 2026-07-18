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
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    printf("Socket successfully created inside OS kernel (FD number: %d) !\n\n",server_fd);

    /*
        Reusing ports and address configurations
        setsockopt() allowes the server to instantly attach itself with with port 4000 on failures/restarts.
    
        SO_REUSEADDR -> configures the network stack to reuse last port address immediately
    */

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt config failed!!");
        close(server_fd); 
        exit(EXIT_FAILURE);
    }

    // Populating configuration variales for the interent socket address
    address.sin_family = AF_INET; // IPV4 Protocols as setup earlier
    address.sin_addr.s_addr = INADDR_ANY; // communicates with kernel to allow incoming packets
    address.sin_port = htons (PORT);

    // bind() connects the socket ID with physical port local network configuration (4000)
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Port binding failed!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Successfully bound to physical port: %d\n\n", PORT);

    /*
    Entering passive listen mode:
    listen() switches socket to a passive incoming state(listener).
    */

    if (listen(server_fd, 3) < 0) {
        perror("Listen queue configuration failed!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Awaiting incoming connections (Listening state)....\n\n");

    /*
    Accepting incoming client sync requests
    accept() is woken up the instant a client attempts connection-> extracts the client information
    and spawns a unique file descriptor (client_socket) for it.
     */

    client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (client_socket < 0) {
        perror("Client connection failed!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Remote client connected accepted! (Client FD: %d)\n\n", client_socket);

    /*
        Extracting data sent over the channel
        read() pulls incoming raw bytes directly out of client socket's kernel receiving buffer.
    */
    int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("Couldn't read bytes from socket stream!");
    } else {
        buffer[bytes_read] = '\0'; // Null terminator prevents overflow bugss
        printf("Client communicates: \"%s\"\n\n", buffer);
    }

    /*
        Resource Management: Closing down operational channels to avoid resource leakage
    */

    printf("Freeing network channels! \n\n");
    close(client_socket);
    close(server_fd);

    printf("Server successfully shut down! \n\n");

    return 0;
}