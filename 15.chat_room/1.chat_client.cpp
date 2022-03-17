/*************************************************************************
	> File Name: chat_client.cpp
	> Author: 
	> Mail: 
	> Created Time: Thu 17 Mar 2022 02:41:35 PM CST
 ************************************************************************/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <assert.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE 1
#define BUFFER_SIZE 64

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port\n", argv[0]);
        exit(1);
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert( sockfd >= 0 );

    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("connect failed\n");
        close(sockfd);
        exit(1);
    }
    pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;
    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert( ret != -1 );
    while (1) {
        ret = poll(fds, 2, -1);
        if (ret < 0) {
            printf("poll fail\n");
            break;
        }
        if (fds[1].revents & POLLRDHUP) {
            printf("server close the connection\n");
            break;
        } else if (fds[1].revents & POLLIN) {
            memset(read_buf, 0, BUFFER_SIZE);
            recv(fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
            printf("%s", read_buf);
        }
        
        if (fds[0].revents & POLLIN) {
            ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            ret = splice(pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
        
    }
    close (sockfd);
    return 0;
}
