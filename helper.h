#include "msg.h"
#include <stdio.h>
#include <unistd.h>
#ifndef HELPER_H
#define HELPER_H

int login(int client_socket){
    msg_format msg,response;
    char username[30],password[30];
    do{
        printf("Enter username:");
        scanf("%s",username);
        printf("Enter password:");
        scanf("%s",password);
        snprintf(msg.payload, MAX_PAYLOAD_SIZE, "%s %s",username,password);
        msg.header.type = AUTH_REQ;
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
void register_account(int client_socket){
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

#endif