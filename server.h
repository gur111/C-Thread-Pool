#ifndef THREADPOOL_SERVER_H
#define THREADPOOL_SERVER_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>



#define NO_CHAR '-'
#define SLOTS 4

int answer;

char *check_answer(int guess);
void gen_rand_answer(void);
void *get_in_addr(struct sockaddr *sa);
void conn_handler(void *arg);


#endif //THREADPOOL_SERVER_H
