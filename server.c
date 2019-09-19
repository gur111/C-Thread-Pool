// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "thpool.h"
#include "server.h"
#define PORT 8080
//#include "pthread.h"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: './server port'\n");
        fprintf(stderr, "Example: './server 8080'\n");
        exit(-1);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int *arg;
    
    char *hello = "Hello from server";
    threadpool thpool = ThreadPoolInit(4);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        arg = malloc(sizeof(int));
        *arg = new_socket;
        ThreadPoolInsertTask(thpool, (void *)conn_handler, (void *)arg);
    }
    return 0;

    gen_rand_answer();

    printf("Answer: %d\n", answer);

    printf("%s\n", check_answer(1234));

    return 0;
}

void conn_handler(void *arg)
{
    int game_id, guess, chars_read;
    int connfd = *((int *)arg);
    char buffer[8] = {0};
    char game_id_raw[5] = {0}, guess_raw[5] = {0};
    
    chars_read = read(connfd, buffer, 8);
    if(chars_read != 8){
        send(connfd, "Invalid guess", strlen("Invalid guess"), 0);
        printf("Client sent an invalid guess");
        return;
    }

    printf("Data recv: %s\n", buffer);
    memcpy(buffer, game_id_raw, 4);
    memcpy(buffer+4, guess_raw, 4);
    guess = atoi(guess_raw);
    game_id = atoi(game_id_raw);
    
    send(connfd, "Hello", strlen("Hello"), 0);
    sleep(5);
    printf("Hello message sent\n");
}

void gen_rand_answer(void)
{
    srand(time(NULL)); // Initialization, should only be called once.
    // Gets 4 digits randomly
    answer = rand() % ((int)pow(10, SLOTS)); // %10,000 is the last 4 digits (0-9999)
}

//// get sockaddr, IPv4 or IPv6:
//void *get_in_addr(struct sockaddr *sa) {
//    if (sa->sa_family == AF_INET) {
//        return &(((struct sockaddr_in *) sa)->sin_addr);
//    }
//
//    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
//}

char *check_answer(int guess)
{
    int i, k;
    char *local_answer = malloc(SLOTS + 1);
    char *local_guess = malloc(SLOTS + 1);
    char *response = malloc(5);

    int misplaced_counter = 0;
    int correct_counter = 0;

    // Convert integers to strings
    sprintf(local_answer, "%04d", answer);
    sprintf(local_guess, "%04d", guess);

    printf("Guess: %s Answer: %s\n", local_guess, local_answer);

    for (i = 0; i < SLOTS; i++)
    {
        printf("Cur G: %c, A: %c\n", local_guess[i], local_answer[i]);
        printf("%d\n", local_answer[i] == local_guess[i]);
        if (local_answer[i] == local_guess[i])
        {
            correct_counter++;
            printf("Correct at %d\n", i);
            local_answer[i] = NO_CHAR;
        }
    }

    for (i = 0; i < SLOTS; i++)
    {
        for (k = 0; k < SLOTS; k++)
        {
            if (local_answer[i] == local_guess[k])
            {
                misplaced_counter++;
                printf("Misplaced at %d\n", i);
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