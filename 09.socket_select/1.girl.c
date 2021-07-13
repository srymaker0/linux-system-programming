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

/* 用select IO感知 事件驱动 单进程也可以 弊端数量有限 */
int main(int argc, char **argv) {
    if (argc != 3) {
        Usage(argv[0]);
        exit(1);
    }
    pthread_t tid[MAXUSER + 5];
    int fd[MAXUSER + 5] = {0};
    int opt, port, server_listen, maxfd = 0;
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
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_creat");
        exit(1);
    }
    fd_set rfds;                                // 定义文件描述符集合
    struct timeval tv;
    maxfd = server_listen;
    fd[server_listen] = server_listen; 
    //tv.tv_sec = 5;
    //tv.tv_usec = 0;
    // 持续监听，接受多个连接
    while (1) {
        // 如果循环使用select，每次调用select前需要重新初始化每个集合
        FD_ZERO(&rfds);                         // 清空
        for (int i = 0; i < MAXUSER; i++) {
            if (fd[i] > 0) {
                FD_SET(fd[i], &rfds);           // 将监听文件描述符加入到rfds中    
            }
        }
        // select感知 
        int rnum = select(maxfd + 1, &rfds, NULL, NULL, NULL);   // 最大文件描述符+1，第五个参数NULL：无限期阻塞
        // 返回值<0 说明出错
        if (rnum < 0) {
            perror("select");
            exit(1);
        } 
        // 若server_listen在rfds集合中 则说明有新的连接请求
        if (FD_ISSET(server_listen, &rfds)){
            int new_fd;                         // new_fd:用于接受accept返回的新fd 
            struct sockaddr_in client;          // 客户端的地址【sockaddr_in类型】 
            socklen_t len = sizeof(client);     // 地址长度
            if ((new_fd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
                perror("accept");
                exit(1);
            }
            if (new_fd >= MAXUSER) {
                close(new_fd);
                printf("Too many Users!\n"); 
                break;
            }
            if (maxfd < new_fd) maxfd = new_fd;
            // 注意理解 每次产生的fd是最小未用的  
            fd[new_fd] = new_fd;
            printf("<Accept> %s:%d!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        }
        // 回显
        for (int i = 0; i < MAXUSER; i++) {
            if (fd[i] == server_listen) continue;
            if (fd[i] > 0 && FD_ISSET(fd[i], &rfds)) {
                char buff[1024] = {0};
                int rsize = recv(fd[i], buff, sizeof(buff), 0);
                if (rsize <= 0) {
                    close(fd[i]);
                    fd[i] = 0;
                } else {
                    send(fd[i], buff, strlen(buff), 0);
                }
            }
        }
    }
    return 0;
}
