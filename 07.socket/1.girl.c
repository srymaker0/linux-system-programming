/*************************************************************************
	> File Name: 1.girl.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 25 May 2021 07:31:56 PM CST
 ************************************************************************/

#include "head.h"

void Usage(const char *arg) {
    fprintf(stderr, "Usage %s -p port\n", arg);
    return ;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }
    int opt, port, sockfd;
    while ((opt = getopt(argc, argv, "p:")) != - 1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                Usage(argv[0]);
                exit(1);
        }
    }
    // 调用接口：在指定端口创建一个监听态的快递员
    if ((sockfd = socket_create(port)) < 0) {
        perror("socket_creat");
        exit(1);
    }
    
    // 持续监听，接受多个连接
    while (1) {
        int new_fd, pid;                 // new_fd:用于接受accept返回的新fd pid：用于连接多个客户端
        struct sockaddr_in client;       // 客户端的地址【sockaddr_in类型】 
        socklen_t len = sizeof(client);  // 地址长度
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("<Accept> %s:%d!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        // 子进程用于传输数据
        if (pid == 0) {
            while (1) {
                char buff[1024] = {0};   // 存放接收数据
                // 接收
                size_t rsize = recv(new_fd, buff, sizeof(buff), 0); // sizeof：能收多少收多少
                printf("<Recv %ld> : %s\n", rsize, buff);
                // <0 连接出错；=0 连接关闭【客户端没有发送数据】
                if (rsize <= 0) {
                    close(sockfd);     // 关闭子进程中不需要的资源【监听态的socket】
                    break;
                }
            }
            // 感知到有客户端断开连接
            printf("<Bye> : A boy left!\n");
            exit(0);   // 正常退出当前进程
        }
    }
    return 0;
}
