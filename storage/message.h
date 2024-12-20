#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#define MAX_MESSAGE 500

#ifndef MESSAGE_H
#define MESSAGE_H


typedef struct {
    int sender_id;
    time_t time;
    char content[1024];
} message;

int message_count = 0;

// Function to get all messages by a given room ID
int load_message(message* messages,int room_id) {
    char filename[50];
    snprintf(filename, sizeof(filename), "./storage/message_data/message_%d.txt", room_id);

    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("[Error] opening file");
        return 0; // Return NULL if the file does not exist
    }

    // Count the number of messages in the file
    message_count = 0;
    char line[1024];
    int index =0;
    while (fgets(line, sizeof(line), file) != NULL) {
        index++;
    }
    message_count = index/3;
    rewind(file);

    if (message_count == 0) {
        fclose(file);
        return 0; // No messages in the file
    }

    // Allocate memory for all messages
    if (messages == NULL) {
        perror("[Error] allocating memory");
        fclose(file);
        return 0;
    }

    // Read messages from the file
    for (int i = 0; i < message_count; i++) {
        int sender_id;
        char time_str[100];
        char content[1024];

        if (fscanf(file, "%d\n", &sender_id) != 1) {
            break;
        }
        fgets(time_str, sizeof(time_str), file);
        fgets(content, sizeof(content), file);

        struct tm tm;
        strptime(time_str, "%a %b %d %H:%M:%S %Y", &tm);
        time_t msg_time = mktime(&tm);
        messages[i].sender_id = sender_id;
        messages[i].time = msg_time;
        strcpy(messages[i].content, content);
    }
    fclose(file);
    return message_count;
}

// Function to append a new message and return all messages
void create_message(int room_id, message* new_message) {
    char filename[50];
    snprintf(filename, sizeof(filename), "./storage/message_data/message_%d.txt", room_id);

    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("[Error] opening file");
        return ;
    }

    // Write the new message to the file
    fprintf(file, "%d\n", new_message->sender_id);
    fprintf(file, "%s", ctime(&new_message->time));
    fprintf(file, "%s\n", new_message->content);
    fclose(file);
}

#endif