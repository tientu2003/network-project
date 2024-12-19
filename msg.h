#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include "storage/account.h"
#include "storage/room.h"
#include "storage/message.h"
#include "storage/notification.h"
#include "storage/friend.h"
#define MAX_PAYLOAD_SIZE 1024  // Define a maximum payload size

#ifndef MSG_H
#define MSG_H

// Enumeration for message types
typedef enum {
    AUTH_REQ = 0x01,         // Client-server authentication request
    AUTH_RES = 0x02,         // Client-server authentication response
    USER_LIST_REQ = 0x03,    // Request for online users
    USER_LIST_RES = 0x04,    // Response with online users
    PRIVATE_MSG = 0x05,      // Direct message between users
    NOTIFICATION_MSG = 0x06, // Notification message
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
    sscanf(msg.payload,"%s %s", username,password);

    if(check_credentials(accounts,username,password) == 1){
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
        friend_t newFriend;
        newFriend.user_id = account_count;
        newFriend.friend_number = 0;
        friends[friend_count++] = newFriend;
        save_friends_to_file();
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
    room room_list[MAX_ROOMS];
    msg_format response;
    memset(&response, 0, sizeof(response));

    find_all_room_by_user_id(room_list,user_id);
    for(int i=0;i<MAX_ROOMS;i++){
        if(room_list[i].member_count==0)break;
        response.header.code=CODE_NO_ERROR;
        response.header.type=ROOM_LIST_RES;
        char room_id[10];
        sprintf(response.payload,"%d",room_list[i].room_id);
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
    response.header.length=0;
    response.header.timestamp=(uint32_t)time(NULL);
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}

int server_send_private_message(int client_socket,msg_format msg){
    int room_id, userId;
    char content[1024];
    sscanf(msg.payload,"%d %d %[^\n]",&userId,&room_id,content);
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
    accounts[user_id].is_online=-1;
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}

int handle_friend_req(int client_socket, msg_format msg){

    int action;
    int sender;
    int receiver;
    int confirm;
    sscanf(msg.payload,"%d %d %d %d",&action,&sender,&receiver, &confirm);
    printf("Received: %d %d %d %d\n",action, sender,receiver,confirm);
    if(action == 0){ //get friend list
        msg_format response;
        response.header.type = FRIEND_RES;
        response.header.code = CODE_NO_ERROR;
        friend_t* temp = find_user(sender);
        for(int i = 0; i< temp->friend_number;i++){
            sprintf(response.payload,"%d %s", temp->friend_ids[i] ,accounts[temp->friend_ids[i]].user_name);
            response.header.length = strlen(response.payload);
            if (send(client_socket, &response, sizeof(response), 0) <= 0) {
                perror("[Error] Failed to send response");
            }
        }
        sprintf(response.payload,"");
        response.header.length = 0;
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }
    }else if(action == 1){ // add friend
        create_new_notification(receiver,0,sender, -1);
    }else if(action == 2){ // remove friend
        delete_friend(sender, receiver);
    }else if(action == 3){ // confirm friend request
        if(confirm == 1){
            add_friend(sender,receiver);
            create_room(sender,receiver, 0);
            delete_notification(receiver, 0, sender, -1);
        }else{
            delete_notification(receiver, 0, sender, -1);
        }
    }
    return 1;
}


int server_get_notifications(int client_socket, msg_format msg){
    search_all_notification_by_user_id(atoi(msg.payload));
    msg_format response;
    response.header.type = NOTIFICATION_MSG;
    response.header.code = CODE_NO_ERROR;
    for(int i = 0; i< result_count;i++){
        sprintf(response.payload,"%d %d %d %s", result[i].sender_id, result[i].type, result[i].target, accounts[result[i].sender_id].user_name);
        response.header.length = strlen(response.payload);
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }
    }

    sprintf(response.payload,"");
    response.header.length = 0;
    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }
    return 1;
}

int handle_group_req(int client_socket, msg_format msg){
    room data[MAX_ROOMS];
    memset(data, 0, sizeof(data));
    int action;
    int sender;
    int receiver;
    int target;
    int confirm;
    sscanf(msg.payload,"%d %d %d %d %d", &action, &sender,&receiver, &target,  &confirm);
    msg_format response;
    response.header.type = GROUP_RES;
    response.header.code = CODE_NO_ERROR;
    if(action == 0){
        // list all group of users
        find_all_group_by_user_id(data,sender);
        for(int i=0;i<MAX_ROOMS;i++){
            if(data[i].member_count==0)break;
            char room_id[10];
            sprintf(response.payload,"%d",data[i].room_id);
            for(int j=0;j<data[i].member_count;j++){
                int member_id=data[i].member_ids[j];
                strcat(response.payload," ");
                strcat(response.payload,accounts[member_id].user_name);
            }
            response.header.length=strlen(response.payload);
            response.header.timestamp=(uint32_t)time(NULL);
            if (send(client_socket, &response, sizeof(response), 0) <= 0) {
                perror("[Error] Failed to send response");
            }
        }
        response.header.length=0;
        response.header.timestamp=(uint32_t)time(NULL);
        if (send(client_socket, &response, sizeof(response), 0) <= 0) {
            perror("[Error] Failed to send response");
        }

    }else if(action == 1){
        // create new room_id is treated as group chat
        create_room(sender, 0, 1);
    }else if(action == 2){
        // invite one people into group chat
        create_new_notification(receiver, 1, sender , target);
    }else if(action == 3){
        // leave a room_id (group chat) of one user
        remove_one_member_by_room_id(target,sender);
    }else if(action == 4){
        // confirm group invitation
        if(confirm == 1){
            add_member_into_room(target,receiver);
            delete_notification(receiver, 1, sender, target);
        }else{
            delete_notification(receiver, 1, sender, target);
        }
    }

    return 1;
}

#endif