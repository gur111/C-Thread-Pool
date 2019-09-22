#ifndef THREADPOOL_SERVER_H
#define THREADPOOL_SERVER_H


#define NO_CHAR '-'
#define SLOTS 4
#define CMD_MAX_LEN 20

// Available commands:
#define LIST "list"
#define QUIT "quit"


char *check_answer(int answer, int guess);
int gen_rand_answer(void);
void *get_in_addr(struct sockaddr *sa);
int conn_listener(void *pool);
void conn_handler(void *arg);


#endif //THREADPOOL_SERVER_H
