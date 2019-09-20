#ifndef THREADPOOL_SERVER_H
#define THREADPOOL_SERVER_H


#define NO_CHAR '-'
#define SLOTS 4


char *check_answer(int answer, int guess);
int gen_rand_answer(void);
void *get_in_addr(struct sockaddr *sa);
void conn_handler(void *arg);


#endif //THREADPOOL_SERVER_H
