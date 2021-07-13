/*************************************************************************
	> File Name: 2.boy.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 25 May 2021 08:39:38 PM CST
 ************************************************************************/

#include "head.h"

struct file_info {
    char name[50];
    ssize_t size;
};

int sockfd;

// 信号的handler
void close_sock(int signum) {
    send(sockfd, "Bye", 3, 0);
    close(sockfd);
    exit(0);
}

int main(int argc, char **argv) {
    struct file_info info;
    if (argc != 4) {
        fprintf(stderr, "Usage : %s ip port file!\n", argv[0]);
        exit(1);
    }
    // 调用接口：主动连接指定IP和端口
    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        exit(1);
    }
    FILE *fp = fopen(argv[3], "r");
    fseek(fp, 0, SEEK_END);
    info.size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    signal(SIGINT, close_sock);     // 设置信号捕捉器【^c信号，代号2】
    strcpy(info.name, argv[3]);
    send(sockfd, (void *)&info, sizeof(info), 0);
    // 进行数据传输
    while (1) {
        char buff[1024] = {0};
        ssize_t cnt = fread(buff, 1, 1024, fp);
        if (cnt == 0) break;
        send(sockfd, buff, strlen(buff), 0);
    }
    fclose(fp);
    close(sockfd);
    return 0;
}
