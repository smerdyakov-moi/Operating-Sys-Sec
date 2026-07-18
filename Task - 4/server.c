#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4000
#define BUFFER_SIZE 512

int main(){
    struct sockaddr_in address; // Used sockaddr_in because it's specifically designed for handling IPV4
    // addresses and ports
    int server_fd, client_socket;

}