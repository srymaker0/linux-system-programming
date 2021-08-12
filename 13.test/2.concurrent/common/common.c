/*************************************************************************
	> File Name: common.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 23 Mar 2021 09:04:38 PM CST
 ************************************************************************/
#include "head.h"
int make_nonblock(int fd) {
    int flag;
    if ((flag = fcntl(fd, F_GETFL)) < 0) {
        perror("fcntl");
        return -1;
    }
    flag |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag);
}   
int make_block(int fd) {
    int flag;
    if ((flag = fcntl(fd, F_GETFL)) <0) {
        perror("fcntl");
        return -1;
    }
    flag &= ~O_NONBLOCK; 
    return fcntl(fd, F_SETFL, flag);
}

int socket_create(int port) {
    int sockfd;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }
    if (listen(sockfd, 20) < 0) {
        return -1;
    }
    return sockfd;
}


