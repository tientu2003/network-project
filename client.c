#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "msg.h"
void login(int client_socket){
    msg_format msg,response;
    char username[30],password[30];
    do{
        printf("Enter username:");
        scanf("%s",username);
        printf("Enter password:");
        scanf("%s",password);
        snprintf(msg.payload, MAX_PAYLOAD_SIZE, "%s %s",username,password);
        msg.header.type = 1; // AUTH_REQ
        msg.header.code = 0;
        msg.header.length = strlen(msg.payload);
        msg.header.timestamp = (uint32_t)time(NULL);
        if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
            perror("Failed to send message");
            close(client_socket);
            exit(0);
        }
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.code==CODE_LOGIN_FAILED)printf("Wrong username or password.Please try again.\n");
    }while(response.header.code!=CODE_LOGIN_SUCCESS);
    printf("Login successful\n");
}
void registerAccount(int client_socket){
    msg_format msg,response;
    char username[30],password[30];
    do{
        printf("Enter username:");
        scanf("%s",username);
        printf("Enter password:");
        scanf("%s",password);
        snprintf(msg.payload, MAX_PAYLOAD_SIZE, "%s %s",username,password);
        msg.header.type = REGI_REQ; // AUTH_REQ
        msg.header.code = 0;
        msg.header.length = strlen(msg.payload);
        msg.header.timestamp = (uint32_t)time(NULL);
        if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
            perror("Failed to send message");
            close(client_socket);
            exit(0);
        }
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.code==CODE_REGISTRATION_FAILED)printf("Username has been used.Please try again.\n");
    }while(response.header.code!=CODE_REGISTRATION_SUCCESS);
    printf("Register successful\n");
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int client_socket;
    struct sockaddr_in server_addr;
    msg_format msg, response;
    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return EXIT_FAILURE;
    }
    int authReq;
    printf("Enter 0 for login or 1 for register:");
    scanf("%d",&authReq);
    if(authReq==0)login(client_socket);
    else registerAccount(client_socket);
    // Prepare message
    msg.header.type = 1; // AUTH_REQ
    msg.header.code = 0; // No error
    snprintf(msg.payload, MAX_PAYLOAD_SIZE, "Hello, server!");
    msg.header.length = strlen(msg.payload);
    msg.header.timestamp = (uint32_t)time(NULL);

    // Send message to server
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        close(client_socket);
        return EXIT_FAILURE;
    }

    // Receive response from server
    ssize_t received = recv(client_socket, &response, sizeof(response), 0);
    if (received <= 0) {
        perror("Failed to receive response");
    } else {
        printf("Server Response:\n");
        printf("Type: %d\n", response.header.type);
        printf("Code: %d\n", response.header.code);
        printf("Length: %d\n", response.header.length);
        printf("Timestamp: %s\n", ctime(&response.header.timestamp));
        printf("Payload: %s\n", response.payload);
    }
    close(client_socket);
    return 0;
}
