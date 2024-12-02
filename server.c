#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "msg.h"

// Thread argument structure
typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} ClientArgs;

// Handle client communication
void *handle_client(void *arg) {
    ClientArgs *client_args = (ClientArgs *)arg;
    int client_socket = client_args->client_socket;
    struct sockaddr_in client_addr = client_args->client_addr;
    char client_ip[INET_ADDRSTRLEN];
    msg_format msg;

    // Get client IP address
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("[Infor] Connected to client: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    // Receive message from client
    ssize_t received = recv(client_socket, &msg, sizeof(msg), 0);
    if (received <= 0) {
        perror("[Error] Failed to receive message");
        close(client_socket);
        free(client_args);
        return NULL;
    }

    // Display received message
    printf("[Infor] Received Message from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
    printf("[Infor] Type: %d, Code: %d, Length: %d, Timestamp: %s\n",
           msg.header.type, msg.header.code, msg.header.length, ctime(&msg.header.timestamp));
    printf("[Infor] Payload: %s\n", msg.payload);

    // Prepare and send response
    msg_format response;
    response.header.type = msg.header.type;
    response.header.code = 0; // No error
    response.header.length = snprintf(response.payload, MAX_PAYLOAD_SIZE, "Hello, client!");
    response.header.timestamp = (uint32_t)time(NULL);

    if (send(client_socket, &response, sizeof(response), 0) <= 0) {
        perror("[Error] Failed to send response");
    }

    printf("[Infor] Response sent to client %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    close(client_socket);
    free(client_args);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int port = atoi(argv[1]);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("[Error] Socket creation failed");
        return EXIT_FAILURE;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Error] Bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0) {
        perror("[Error] Listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    printf("[Infor] Server listening on port %d...\n", port);

    // Accept and handle multiple clients
    while (1) {
        addr_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("[Error] Client accept failed");
            continue;
        }

        // Create argument for thread
        ClientArgs *client_args = malloc(sizeof(ClientArgs));
        if (client_args == NULL) {
            perror("[Error] Failed to allocate memory for client arguments");
            close(client_socket);
            continue;
        }

        client_args->client_socket = client_socket;
        client_args->client_addr = client_addr;

        // Create a thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_args) != 0) {
            perror("[Error] Failed to create thread");
            close(client_socket);
            free(client_args);
            continue;
        }

        // Detach the thread to allow independent execution
        pthread_detach(thread_id);
    }

    close(server_socket);
    return 0;
}
