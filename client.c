#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "msg.h"


int socket_create(const char *server_ip, int port) {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return EXIT_FAILURE;
    }
    return client_socket;
}

int login(int client_socket, char* username, char*password){
    msg_format msg,response;
    snprintf(msg.payload, MAX_PAYLOAD_SIZE, "%s %s",username,password);
    msg.header.type = AUTH_REQ;
    msg.header.code = 0;
    msg.header.length = strlen(msg.payload);
    msg.header.timestamp = (uint32_t)time(NULL);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        close(client_socket);
    }
    ssize_t received = recv(client_socket, &response, sizeof(response), 0);
    if(response.header.code==CODE_LOGIN_FAILED) {
        return 0;
    }else{
        int user_id=atoi(response.payload);
        return user_id;
    }
}

int register_account(int client_socket,char* username, char* password ){
    msg_format msg,response;
    snprintf(msg.payload, MAX_PAYLOAD_SIZE, "%s %s",username,password);
    msg.header.type = REGI_REQ; // AUTH_REQ
    msg.header.code = 0;
    msg.header.length = strlen(msg.payload);
    msg.header.timestamp = (uint32_t)time(NULL);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        close(client_socket);
    }
    ssize_t received = recv(client_socket, &response, sizeof(response), 0);

    if(response.header.code==CODE_REGISTRATION_FAILED) {
        return 0;
    }else {
        return 1;
    }
}

typedef struct online_list{
    int id;
    char name[30];
} onlineuserlist;

onlineuserlist online_user[100];

onlineuserlist* get_online_users(int client_socket, int* size) {

    msg_format msg, response;
    msg.header.type = USER_LIST_REQ;
    msg.header.code = 0;
    msg.header.timestamp = (uint32_t)time(NULL);

    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        exit(1);
    }

    int cnt = 0;
    while (1) {
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
            break;
        }
        if (response.header.length == 0) {
            break;
        } else {
            sscanf(response.payload, "%d %s", &online_user[cnt].id, online_user[cnt].name);
            cnt++;
        }
    }

    *size = cnt;
    return online_user;
}




