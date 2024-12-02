#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_ACCOUNT 100
#ifndef ACCOUNT_H
#define ACCOUNT_H


typedef struct {
    int id;
    char user_name[30];
    char password[60];
    bool is_online;
} account;

int account_count = 0;
account accounts[MAX_ACCOUNT];


int save_account(account* data){
    FILE* file = fopen("./storage/account.txt", "w"); // Open the file in append mode
    if (file == NULL) {
        printf("[Error] Unable to open file for saving.\n");
        return -1; // Error opening the file
    }

    // Write the account to the file
    for(int i=0;i<account_count;i++){
        fprintf(file, "%d %s %s\n", data[i].id, data[i].user_name, data[i].password);
    }
    fclose(file);
    return 0; // Success
}

int load_account(account* accounts){
    FILE* file = fopen("./storage/account.txt", "r"); // Open the file in read mode
    if (file == NULL) {
        printf("[Error] Unable to open file for loading.\n");
        account_count = 0;
        return -1; // Error opening the file
    }

    //account* accounts =  (account*) malloc(100 * sizeof(account)); // Allocate memory for up to 100 accounts
    if (accounts == NULL) {
        printf("[Error] Memory allocation failed.\n");
        fclose(file);
        account_count = 0;
        return -1;
    }

    int index = 0;
    while (fscanf(file, "%d %s %s",
                  &accounts[index].id,
                  accounts[index].user_name,
                  accounts[index].password) !=EOF){
        index++;
    }
    fclose(file);
    account_count = index; // Set the total number of loaded accounts
    //return accounts; // Return the array of accounts
    return 0;
}

int find_account(account* accounts, char* user_name) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].user_name, user_name) == 0) {
            return accounts[i].id; // Return the account ID if the username matches
        }
    }
    return -1; // Return -1 if the account is not found
}

// Check credentials for authentication
bool check_credentials(account* accounts, char* user_name, char* password){
    for(int i = 0; i < account_count; i++){
        if (strcmp(accounts[i].user_name, user_name) == 0){
            if(strcmp(accounts[i].password, password) == 0){
                return true;
            }else{
                return false;
            }
        }
    }
    return false; // NOT FIND VALID USER_NAME
}

#endif