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
    int user_id=atoi(response.payload);
    printf("Login successful\n");
    return user_id;
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
void get_online_users(int client_socket){
    msg_format msg,response;
    msg.header.type=USER_LIST_REQ;
    msg.header.code=0;
    msg.header.timestamp = (uint32_t)time(NULL);
    if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
        perror("Failed to send message");
        close(client_socket);
        exit(0);
    }
    char online_user_username[1000][30];
    int cnt=0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.length==0)break;
        else{
            strcpy(online_user_username[cnt],response.payload);
            cnt++;
        }
    }
    printf("\n%d online user(s)\n",cnt);
    for(int i=0;i<cnt;i++){
        printf("%s\n",online_user_username[i]);
    }
    printf("\n");

}

void get_room_list(int client_socket,int user_id){
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
    char room_list[1000][256];
    int cnt=0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.length==0)break;
        else{
            strcpy(room_list[cnt],response.payload);
            cnt++;
        }
    }
    printf("\n%d room(s)\n",cnt);
    printf("RoomID\n");
    for(int i=0;i<cnt;i++){
        printf("%s\n",room_list[i]);
    }
    printf("\n");
    
}

int get_room_message(int client_socket, int room_id){
    int room_type;
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
    char messages[500][1000];
    int cnt=0;
    while(1){
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
        if(response.header.length==1){
            room_type=atoi(response.payload);
            break;
        }
        else{
            strcpy(messages[cnt],response.payload);
            cnt++;
        }
    }
    printf("Sender | Content\n");
    for(int i=0;i<cnt;i++){
        printf("%s",messages[i]);
    }
    return room_type;
}
void enter_chat_room(int client_socket,int user_id){
    printf("Enter room ID:");
    int room_id;
    scanf("%d",&room_id);
    int room_type=get_room_message(client_socket,user_id);
    msg_format msg,response;
    char message[256];
    
    while(1){
        printf("Enter chat([quit] to quit):");
        scanf("%s",message);
        if(strcmp(message,"[quit]")==0)break;
        else{
            sprintf(msg.payload,"%d %d %s",room_id,user_id,message);
        }
        if(room_type==0)msg.header.type=PRIVATE_MSG;
        else msg.header.type=GROUP_MSG;
        msg.header.code=0;
        msg.header.timestamp = (uint32_t)time(NULL);
        msg.header.length=strlen(msg.payload);
        if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
            perror("Failed to send message");
            close(client_socket);
            exit(0);
        }
        get_room_message(client_socket,room_id);
    }
    
}
void logout(int client_socket,int user_id){
    msg_format msg,response;
    msg.header.type=LOGOUT_REQ;
    msg.header.code=0;
    msg.header.timestamp = (uint32_t)time(NULL);
    sprintf(msg.payload,"%d",user_id);
    msg.header.length=strlen(msg.payload);
    do{
        if (send(client_socket, &msg, sizeof(msg), 0) <= 0) {
            perror("Failed to send message");
            close(client_socket);
            exit(0);
        }
        ssize_t received = recv(client_socket, &response, sizeof(response), 0);
        if (received <= 0) {
            perror("Failed to receive response");
        }
    }while(response.header.code!=CODE_LOGOUT_SUCCESS);
    printf("--------------Logout--------------\n\n");
}

#endif