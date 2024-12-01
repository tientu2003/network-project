#include <stdio.h>
#include "account.h"
#include "message.h"
#include "room.h"
#include "friend.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


int main() {
// Test for account
//    account* accounts = load_account();
////
////
////    account acc1 = {++account_count, "user1", "password1", false};
////    account acc2 = {++account_count, "user2", "password2", true};
////
////    // Save accounts to the file
////    save_account(&acc1);
////    save_account(&acc2);
//
//    printf("Loaded %d accounts:\n", account_count);
//    for (int i = 0; i < account_count; i++) {
//        printf("ID: %d, Username: %s, Password: %s, Online: %d\n",
//               accounts[i].id, accounts[i].user_name, accounts[i].password, accounts[i].is_online);
//    }
//
//    char search_user[] = "user2";
//    int id = find_account(accounts,search_user);
//    if (id != -1) {
//        printf("Account found. ID: %d\n", id);
//    } else {
//        printf("Account not found for username: %s\n", search_user);
//    }
//
//    if( check_credentials(accounts,"user1", "password1") ){
//        printf("Correct Credentials\n");
//    } else{
//        printf("Wrong Credentials\n");
//    }
//
//    if( check_credentials(accounts,"user2", "password1") ){
//        printf("Correct Credentials\n");
//    } else{
//        printf("Wrong Credentials\n");
//
//    }
//
//    free(accounts);

// Test for message.h

//    message new_message = {1, time(0) , "Test new message here"};
//    message* current_messages = create_message(1, &new_message);
//
//
//    for(int i =0; i < message_count; i++){
//        printf("[Infor] Found One Message:\n");
//        printf("[Infor] %d\n",current_messages[i].sender_id);
//        printf("[Infor] %s",ctime(&current_messages[i].time));
//        printf("[Infor] %s\n",current_messages[i].content);
//    }
//    free(current_messages);

//    Test for room.h
//    load_rooms_from_file();
//
//    room new_room = {room_count, {101, 103}, 2};
//    create_room(&new_room);
//
//    room* room_data = find_all_member_in_room(1);
//    if (room_data) {
//        printf("Room ID: %d\nMembers: ", room_data->room_id);
//        for (int i = 0; i < room_data->member_count; i++) {
//            printf("%d ", room_data->member_ids[i]);
//        }
//        printf("\n");
//    }
//
//    room* room_data = find_all_room_by_user_id(103);
//
//    for(int i = 0; i < result_count; i++){
//        printf("[Infor] Room ID: %d\n[Infor] Members: ", room_data[i].room_id);
//        for (int j = 0; j < room_data[i].member_count; j++) {
//            printf("%d ", room_data[i].member_ids[j]);
//        }
//        printf("\n");
//    }

//  Test for friend.h
//    load_friends_from_file();
//
//    if(add_friend(1,2)){
//        printf("[Error] friend already added\n");
//    }else{
//        friend_t* friend_list_a = find_user(1);
//        printf("[Infor] User Id: %d\n", friend_list_a->user_id);
//        printf("[Infor] Friend List: ");
//        for(int i = 0; i < friend_list_a->friend_number; i++){
//            printf("%d ", friend_list_a->friend_ids[i]);
//        }
//        printf("\n");
//    }
//
//    add_friend(2,3);
//   friend_t* friend_list_b = find_user(2);
//    printf("[Infor] User Id: %d\n", friend_list_b->user_id);
//    printf("[Infor] Friend List: ");
//    for(int i = 0; i < friend_list_b->friend_number; i++){
//        printf("%d ", friend_list_b->friend_ids[i]);
//    }
//    printf("\n");
//    if(delete_friend(2,3)){
//        printf("[Error] Can not delete friendship\n");
//    }
//
//
//    friend_list_b = find_user(2);
//    printf("[Infor] User Id: %d\n", friend_list_b->user_id);
//    printf("[Infor] Friend List: ");
//    for(int i = 0; i < friend_list_b->friend_number; i++){
//        printf("%d ", friend_list_b->friend_ids[i]);
//    }
//    printf("\n");


    return 0;
}

