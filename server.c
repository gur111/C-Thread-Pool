// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "thpool.h"
#include "server.h"
#include "common.h"
#include "game_list.h"

int main(void) {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    Task task = {0};
    threadpool thpool = ThreadPoolInit(4);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port PORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port PORT
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        task.arg = malloc(sizeof(int *));
        *((int *) task.arg) = new_socket;
        task.f = &conn_handler;
        ThreadPoolInsertTask(thpool, &task);
    }

    return 0;
}

void conn_handler(void *arg) {
    int game_id, guess, chars_read;
    int connfd = *((int *) arg);
    GameNode *game_info;
    char *response;
    char buffer[GAME_ID_LEN + GUESS_LEN + 1] = {0};
    char game_id_raw[GAME_ID_LEN + 1] = {0}, guess_raw[GUESS_LEN + 1] = {0};

    chars_read = read(connfd, buffer, GAME_ID_LEN + GUESS_LEN);
    if (chars_read != (GAME_ID_LEN + GUESS_LEN)) {
        send(connfd, INVALID_GUESS_MSG, strlen(INVALID_GUESS_MSG), 0);
        printf("Client sent an invalid guess\n");
        return;
    }

    listGames();

    memcpy(game_id_raw, buffer, GAME_ID_LEN);
    memcpy(guess_raw, buffer + GAME_ID_LEN, GUESS_LEN);
    printf("Data recv: %s%s\n", game_id_raw, guess_raw);
    guess = atoi(guess_raw);
    game_id = atoi(game_id_raw);

    // Get game info, create new game if requested
    if (!strcmp(game_id_raw, NEW_GAME_CMD)) {
        if (game._head == NULL) {
            game_info = createGame(gen_rand_answer(), /* id: */ 1, guess);
        } else if (game._tail == NULL) {
            printf("GameList tail was NULL when head was not\n");
            send(connfd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
            return;
        } else {
            game_info = createGame(gen_rand_answer(), /* id: */ game._tail->game_id+1, guess);
        }
        sprintf(game_id_raw, "%0"STR(GAME_ID_LEN)"d", game_info->game_id);
    }else{
        game_info = getGame(game_id, guess);
    }

    if(game_info == NULL){
        perror("game_info is NULL");
        send(connfd, SERVER_ERROR, strlen(SERVER_ERROR), 0);
        return;
    }

    response = check_answer(game_info->answer, guess);
    // concat game_id right after the response string
    response = realloc(response, strlen(response)+GAME_ID_LEN+1);
    if (!response) {
        perror("Allocation");
        return;
    }
    response = strncat(response, game_id_raw, GAME_ID_LEN);

    // Check if game is over
    if((response[0] - '0') == GUESS_LEN) {
        deleteGame(game_info->game_id);
        response = realloc(response, GUESS_LEN + strlen(GAME_OVER_MSG) + 1);
        if (!response) {
            perror("Allocation");
            return;
        }
        strncat(response, GAME_OVER_MSG, strlen(GAME_OVER_MSG));
    }

    send(connfd, response, strlen(response), 0);

    sleep(4);
    printf("%d\n", game_info->answer);
    free(game_info);
    free(response);
    return;
}

void admin_handler(){
    // TODO: Implement Admin handler "branched" from the conn_handler
}

int gen_rand_answer(void) {
    srand(time(NULL)); // Initialization, should only be called once.
    // Gets 4 digits randomly

    return rand() % ((int) pow(10, SLOTS)); // %10,000 is the last 4 digits (0-9999)
}

//// get sockaddr, IPv4 or IPv6:
//void *get_in_addr(struct sockaddr *sa) {
//    if (sa->sa_family == AF_INET) {
//        return &(((struct sockaddr_in *) sa)->sin_addr);
//    }
//
//    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
//}

char *check_answer(int answer, int guess) {
    int i, k;
    char *local_answer = malloc(SLOTS + 1);
    char *local_guess = malloc(SLOTS + 1);
    char *response = malloc(GUESS_LEN + 1);

    int misplaced_counter = 0;
    int correct_counter = 0;

    // Convert integers to strings
    sprintf(local_answer, "%04d", answer);
    sprintf(local_guess, "%04d", guess);

    for (i = 0; i < SLOTS; i++) {
        if (local_answer[i] == local_guess[i]) {
            correct_counter++;
            local_guess[i] = local_answer[i] = NO_CHAR;
        }
    }

    for (i = 0; i < SLOTS; i++) {
        if(local_answer[i] == NO_CHAR)
            continue; // Skip NO_CHAR

        for (k = 0; k < SLOTS; k++) {
            if(local_guess[k] == NO_CHAR)
                continue; // Skip NO_CHAR

            if (local_answer[i] == local_guess[k]) {
                misplaced_counter++;
                local_guess[k] = NO_CHAR;
                break;
            }
        }
    }

    // Formats response
    sprintf(response, "%dA%dB", correct_counter, misplaced_counter);
    free(local_guess);
    free(local_answer);
    return response;
}