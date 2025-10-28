#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "list.h"  // include your linked list header

#define PORT 9001
#define ACK "ACK "

// Global pointer for graceful shutdown
list_t *mylist;

// Signal handler to gracefully terminate and free memory
void handle_sigint(int sig) {
    printf("\nServer shutting down gracefully...\n");
    if (mylist != NULL) {
        list_free(mylist);
    }
    exit(0);
}

int main(int argc, char const* argv[]) 
{ 
    int n, val, idx;
    int servSockD = socket(AF_INET, SOCK_STREAM, 0); 

    if (servSockD < 0) {
        perror("socket creation failed");
        exit(1);
    }

    // Buffers
    char buf[1024];
    char sbuf[1024];
    char *token;

    struct sockaddr_in servAddr; 

    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 

    // Bind socket to port
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // Listen for a client connection
    if (listen(servSockD, 1) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server started. Waiting for client connection on port %d...\n", PORT);

    // Accept connection
    int clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket < 0) {
        perror("accept failed");
        exit(1);
    }

    printf("Client connected.\n");

    // Create linked list
    mylist = list_alloc();

    // Register signal handler
    signal(SIGINT, handle_sigint);

    // Main loop
    while (1) {
        n = recv(clientSocket, buf, sizeof(buf), 0);
        if (n <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }

        buf[n] = '\0';
        token = strtok(buf, " ");

        if (token == NULL) continue;

        memset(sbuf, '\0', sizeof(sbuf));

        // COMMAND HANDLING
        if (strcmp(token, "exit") == 0) {
            sprintf(sbuf, "Goodbye. Server shutting down...");
            send(clientSocket, sbuf, sizeof(sbuf), 0);
            list_free(mylist);
            printf("Server exiting...\n");
            break;
        }
        else if (strcmp(token, "get_length") == 0) {
            val = list_length(mylist);
            sprintf(sbuf, "Length = %d", val);
        }
        else if (strcmp(token, "add_front") == 0) {
            token = strtok(NULL, " ");
            val = atoi(token);
            list_add_to_front(mylist, val);
            sprintf(sbuf, ACK "%d added to front", val);
        }
        else if (strcmp(token, "add_back") == 0) {
            token = strtok(NULL, " ");
            val = atoi(token);
            list_add_to_back(mylist, val);
            sprintf(sbuf, ACK "%d added to back", val);
        }
        else if (strcmp(token, "add_position") == 0) {
            token = strtok(NULL, " ");
            idx = atoi(token);
            token = strtok(NULL, " ");
            val = atoi(token);
            list_add_at_index(mylist, idx, val);
            sprintf(sbuf, ACK "%d added at position %d", val, idx);
        }
        else if (strcmp(token, "remove_front") == 0) {
            val = list_remove_from_front(mylist);
            sprintf(sbuf, "Removed from front: %d", val);
        }
        else if (strcmp(token, "remove_back") == 0) {
            val = list_remove_from_back(mylist);
            sprintf(sbuf, "Removed from back: %d", val);
        }
        else if (strcmp(token, "remove_position") == 0) {
            token = strtok(NULL, " ");
            idx = atoi(token);
            val = list_remove_at_index(mylist, idx);
            sprintf(sbuf, "Removed from position %d: %d", idx, val);
        }
        else if (strcmp(token, "get") == 0) {
            token = strtok(NULL, " ");
            idx = atoi(token);
            val = list_get_elem_at(mylist, idx);
            sprintf(sbuf, "Value at index %d: %d", idx, val);
        }
        else if (strcmp(token, "print") == 0) {
            sprintf(sbuf, "%s", listToString(mylist));
        }
        else {
            sprintf(sbuf, "Unknown command. Type 'menu' in client for options.");
        }

        // Send response to client
        send(clientSocket, sbuf, sizeof(sbuf), 0);

        // Clear buffers
        memset(buf, '\0', sizeof(buf));
    }

    // Cleanup
    close(clientSocket);
    close(servSockD);
    list_free(mylist);
    return 0;
}
