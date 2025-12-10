#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h" // Assumes you have a header file for your list

#define PORT 8080
#define ACK "ACK"

int main(int argc, char const* argv[]) { 
    int n, val, idx;
    int servSockD = socket(AF_INET, SOCK_STREAM, 0); 
  
    char buf[1024];
    char sbuf[1024];
    char* token;

    struct sockaddr_in servAddr; 
    list_t *mylist;

    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)); 
    listen(servSockD, 1); 
  
    printf("Server listening on port %d...\n", PORT);

    int clientSocket = accept(servSockD, NULL, NULL); 
    printf("Client connected!\n");

    mylist = list_alloc(); 

    while(1){
        n = recv(clientSocket, buf, sizeof(buf), 0);
        if (n <= 0) break; // Client closed connection
        
        buf[n] = '\0';
        token = strtok(buf, " ");

        // Reset response buffer
        memset(sbuf, '\0', sizeof(sbuf));

        if(token == NULL) {
            continue;
        }
        else if(strcmp(token,"exit") == 0){
             list_free(mylist);
             printf("Exiting server...\n");
             exit(0);
        }
        else if(strcmp(token,"print") == 0){
             char* listStr = listToString(mylist);
             snprintf(sbuf, sizeof(sbuf), "%s", listStr);
             free(listStr); // Assuming listToString allocates memory
        }
        else if(strcmp(token,"get_length") == 0){
             val = list_length(mylist);
             snprintf(sbuf, sizeof(sbuf), "Length = %d", val);
        }
        else if(strcmp(token,"add_front") == 0){
             token = strtok(NULL, " ");
             val = atoi(token);
             list_add_to_front(mylist, val);
             snprintf(sbuf, sizeof(sbuf), "%s %d", ACK, val);
        }
        else if(strcmp(token,"add_back") == 0){
             token = strtok(NULL, " ");
             val = atoi(token);
             list_add_to_back(mylist, val);
             snprintf(sbuf, sizeof(sbuf), "%s %d", ACK, val);
        }
        else if(strcmp(token,"add_position") == 0){
             token = strtok(NULL, " ");
             idx = atoi(token);
             token = strtok(NULL, " ");
             val = atoi(token);
             list_add_at_index(mylist, val, idx); // Check your list.c param order!
             snprintf(sbuf, sizeof(sbuf), "%s %d at %d", ACK, val, idx);
        }
        else if(strcmp(token,"remove_front") == 0){
             val = list_remove_from_front(mylist);
             snprintf(sbuf, sizeof(sbuf), "Removed %d", val);
        }
        else if(strcmp(token,"remove_back") == 0){
             val = list_remove_from_back(mylist);
             snprintf(sbuf, sizeof(sbuf), "Removed %d", val);
        }
        else if(strcmp(token,"remove_position") == 0){
             token = strtok(NULL, " ");
             idx = atoi(token);
             val = list_remove_at_index(mylist, idx);
             snprintf(sbuf, sizeof(sbuf), "Removed %d", val);
        }
        else if(strcmp(token,"get") == 0){
             token = strtok(NULL, " ");
             idx = atoi(token);
             val = list_get_elem_at(mylist, idx);
             snprintf(sbuf, sizeof(sbuf), "Value at %d = %d", idx, val);
        }
        else {
             snprintf(sbuf, sizeof(sbuf), "Unknown Command");
        }

        send(clientSocket, sbuf, sizeof(sbuf), 0);
        memset(buf, '\0', 1024);
    }
    return 0; 
}
