#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "helper.h"

void function_menu(int client_socket, int user_id){
    while(1){
        int choice;
        printf("---------------Menu---------------\n");
        printf("1. List Online User\n");
        printf("2. Chat List\n");
        printf("3. Enter Chat\n");
        printf("4. Add Friend\n");
        printf("5. Group Management\n");
        printf("6. Logout\n");
        if(scanf("%d", &choice) != 1){
            return;
        }
        switch (choice) {
            case 1:
                // Logic for list online user
                break;
            case 2:
                // Logic for list chat room
                break;
            case 3:
                // Logic for enter chat room to chat
                break;
            case 4:

                break;
            case 5:

                break;
            case 6:

                break;
            default:
                close(client_socket);
                return;
                break;
        }
    }
}

void enter_room(int client_socket, int room_id, int user_id){

}

void enter_group_management(int client_socket, int user_id){

}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int client_socket;
    struct sockaddr_in server_addr;
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
    while(1){
        printf("--------------Welcome-------------\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("Select: ");

        if(scanf("%d",&authReq) != 1) break;
        if(authReq == 1){
            int user_id = login(client_socket);
            function_menu(client_socket , user_id);
        }else if (authReq == 2) {
            register_account(client_socket);
        }else{
            break;
        }
    }

    close(client_socket);
    return 0;
}
