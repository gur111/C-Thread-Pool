#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "thpool.h"
#include "server.h"
#include "common.h"
#include "game_list.h"

int main(void) {
    char input_cmd[CMD_MAX_LEN] = {0};
    threadpool thpool = ThreadPoolInit(4);
    pthread_t conn_listener_thread;

    { // Create thread to run the connection listener in
        pthread_create(&conn_listener_thread, NULL, (void * (*)(void *)) &conn_listener, thpool);
        pthread_detach(conn_listener_thread);
    }

    printf("Welcome admin. From here you can enter commands to the server\n"
           "The available commands are:\n"
           "list and quit.\n"
           "Please do NOT enter commands longer than that.\n");
    // Handle admin commands
    while (1) {
        printf("$ ");
        // %xs while x is a number means that we limit the input token length to at most x chars
        scanf("%"STR(CMD_MAX_LEN)"s", input_cmd);
        if (!strncmp(input_cmd, LIST, strlen(LIST))) {
            listGames();
        } else if (!strncmp(input_cmd, QUIT, strlen(QUIT))) {
            printf("Shutting down server...\n");
            ThreadPoolDestroy((threadpool) thpool);
            exit(0);
            return 0;
        }else{
            printf("%s: Command not found\n", input_cmd);
        }
    }
}

int conn_listener(void *pool) {
    Task task = {0};
    struct sockaddr_in address;
    int server_fd, new_socket, opt = 1, addrlen = sizeof(address);

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

    // Main listener loop
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        task.arg = malloc(sizeof(int *));
        if(!task.arg){
			perror("Allocation");
            exit(EXIT_FAILURE);
        }
        *((int *) task.arg) = new_socket;
        task.f = &conn_handler;
        ThreadPoolInsertTask(pool, &task);
    }
}

void conn_handler(void *arg) {
    int game_id, guess, chars_read, connfd = *((int *) arg);
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
            game_info = createGame(gen_rand_answer(), /* id: */ game._tail->game_id + 1, guess);
        }
        sprintf(game_id_raw, "%0"STR(GAME_ID_LEN)"d", game_info->game_id);
    } else {
        game_info = getGame(game_id, guess);
    }

    if (game_info == NULL) {
        send(connfd, OLD_GAME_ERROR, strlen(OLD_GAME_ERROR), 0);
        return;
    }

    response = check_answer(game_info->answer, guess);
    // concat game_id right after the response string
    response = realloc(response, strlen(response) + GAME_ID_LEN + 1);
    if (!response) {
        perror("Allocation");
        return;
    }
    response = strncat(response, game_id_raw, GAME_ID_LEN);

    // Check if game is over
    if ((response[0] - '0') == GUESS_LEN) {
        deleteGame(game_info->game_id);
        response = realloc(response, GUESS_LEN + strlen(GAME_OVER_MSG) + 1);
        if (!response) {
            perror("Allocation");
            return;
        }
        strncat(response, GAME_OVER_MSG, strlen(GAME_OVER_MSG));
    }

    send(connfd, response, strlen(response), 0);
    free(arg); // Since we got it using malloc in the main loop
    free(game_info);
    free(response);
    return;
}

int gen_rand_answer(void) {
    srand(time(NULL)); // Initialization, should only be called once.
    // Gets 4 digits randomly
    return rand() % ((int) pow(10, SLOTS)); // %10,000 is the last 4 digits (0-9999)
}

//checks if the answer the client gave is equal to the server data
char *check_answer(int answer, int guess) {
    int i, k, misplaced_counter = 0, correct_counter = 0;
    char *local_answer = malloc(SLOTS + 1);
    if(!local_answer){
		perror("Allocation");
        exit(EXIT_FAILURE);
    }
    char *local_guess = malloc(SLOTS + 1);
    if(!local_guess){
        free(local_answer);
		perror("Allocation");
        exit(EXIT_FAILURE);
    }
    char *response = malloc(GUESS_LEN + 1);
    if(!response){
        free(local_answer);
        free(local_guess);
		perror("Allocation");
        exit(EXIT_FAILURE);
    }

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
        if (local_answer[i] == NO_CHAR)
            continue; // Skip NO_CHAR

        for (k = 0; k < SLOTS; k++) {
            if (local_guess[k] == NO_CHAR)
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