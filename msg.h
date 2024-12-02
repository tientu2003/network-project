#include <stdint.h>
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include "storage/account.h"

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
    LOGOUT_REQ = 0x0F        // LOGOUT_REQ
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


#endif