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

int logout(int client_socket, int user_id){
    msg_format msg,response;
    msg.header.type=LOGOUT_REQ;
    msg.header.code=0;
    msg.header.timestamp = (uint32_t)time(NULL);
    sprintf(msg.payload,"%d",user_id);
    msg.header.length=strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        close(client_socket);
        exit(0);
    }

    ssize_t received = recv(client_socket, &response, sizeof(response), 0);
    if (received <= 0) {
        perror("Failed to receive response");
    }

    if(response.header.code != CODE_LOGOUT_SUCCESS){
        return 0;
    }else{
        return 1;
    }
}


typedef struct room_list{
    int id;
    char name[MAX_PAYLOAD_SIZE];
} room_list;

room_list* get_room_list(int client_socket, int user_id, int* size){
    static room_list rooms[MAX_ROOMS];
    msg_format msg,response;
    msg.header.type=ROOM_LIST_REQ;
    msg.header.code=0;
    msg.header.timestamp = (uint32_t)time(NULL);
    sprintf(msg.payload,"%d",user_id);
    msg.header.length=strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        close(client_socket);
        exit(0);
    }
    int cnt=0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.length==0)break;
        else{

            char buffer[MAX_PAYLOAD_SIZE];
            strcpy(buffer, response.payload);
            char *token = strtok(buffer, " ");
            if (token != NULL) {
                rooms[cnt].id = atoi(token); // Convert the first token to integer
            }

            // Extract the rest of the payload as the room name
            token = strtok(NULL, ""); // Get the remaining part of the payload
            if (token != NULL) {
                strncpy(rooms[cnt].name, token, MAX_PAYLOAD_SIZE - 1);
            }

            size_t len = strlen(rooms[cnt].name);
            if (len > 0 && rooms[cnt].name[len - 1] == ' ') {
                rooms[cnt].name[len - 1] = '\0';
            }
            cnt++;
        }
    }

    *size = cnt;
    return rooms;
}

typedef struct mess{
    char sender[30];
    char content[1024];
}mess;

mess* get_history_messages(int client_socket, int room_id, int* size){
    static mess message_data[MAX_MESSAGE];
    msg_format msg,response;
    msg.header.type=MESSAGE_LIST_REQ;
    msg.header.code=0;
    msg.header.timestamp = (uint32_t)time(NULL);
    sprintf(msg.payload,"%d",room_id);
    msg.header.length=strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        close(client_socket);
        exit(0);
    }

    int count = 0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }else{
            if(response.header.length == 0) break;
            // Parsing payload into mess struct
            sscanf(response.payload, "%s | %[^\n]",
                   message_data[count].sender,
                   message_data[count].content);
            count++;
        }
    }
    *size = count;
    return message_data;
}