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

int login(int client_socket, char* username, char*password , int message_socket){
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
        if(response.header.type != USER_LIST_RES) continue;
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
        if(response.header.type != ROOM_LIST_RES) continue;
        if(response.header.length==0)break;
        else{
            sscanf(response.payload,"%d %[^\n]",&rooms[cnt].id, rooms[cnt].name);
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
            if(response.header.type != MESSAGE_LIST_RES) continue;
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

int send_message(int client_socket, int sender, int room_id, char* content){
    msg_format msg;
    msg.header.type = PRIVATE_MSG;
    msg.header.timestamp = (uint32_t)time(NULL);
    sprintf(msg.payload,"%d %d %s", sender, room_id, content);
    msg.header.length = strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        return 1;
    }
    return 0;
}

typedef struct noti_data {
    int sender;
    int type;
    int target;
    char name[30];
}noti_data;

noti_data* get_notifications(int client_socket, int user_id, int* size){
    msg_format msg,response;
    static noti_data data[MAX_NOTIFICATION];
    msg.header.type = NOTIFICATION_MSG;
    sprintf(msg.payload,"%d", user_id);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
    }
    int count = 0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if(response.header.length == 0) break;
        if (received <= 0) {
            perror("Failed to receive response");
        }else{
            sscanf(response.payload,"%d %d %d %s", &data[count].sender, &data[count].type, &data[count].target, data[count].name);
            count++;
        }
    }
    *size = count;
    return data;
}

typedef struct friend {
    int id;
    char name[30];
}friend;


friend* get_friend_list(int client_socket, int sender, int* size) {
    msg_format msg, response;
    static friend data[100];
    msg.header.type = FRIEND_REQ;
    sprintf(msg.payload, "%d %d %d %d", 0, sender, 0, 0);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
    }
    int count = 0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if(response.header.length == 0) break;
        if (received <= 0) {
            perror("Failed to receive response");
        }else{
            sscanf(response.payload,"%d %s", &data[count].id, data[count].name);
            count++;
        }
    }
    *size = count;
    return data;
}



int send_friend_cmd(int client_socket,int action , int sender, int receiver, int confirm){
    msg_format msg;
    msg.header.type = FRIEND_REQ;
    msg.header.code = CODE_NO_ERROR;
    sprintf(msg.payload,"%d %d %d %d", action, sender, receiver , confirm);
    msg.header.length = strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
    }
    return 0;
}

typedef struct group{
    int room_id;
    char name[500];
}group;

group* get_group_list(int client_socket, int sender, int* size){
    msg_format msg, response;
    static group data[100];
    memset(data, 0, sizeof(data));

    msg.header.type = GROUP_REQ;
    sprintf(msg.payload, "%d %d %d %d %d", 0, sender, 0, 0, 0);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
    }
    int count = 0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if(response.header.length == 0) break;
        if (received <= 0) {
            perror("Failed to receive response");
        }else{
            sscanf(response.payload,"%d %[^\n]", &data[count].room_id, data[count].name);
            count++;
        }
    }
    *size = count;
    return data;
}


int send_group_cmd(int client_socket,int action , int sender, int room_id, int receiver, int confirm){
    msg_format msg;
    msg.header.type = GROUP_REQ;
    msg.header.code = CODE_NO_ERROR;
    sprintf(msg.payload,"%d %d %d %d %d", action, sender, room_id , receiver, confirm);
    msg.header.length = strlen(msg.payload);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
    }
    return 0;
}