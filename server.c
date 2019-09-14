
#include "server.h"
//#include "pthread.h"


int main() {

    gen_rand__answer();

    printf("Answer: %d\n", answer);

    printf(check_answer(1234));

    return 0;/*
    int server_fd, newfd, valread, action = 0, size = 0, fd, countFiles = 0, amount = 0, ibuf = 0;
    void *dst;
    int *fileFD = (int *) malloc(1);
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    struct sockaddr_in address;
    struct sockaddr_storage remoteaddr; // client address
    int opt = 1;
    int addrlen = sizeof(address);
    char *buf = 0;

    // char *hello = "Hello from server";
    char remoteIP[INET6_ADDRSTRLEN];

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
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // add the listener to the master set
    FD_SET(server_fd, &master);

    // keep track of the biggest file descriptor
    fdmax = server_fd; // so far, it's this one
*/
}


void gen_rand__answer(void) {
    srand(time(NULL));   // Initialization, should only be called once.
    // Gets 4 digits randomly
    answer = rand() % ((int) pow(10, SLOTS)); // %10,000 is the last 4 digits (0-9999)
}

//// get sockaddr, IPv4 or IPv6:
//void *get_in_addr(struct sockaddr *sa) {
//    if (sa->sa_family == AF_INET) {
//        return &(((struct sockaddr_in *) sa)->sin_addr);
//    }
//
//    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
//}


char *check_answer(int guess) {
    int i, k;
    char *local_answer = malloc(SLOTS+1);
    char *local_guess = malloc(SLOTS+1);
    char *response = malloc(5);

    int misplaced_counter = 0;
    int correct_counter = 0;

    // Convert integers to strings
    sprintf(local_answer, "%04d", answer);
    sprintf(local_guess, "%04d", guess);

    printf("Guess: %s Answer: %s\n", local_guess, local_answer);

    for (i = 0; i < SLOTS; i++) {
        printf("Cur G: %c, A: %c\n", local_guess[i], local_answer[i]);
        printf("%d\n", local_answer[i] == local_guess[i]);
        if (local_answer[i] == local_guess[i]) {
            correct_counter++;
            printf("Correct at %d\n", i);
            local_answer[i] = NO_CHAR;
        }
    }

    for (i = 0; i < SLOTS; i++) {
        for (k = 0; k < SLOTS; k++) {
            if (local_answer[i] == local_guess[k]) {
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