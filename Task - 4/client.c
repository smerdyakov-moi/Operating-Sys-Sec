#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // BYTE FORMATTING LIBBRARIES

#define PORT 4000
#define BUFFER_SIZE 512 // Match the server buffer capacity for localized input pooling
#define XOR_KEY 0x5A    // Secret key for XOR encryption

// Custom protocol structural packet header used for framing data and validation checks
typedef struct {
    char command[8];       // Directives: "AUTH", "MSG", or "EXIT"
    int payload_length;    // Data validation parameter to prevent buffer overflows
    char session_token[32];// Cryptographic tracking identifier to isolate sessions
} PacketHeader;

// Function to encrypt or decrypt data using XOR
void custom_crypt(char *data, int length) {
    for (int i = 0; i < length; i++) {
        data[i] ^= XOR_KEY;
    }
}

int main(){
    int sock_fd = 0; // File Descriptor tracking local client socket channel
    struct sockaddr_in serv_addr; 
    char message[BUFFER_SIZE]; // Interactive allocation array replacing the mock string pointer
    char server_reply[BUFFER_SIZE]; // Array to hold the server's acknowledgment message

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
        perror("Network handshake connection failed!");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Communication pipeline successfully setup between server and client \n\n");

    // Ask user for the secure password
    printf("Enter secure jail access password: ");
    scanf(" %[^\n]", message);

    // Build the authentication header packet
    PacketHeader auth_header;
    strcpy(auth_header.command, "AUTH");
    auth_header.payload_length = strlen(message);
    strcpy(auth_header.session_token, "INIT_TOKEN");

    // Encrypt the password before sending
    custom_crypt(message, auth_header.payload_length);

    // Send the authentication packet with crash protection flags
    if (send(sock_fd, &auth_header, sizeof(PacketHeader), MSG_NOSIGNAL) < 0 || 
        send(sock_fd, message, auth_header.payload_length, MSG_NOSIGNAL) < 0) {
        printf("[NETWORK ERROR] Failed to connect to security endpoint!\n\n");
        close(sock_fd);
        return 0;
    }

    // Read server response for authentication status
    int auth_bytes = read(sock_fd, server_reply, BUFFER_SIZE - 1);
    if (auth_bytes <= 0) {
        printf("[NETWORK ERROR] Server broke connection during authentication phase.\n\n");
        close(sock_fd);
        return 0;
    }
    server_reply[auth_bytes] = '\0';

    // Verify if password was accepted
    if (strcmp(server_reply, "AUTH_SUCCESS") != 0) {
        printf("[SECURITY EXCLUSION] Access Denied! Incorrect passphrase string.\n\n");
        close(sock_fd);
        return 0;
    }
    printf("[SECURITY ACCESS] Authentication verified! Entering dynamic communication state.\n\n");

    // Persistent Loop
    while (true) {
        printf("Enter message to transmit (or type 'exit'): ");
        scanf(" %[^\n]", message); // Format constraint allows strings containing whitespace layout patterns

        PacketHeader header;
        strcpy(header.session_token, "MOCK_KEY");

        // Evaluating if user initialized a clean terminal connection teardown
        if (strcmp(message, "exit") == 0) {
            strcpy(header.command, "EXIT");
            header.payload_length = 0;
            
            // Transmitting using MSG_NOSIGNAL to prevent client from crashing if server drops during exit request
            send(sock_fd, &header, sizeof(PacketHeader), MSG_NOSIGNAL);
            break;
        }

        // Configure standard dynamic message transport protocol variables
        strcpy(header.command, "MSG");
        header.payload_length = strlen(message);

        // Encrypt the message body before sending it over the network
        custom_crypt(message, header.payload_length);

        printf("Transmitting raw  data bytes payload down the pipeline...\n\n");
        
        // Transmitting sequential structural protocol parts over the socket channel
        if (send(sock_fd, &header, sizeof(PacketHeader), MSG_NOSIGNAL) < 0 || 
            send(sock_fd, message, header.payload_length, MSG_NOSIGNAL) < 0) {
            
            printf("\nServer pipeline severed. Shutting down!\n\n");
            break; // Breaks the loop cleanly so it runs your final close() routine
        }
        
        printf("Mesasge succesful transmission \n\n");

        // Reading the confirmation message back from the server
        int reply_bytes = read(sock_fd, server_reply, BUFFER_SIZE - 1);
        if (reply_bytes <= 0) {
            perror("Connection to server acknowledgement failed!");
            break;
        }
        server_reply[reply_bytes] = '\0'; // Null terminator prevents overflow bugss
        printf("Server replies: \"%s\"\n\n", server_reply);
    }

    close(sock_fd);
    printf("Socket file descriptor cleaned succesfully! Session terminated \n\n");
    return 0;
}