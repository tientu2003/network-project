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
    int type; // 0 for
} room;

room rooms[MAX_ROOMS];
int room_count = 0;

// Hàm ghi toàn bộ danh sách rooms vào file
void save_rooms_to_file() {
    FILE *file = fopen("./storage/room.txt", "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < room_count; i++) {
        fprintf(file, "%d %d %d ", rooms[i].room_id, rooms[i].member_count, rooms[i].type);
        for (int j = 0; j < rooms[i].member_count; j++) {
            fprintf(file, "%d ", rooms[i].member_ids[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Hàm đọc danh sách rooms từ file
void load_rooms_from_file() {
    FILE *file = fopen("./storage/room.txt", "r");
    if (!file) {
        perror("Error opening file room");
        return;
    }

    room_count = 0;
    while (fscanf(file, "%d %d %d", &rooms[room_count].room_id, &rooms[room_count].member_count,&rooms[room_count].type) != EOF) {
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
void find_all_room_by_user_id(room* result,int user_id) {
    int result_count = 0;
    for (int i = 0; i < room_count; i++) {
        for (int j = 0; j < rooms[i].member_count; j++) {
            if (rooms[i].member_ids[j] == user_id) {
                result[result_count++] = rooms[i];
                break;
            }
        }
    }
}

void find_all_group_by_user_id(room* result,int user_id) {
    int result_count = 0;
    for (int i = 0; i < room_count; i++) {
        if(rooms[i].type == 0) continue;
        for (int j = 0; j < rooms[i].member_count; j++) {
            if (rooms[i].member_ids[j] == user_id) {
                result[result_count++] = rooms[i];
                break;
            }
        }
    }
}

// Hàm tạo room
int create_room(int user_a, int user_b, int input_type) {
    if (room_count >= MAX_ROOMS) {
        return -1; // Không thể tạo thêm room
    }
    room data;
    if(input_type == 0){
        data.room_id = room_count;
        data.member_count = 2;
        data.member_ids[0] = user_a;
        data.member_ids[1] = user_b;
        data.type = input_type;
    }else{
        data.room_id = room_count;
        data.member_count = 1;
        data.member_ids[0] = user_a;
        data.type = input_type;
    }
    rooms[room_count++] = data;
    save_rooms_to_file();
    return 0;
}

// Hàm xóa một thành viên khỏi room với room.type == 1
int remove_one_member_by_room_id(int room_id, int user_id) {
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_id == room_id && rooms[i].type == 1) {
            int found = 0;
            for (int j = 0; j < rooms[i].member_count; j++) {
                if (rooms[i].member_ids[j] == user_id) {
                    found = 1;
                }
                if (found && j < rooms[i].member_count - 1) {
                    // Dịch các phần tử còn lại sang trái
                    rooms[i].member_ids[j] = rooms[i].member_ids[j + 1];
                }
            }
            if (found) {
                rooms[i].member_count--; // Giảm số lượng thành viên
                save_rooms_to_file();
                return 0; // Thành công
            }
            return -1; // Không tìm thấy thành viên
        }
    }
    return -1; // Room không tồn tại hoặc không phải group
}

// Sửa đổi hàm add_member_into_room
int add_member_into_room(int room_id, int user_id) {
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_id == room_id && rooms[i].type == 1) { // Chỉ áp dụng với room.type == 1
            if (rooms[i].member_count >= MAX_MEMBER) {
                return -1; // Room đầy
            }

            // Kiểm tra nếu thành viên đã tồn tại trong room
            for (int j = 0; j < rooms[i].member_count; j++) {
                if (rooms[i].member_ids[j] == user_id) {
                    return -1; // Thành viên đã tồn tại
                }
            }

            // Thêm thành viên mới vào room
            rooms[i].member_ids[rooms[i].member_count++] = user_id;
            save_rooms_to_file();
            return 0; // Thành công
        }
    }
    return -1; // Room không tồn tại hoặc không phải group
}

#endif

