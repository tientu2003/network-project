#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MEMBER 100
#define MAX_ROOMS 1000

#ifndef ROOM_H
#define ROOM_H

typedef struct {
    int room_id;
    int member_ids[MAX_MEMBER];
    int member_count;
} room;

room rooms[MAX_ROOMS];
int room_count = 0;
room result[MAX_ROOMS];
int result_count = 0;

// Hàm ghi toàn bộ danh sách rooms vào file
void save_rooms_to_file() {
    FILE *file = fopen("room.txt", "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < room_count; i++) {
        fprintf(file, "%d %d ", rooms[i].room_id, rooms[i].member_count);
        for (int j = 0; j < rooms[i].member_count; j++) {
            fprintf(file, "%d ", rooms[i].member_ids[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Hàm đọc danh sách rooms từ file
void load_rooms_from_file() {
    FILE *file = fopen("room.txt", "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    room_count = 0;
    while (fscanf(file, "%d %d", &rooms[room_count].room_id, &rooms[room_count].member_count) != EOF) {
        for (int j = 0; j < rooms[room_count].member_count; j++) {
            fscanf(file, "%d", &rooms[room_count].member_ids[j]);
        }
        room_count++;
    }
    fclose(file);
}

// Hàm tìm tất cả member trong một room
room* find_all_member_in_room(int room_id) {
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_id == room_id) {
            return &rooms[i];
        }
    }
    return NULL; // Không tìm thấy
}

// Hàm tìm tất cả rooms mà user_id tham gia
room* find_all_room_by_user_id(int user_id) {
    result_count = 0;
    for (int i = 0; i < room_count; i++) {
        for (int j = 0; j < rooms[i].member_count; j++) {
            if (rooms[i].member_ids[j] == user_id) {
                result[result_count++] = rooms[i];
                break;
            }
        }
    }

    return result_count > 0 ? result : NULL;
}

// Hàm tạo room
int create_room(room* data) {
    if (room_count >= MAX_ROOMS) {
        return -1; // Không thể tạo thêm room
    }
    rooms[room_count++] = *data;
    save_rooms_to_file();
    return 0;
}

// Hàm thêm member vào room
int add_member_into_room(int room_id, int user_id) {
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_id == room_id) {
            if (rooms[i].member_count >= MAX_MEMBER) {
                return -1; // Room đầy
            }
            rooms[i].member_ids[rooms[i].member_count++] = user_id;
            save_rooms_to_file();
            return 0;
        }
    }
    return -1; // Room không tồn tại
}

#endif

