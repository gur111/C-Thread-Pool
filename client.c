#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include "client.h"
#include "common.h"
   
int main(void) {
    int i;

    char guess[GUESS_LEN+1] = {0},
         req[GAME_ID_LEN+GUESS_LEN+1] = {0},
         buffer[BUFFER_SIZE] = {0},
         *server_msg;

    strcpy(req, NEW_GAME_CMD);
    { // Get initial guess from user
        printf("Welcome to Bulls&Cows. Please enter your guess (must be " STR(GUESS_LEN) " digits): ");
        scanf("%"STR(GUESS_LEN)"s", guess);
        if (strlen(guess) != GUESS_LEN) {
            printf("Wrong guess length.\n");
            return -1;
        }

        // Validate guess characters
        for(i = 0; i<GUESS_LEN; i++){
            if(!isdigit(guess[i])){
                printf("Not a digit");
                return -1;
            }
        }
    } // End getting initial guess


    while(1){
        strncat(req, guess, GUESS_LEN);
        communicate_guess(req, buffer, BUFFER_SIZE);
        strncpy(req, buffer+CHECK_LEN, GAME_ID_LEN);
        req[GAME_ID_LEN] = '\0';
        server_msg = buffer+CHECK_LEN+GAME_ID_LEN;
        buffer[CHECK_LEN] = '\0';
        { // Print response to the user
            if (strlen(server_msg)) {
                if (strcmp(server_msg, GAME_OVER_MSG)) {
                    printf("%s%s\n", buffer, server_msg);
                    return 0;
                } else {
                    perror(server_msg);
                    return -1;
                }
            } else {
                // Everything is fine. No errors nor the game is won
                // Get guess
                printf("Response: %s\n", buffer);
            }
        }
        { // Get guess
            printf("Enter another guess: ");
            scanf("%"STR(GUESS_LEN)"s", guess);
            if (strlen(guess) != GUESS_LEN) {
                printf("Wrong guess length.\n");
                return -1;
            }

            // Validate guess characters
            for (i = 0; i < GUESS_LEN; i++) {
                if (!isdigit(guess[i])) {
                    perror("Not a digit");
                    return -1;
                }
            }
        } // End get guess
    }
}

void communicate_guess(char *req, char *result_dest, size_t size){
    int sock, valread;

    sock = get_conn();

    send(sock, req, GUESS_LEN+GAME_ID_LEN, 0);

    valread = read(sock, result_dest, size);

    if(valread < 0) {
        printf("Ohh no, I got an invalid response from the server. Server returned: %s", strerror(errno));
        exit(-1);
    }else if(valread < GAME_ID_LEN+CHECK_LEN){
        printf("Server returned too short an answer %s", result_dest);
        exit(-1);
    }
}

int get_conn(void){
    struct sockaddr_in serv_addr;
    int sock = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\n Socket creation error \n");
        exit(-1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        perror("\nInvalid address/ Address not supported \n");
        exit(-1);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(-1);
    }
    return sock;
}