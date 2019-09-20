//
// Created by gur on 9/14/19.
//

#ifndef THREADPOOL_CLIENT_H
#define THREADPOOL_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <ctype.h>

#define IP_ADDR 0x7f000001
#define BUFFER_SIZE 1024

int get_conn(void);
void communicate_guess(char *req, char *result_dest, size_t size);



#endif //THREADPOOL_CLIENT_H
