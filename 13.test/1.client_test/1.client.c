/*************************************************************************
	> File Name: 1.client.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Thu 12 Aug 2021 10:03:26 AM CST
 ************************************************************************/

#include "head.h"

struct UserInfo {
    char name[50];
    char phone[20];
};


void handle(int num, int sockfd) {
    struct UserInfo user;
    char buff_n[50] = "zhengyujie";
    char buff_p[20] = "15958107734";
    strcpy(user.name, buff_n);
    strcpy(user.phone, buff_p);
    switch(num) {
        case 1: {
            send(sockfd, "15958107734", sizeof("15958107734"), 0);
        } break;
        case 2: {
            send(sockfd, "zhengyujie", sizeof("zhengyujie"), 0);
        } break;
        case 3: {
            send(sockfd, (void *)&user, sizeof(user), 0);
        } break;
        case 4: {
            close(sockfd);
        } break;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ip port!\n", argv[0]);
    }
    int sockfd;
    struct sockaddr_in server;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(1);
    }
    while (1) {
        char buff[100] = {0};
        int num;
        int rsize = recv(sockfd, (int *)&num, sizeof(int), 0);
        if (rsize <= 0) {
            close(sockfd);
            break;
        }
        handle(num, sockfd);
    }
    return 0;
}

