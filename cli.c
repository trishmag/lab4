#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_COMMAND_LINE_LEN 1024
#define PORT 8080

char* getCommandLine(char *command_line){
    do{ 
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(0);
        }
    } while(command_line[0] == 0x0A); // Ignore just ENTER
    command_line[strlen(command_line) - 1] = '\0';
    return command_line;
}

int main(int argc, char const* argv[]) { 
    int sockID = socket(AF_INET, SOCK_STREAM, 0); 
    char *token, *cp;
    char buf[MAX_COMMAND_LINE_LEN];
    char responeData[MAX_COMMAND_LINE_LEN];

    struct sockaddr_in servAddr; 
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    int connectStatus = connect(sockID, (struct sockaddr*)&servAddr, sizeof(servAddr)); 
  
    if (connectStatus == -1) { 
        printf("Error connecting to server...\n"); 
        return -1;
    } 
  
    while(1) {
        printf("Enter Command (or menu): ");
        getCommandLine(buf);

        // Send command to server
        send(sockID, buf, strlen(buf), 0); 

        cp = strdup(buf); // Copy buf to avoid modifying it for logic checks
        token = strtok(cp, " ");

        if(token && strcmp(token,"exit") == 0){
            free(cp);
            exit(1);
        }
        else if(token && strcmp(token,"menu") == 0){
            printf("COMMANDS:\n---------\n1. print\n2. get_length\n3. add_back <value>\n4. add_front <value>\n5. add_position <index> <value>\n6. remove_back\n7. remove_front\n8. remove_position <index>\n9. get <index>\n10. exit\n");
            free(cp);
            // Don't wait for server response on 'menu'
            continue; 
        }
        free(cp);
 
        // Receive response
        memset(responeData, 0, sizeof(responeData));
        recv(sockID, responeData, sizeof(responeData), 0); 
        printf("\nSERVER RESPONSE: %s\n", responeData); 
        memset(buf, '\0', MAX_COMMAND_LINE_LEN);
    }
    return 0; 
}
