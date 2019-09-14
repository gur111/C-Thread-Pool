//
// Created by gur on 9/14/19.
//

#include "client.h"

int main1() {/*
    int sock = 0, valread, length, i = 1, fd, action = 3;
    int down = 1, up = 2;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_RAW)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) < 1) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return -1;
    }
    printf("Connected\n");
    int filesToSend = argc - 2;
*/

    return 0;
}