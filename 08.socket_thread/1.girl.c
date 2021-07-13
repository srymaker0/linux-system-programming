/*************************************************************************
	> File Name: 1.girl.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 25 May 2021 07:31:56 PM CST
 ************************************************************************/

#include "head.h"
#define MAXUSER 100

void Usage(const char *arg) {
    fprintf(stderr, "Usage %s -p port\n", arg);
    return ;
}

// 可优化 用线程池
/* echo（回声）服务器 客户端发给服务端的数据，会从服务端回显给客户端 */
void *worker(void *arg) {
    int sockfd;
    sockfd = *(int *)arg;
    while (1) {
            char buff[1024] = {0};   // 存放接收数据
            // 接收
            size_t rsize = recv(sockfd, buff, sizeof(buff), 0); // sizeof：能收多少收多少
            printf("<Recv %ld> : %s\n", rsize, buff);
            // <0 连接出错；=0 连接关闭【客户端没有发送数据】
            if (rsize <= 0) {
                close(sockfd);     // 关闭子进程中不需要的资源【监听态的socket】
                break;
            } else {
                send(sockfd, buff, strlen(buff), 0);  // 回显给客户端
            }
        }
        // 感知到有客户端断开连接
        printf("<Bye> : A boy left!\n");
        exit(0);   // 正常退出当前进程   
}

int main(int argc, char **argv) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }
    pthread_t tid[MAXUSER + 5];
    int fd[MAXUSER + 5];
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
        int new_fd;                 // new_fd:用于接受accept返回的新fd 
        struct sockaddr_in client;       // 客户端的地址【sockaddr_in类型】 
        socklen_t len = sizeof(client);  // 地址长度
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept");
            exit(1);
        }
        if (new_fd >= MAXUSER) {
            close(new_fd);
            printf("Too many Users!\n");
            break;
        }
        // 注意理解 每次产生的fd是最小未用的  
        fd[new_fd] = new_fd;
        printf("<Accept> %s:%d!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        // 每个线程有两个独立的空间，1、存放tid 2、存放new_fd（新连接）
        pthread_create(&tid[new_fd], NULL, worker, (void *)&fd[new_fd]);
    }
    return 0;
}
