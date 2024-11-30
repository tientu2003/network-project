#include <stdio.h>
#include "account.h"
#include "message.h"
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

    message new_message = {1, time(0) , "Test new message here"};
    message* current_messages = create_message(1, &new_message);


    for(int i =0; i < message_count; i++){
        printf("[Infor] Found One Message:\n");
        printf("[Infor] %d\n",current_messages[i].sender_id);
        printf("[Infor] %s",ctime(&current_messages[i].time));
        printf("[Infor] %s\n",current_messages[i].content);
    }

    free(current_messages);

    return 0;
}

