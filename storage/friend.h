#include <stdio.h>
#define MAX_FRIEND 100

#ifndef FRIEND_H
#define FRIEND_H

typedef struct{
    int user_id;
    int friend_number;
    int friend_ids[MAX_FRIEND - 1];
}friend_t;

friend_t friends[MAX_FRIEND];
int friend_count = 0;

void load_friends_from_file(){
    // load from friend.txt into friends[MAX_FRIEND]
    FILE *file = fopen("./storage/friend.txt", "r+");
    if (!file) {
        perror("Error opening file friend");
        return;
    }

    friend_count = 0;
    while (fscanf(file, "%d %d", &friends[friend_count].user_id, &friends[friend_count].friend_number) != EOF) {
        for (int j = 0; j < friends[friend_count].friend_number; j++) {
            fscanf(file, "%d", &friends[friend_count].friend_ids[j]);
        }
        friend_count++;
    }
    fclose(file);
}

void save_friends_to_file() {
    // save friends[MAX_FRIEND] into friend.txt
    FILE *file = fopen("./storage/friend.txt", "w+");
    if (!file) {
        perror("Error opening file friend");
        return;
    }

    for (int i = 0; i < friend_count; i++) {
        fprintf(file, "%d %d ", friends[i].user_id, friends[i].friend_number);
        for (int j = 0; j < friends[i].friend_number; j++) {
            fprintf(file, "%d ", friends[i].friend_ids[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Hàm tìm một user dựa trên user_id
friend_t* find_user(int user_id) {
    for (int i = 0; i < friend_count; i++) {
        if (friends[i].user_id == user_id) {
            return &friends[i];
        }
    }
    return NULL;
}

// Hàm thêm bạn
int add_friend(int user_a_id, int user_b_id) {
    friend_t *user_a = find_user(user_a_id);
    friend_t *user_b = find_user(user_b_id);

    // Nếu user_a chưa tồn tại, thêm vào danh sách
    if (!user_a) {
        friends[friend_count].user_id = user_a_id;
        friends[friend_count].friend_number = 0;
        user_a = &friends[friend_count++];
    }

    // Nếu user_b chưa tồn tại, thêm vào danh sách
    if (!user_b) {
        friends[friend_count].user_id = user_b_id;
        friends[friend_count].friend_number = 0;
        user_b = &friends[friend_count++];
    }

    // Kiểm tra và thêm user_b vào danh sách bạn bè của user_a
    for (int i = 0; i < user_a->friend_number; i++) {
        if (user_a->friend_ids[i] == user_b_id) {
            return 1; // Đã là bạn, không thêm nữa
        }
    }
    user_a->friend_ids[user_a->friend_number++] = user_b_id;

    // Kiểm tra và thêm user_a vào danh sách bạn bè của user_b
    for (int i = 0; i < user_b->friend_number; i++) {
        if (user_b->friend_ids[i] == user_a_id) {
            return 1; // Đã là bạn, không thêm nữa
        }
    }
    user_b->friend_ids[user_b->friend_number++] = user_a_id;

    // Lưu dữ liệu vào file
    save_friends_to_file();

    return 0;
}

int delete_friend(int user_a_id, int user_b_id) {
    friend_t *user_a = find_user(user_a_id);
    friend_t *user_b = find_user(user_b_id);

    if (!user_a || !user_b) {
        return 1; // One of the users does not exist
    }

    // Remove user_b_id from user_a's friend list
    int found_a = 0;
    for (int i = 0; i < user_a->friend_number; i++) {
        if (user_a->friend_ids[i] == user_b_id) {
            found_a = 1;
            for (int j = i; j < user_a->friend_number - 1; j++) {
                user_a->friend_ids[j] = user_a->friend_ids[j + 1];
            }
            user_a->friend_number--;
            break;
        }
    }

    // Remove user_a_id from user_b's friend list
    int found_b = 0;
    for (int i = 0; i < user_b->friend_number; i++) {
        if (user_b->friend_ids[i] == user_a_id) {
            found_b = 1;
            for (int j = i; j < user_b->friend_number - 1; j++) {
                user_b->friend_ids[j] = user_b->friend_ids[j + 1];
            }
            user_b->friend_number--;
            break;
        }
    }

    // Check if both deletions were successful
    if (found_a && found_b) {
        save_friends_to_file();
        return 0; // Success
    }

    return 1; // Failed to delete friend relationship
}

#endif