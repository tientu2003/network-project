#include <stdio.h>
#define MAX_NOTIFICATION 1000

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

typedef struct {
    int user_id;   // receiver
    int type;      // 0 for friend request, 1 for group invitation
    int sender_id; // sender request or invitation
} notification;

notification news[MAX_NOTIFICATION];
int notification_count = 0;

// Function to load notifications for a user from file
void load_notification_from_file() {
    FILE *file = fopen("./storage/notifications.txt", "r+");
    if (file == NULL) {
        printf("Error: Cannot open file notifications.dat.\n");
        return;
    }

    notification_count = 0; // Reset notification count for the user
    while (fscanf(file, "%d %d %d\n",
                  &news[notification_count].user_id,
                  &news[notification_count].type,
                  &news[notification_count].sender_id) !=EOF){
        notification_count++;
        if (notification_count >= MAX_NOTIFICATION) {
            printf("Warning: Reached maximum notification limit.\n");
            break;
        }
    }
    fclose(file);
}

notification result[MAX_NOTIFICATION];
int result_count = 0; // Track the number of found notifications

notification* search_all_notification_by_user_id(int user_id) {
    result_count = 0; // Reset result count

    // Find all notifications for the given user_id
    for (int i = 0; i < notification_count; i++) {
        if (news[i].user_id == user_id) {
            result[result_count++] = news[i]; // Add the found notification to result
            if (result_count >= MAX_NOTIFICATION) {
                printf("Warning: Reached maximum result limit.\n");
                break;
            }
        }
    }

    // Return the result array, containing all matching notifications
    return result;
}

// Function to save notifications to file
void save_notifications_to_file() {
    FILE *file = fopen("./storage/notifications.txt", "w+");
    if (file == NULL) {
        printf("Error: Cannot open file notifications.dat.\n");
        return;
    }

    for (int i = 0; i < notification_count; i++) {
        fprintf(file, "%d %d %d\n", news[i].user_id, news[i].type, news[i].sender_id);
    }

    fclose(file);
}

// Function to create a new notification and save to file
void create_new_notification(int user_id, int type, int sender_id) {

    // Append to the notification array
    if (notification_count < MAX_NOTIFICATION) {
        news[notification_count].user_id = user_id;
        news[notification_count].type = type;
        news[notification_count].sender_id = sender_id;
        notification_count++;
        save_notifications_to_file();
    } else {
        printf("Error: Maximum notification limit reached.\n");
    }
}

// Function to delete a notification and update the file
int delete_notification(int user_id, int type, int sender_id) {
    int deleted = 0;

    // Remove the notification from the array
    for (int i = 0; i < notification_count; i++) {
        if (news[i].user_id == user_id && news[i].type == type && news[i].sender_id == sender_id) {
            // Shift all following notifications
            for (int j = i; j < notification_count - 1; j++) {
                news[j] = news[j + 1];
            }
            notification_count--;
            deleted = 1;
            break;
        }
    }

    if (deleted) {
        // Save the updated notifications to the file
        save_notifications_to_file();
    } else {
        printf("Notification not found.\n");
    }

    return deleted;
}

#endif
