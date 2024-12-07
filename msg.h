#include <stdint.h>
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include "storage/account.h"
#include "storage/room.h"
#include "storage/message.h"

#define MAX_PAYLOAD_SIZE 256  // Define a maximum payload size

#ifndef MSG_H
#define MSG_H

// Enumeration for message types
typedef enum {
    AUTH_REQ = 0x01,         // Client-server authentication request
    AUTH_RES = 0x02,         // Client-server authentication response
    USER_LIST_REQ = 0x03,    // Request for online users
    USER_LIST_RES = 0x04,    // Response with online users
    PRIVATE_MSG = 0x05,      // Direct message between users
    GROUP_MSG = 0x06,        // Group message
    MESSAGE_LIST_REQ = 0x07, // Request for message list
    MESSAGE_LIST_RES = 0x08, // Response with message list
    REGI_REQ = 0x09,         // Registration request
    REGI_RES = 0x0A,         // Registration response
    GROUP_REQ = 0x0B,        // Group action (create/join/leave)
    GROUP_RES = 0x0C,        // Group action response
    FRIEND_REQ = 0x0D,       // Add/remove friend request
    FRIEND_RES = 0x0E,       // Add/remove friend response
    LOGOUT_REQ = 0x0F,       // LOGOUT_REQ
    ROOM_LIST_REQ = 0x10,    //ROOM_LIST_REQ
    ROOM_LIST_RES = 0x11,    //ROOM_LIST_RES
    LOGOUT_RES=0x12          //LOG_OUT_RES 
} msg_type;

// Enumeration for codes
typedef enum {
    CODE_NO_ERROR = 0x00,           // No error
    CODE_LOGIN_SUCCESS = 0x01,      // Login success
    CODE_LOGOUT_SUCCESS = 0x03,     // Logout success
    CODE_REGISTRATION_SUCCESS = 0x02, // Registration success
    CODE_LOGIN_FAILED = 0x11,       // Login failed
    CODE_REGISTRATION_FAILED = 0x12, // Registration failed
    CODE_USER_NOT_FOUND = 0x13,     // User not found
    CODE_MESSAGE_FAILED = 0x14,     // Message failed
    CODE_CREATE_GROUP_FAILED = 0x15,// Failed to create group
    CODE_JOIN_GROUP_FAILED = 0x16   // Failed to join group
} ResponseCode;

typedef struct {
    msg_type type;      // Message type
    short code;         // Response Code (2 byptes)
    int length;         // Message Length (4 bytes)
    time_t timestamp;   // Timestamp (8 bytes)
} msg_header;

// Full message structure
typedef struct {
    msg_header header;
    char payload[MAX_PAYLOAD_SIZE];        // Pointer to dynamic payload
} msg_format;


int server_login(msg_format msg,int client_socket){
    char username[30],password[30];
    msg_format response;
    sscanf(msg.payload,"%s %s",username,password);
    if(check_credentials(accounts,username,password)){
        response.header.code=CODE_LOGIN_SUCCESS;
        int user_id=find_account(accounts,username);
        sprintf(response.payload,"%d",user_id);
        accounts[user_id].is_online=client_socket;
    }
    else{
        response.header.code=CODE_LOGIN_FAILED;
    }
    response.header.type=AUTH_RES;
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return (response.header.code==CODE_LOGIN_SUCCESS)?1:0;
}

int server_register_account(msg_format msg,int client_socket){
    char username[30],password[30];
    msg_format response;
    sscanf(msg.payload,"%s %s",username,password);
    int res;
    res=find_account(accounts,username);
    if(res>-1){
        response.header.code=CODE_REGISTRATION_FAILED;
    }
    else{
        response.header.code=CODE_REGISTRATION_SUCCESS;
        accounts[account_count].id=account_count;
        strcpy(accounts[account_count].user_name,username);
        strcpy(accounts[account_count].password,password);
        account_count++;
        save_account(accounts);
    }
    response.header.type=REGI_RES;
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return (response.header.code==CODE_REGISTRATION_SUCCESS)?1:0;
}

int server_get_online_user(int client_socket){
    int online_user_ids[100];
    list_user_online(online_user_ids);
    msg_format response;
    for(int i=0;i<100;i++){
        int id=online_user_ids[i];
        if(id<0)break;
        response.header.code=CODE_NO_ERROR;
        response.header.type=USER_LIST_RES;
        memset(response.payload, 0, sizeof(response.payload));
        sprintf(response.payload,"%d %s", accounts[id].id, accounts[id].user_name);
//        strcpy(response.payload,accounts[id].user_name);
        response.header.length=strlen(response.payload);
        response.header.timestamp=(uint32_t)time(NULL);
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }
    }
    response.header.code=CODE_NO_ERROR;
    response.header.type=USER_LIST_RES;
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}


int server_get_room_with_user(int client_socket,int user_id){
    room room_list[1000];
    msg_format response;
    find_all_room_by_user_id(room_list,user_id);
    for(int i=0;i<1000;i++){
        if(room_list[i].member_count==0)break;
        response.header.code=CODE_NO_ERROR;
        response.header.type=ROOM_LIST_RES;
        char room_id[10];
        sprintf(room_id,"%d",room_list[i].room_id);
        strcpy(response.payload,room_id);
        for(int j=0;j<room_list[i].member_count;j++){
            int member_id=room_list[i].member_ids[j];
            strcat(response.payload," ");
            strcat(response.payload,accounts[member_id].user_name);
        }
        response.header.length=strlen(response.payload);
        response.header.timestamp=(uint32_t)time(NULL);
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }
    }
    response.header.code=CODE_NO_ERROR;
    response.header.type=ROOM_LIST_RES;
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}

int server_get_messages(int client_socket,msg_format msg){
    int room_id = atoi(msg.payload);
    msg_format response;
    message messages[MAX_MESSAGE];
    int message_count=load_message(messages,room_id);
    
    for(int i=0;i<message_count;i++){
        response.header.type=MESSAGE_LIST_RES;
        response.header.code=0;
        int sender_id=messages[i].sender_id;
        sprintf(response.payload,"%s | %s",accounts[sender_id].user_name,messages[i].content);
        response.header.length=strlen(response.payload);
        response.header.timestamp=(uint32_t)time(NULL);
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }
    }
    response.header.type=MESSAGE_LIST_RES;
    response.header.code=0;
    if(rooms[room_id].member_count>2)strcpy(response.payload,"1");
    else strcpy(response.payload,"0");
    response.header.length=1;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}

int server_send_private_message(int client_socket,msg_format msg){
    int room_id, userId;
    char content[1000];
    sscanf(msg.payload,"%d %d %s",&room_id,&userId,content);
    message message;
    strcpy(message.content,content);
    message.time=msg.header.timestamp;
    message.sender_id=userId;
    create_message(room_id,&message);
    return 1;
}

int server_logout(int client_socket,msg_format msg){
    int user_id;
    msg_format response;
    sscanf(msg.payload,"%d",&user_id);
    response.header.type=LOGOUT_RES;
    response.header.code=CODE_LOGOUT_SUCCESS;
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    accounts[user_id].is_online=false;
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}
#endif