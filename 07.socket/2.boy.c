/*************************************************************************
	> File Name: 2.boy.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 25 May 2021 08:39:38 PM CST
 ************************************************************************/

#include "head.h"

int sockfd;

// 信号的handler
void close_sock(int signum) {
    send(sockfd, "Bye", 3, 0);
    close(sockfd);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port!\n", argv[0]);
        exit(1);
    }
    // 调用接口：主动连接指定IP和端口
    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        exit(1);
    }
    signal(SIGINT, close_sock);     // 设置信号捕捉器【^c信号，代号2】
    // 进行数据传输
    while (1) {
        char buff[1024] = {0};
        // 输入数据
        scanf("%[^\n]s", buff);
        getchar();
        if (strlen(buff)) {
            // 发送数据
            send(sockfd, buff, strlen(buff), 0);
        }
    }
}
