/*************************************************************************
	> File Name: 1.server.c
	> Author: 
	> Mail: 
	> Created Time: Tue 19 Oct 2021 10:02:46 PM CST
 ************************************************************************/

#include "head.h"
#include "./thread_poll/thread_pool.h"

#define MAXUSER 1024
#define MAXEVENTS 5
#define MAXQUEUE 1000
#define MAXTHREAD 4

char *data[MAXUSER];
pthread_mutex_t mutex[MAXUSER];     // data race
int fd[MAXUSER], epollfd;

int main(int argc, char **argv) {
    int opt, port;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p': {
                port = atoi(optarg);   
            } break;
            default : {
                fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
                exit(1); 
            }
        }
    }
    
    int sockfd, server_listen;
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_created");
        exit(1);
    }
    
    // 创建epoll实例
    if ((epollfd = epoll_create(1)) < 0) {
        perror("epollfd_create");
        exit(1);
    }
    
    // 初始化互斥锁
    for (int i = 0; i < MAXUSER; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    // 初始化任务队列
    struct task_queue taskQueue;
    task_queue_init(&taskQueue, MAXQUEUE);
    
    // 存储工作线程
    pthread_t *tid = (pthread_t *)calloc(MAXTHREAD, sizeof(pthread_t));
    
    // 创建线程
    for (int i = 0; i < MAXTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)&taskQueue);
    }

    struct epoll_event ev, events[MAXEVENTS];
    // 初始化epoll实例
    ev.data.fd = server_listen;
    ev.events = EPOLLIN;

    // 将server_listen加入到epollfd指向的实例
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl1");
        exit(1);
    }
    
    for (;;) {
        // 每一个线程都会等待事件
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds < 0) {
            perror("epollfd_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            // 若是监听套接字，进行三次握手
            if (events[i].data.fd == server_listen && (events[i].events & EPOLLIN)) {
                // 产生一个新的fd用于处理数据
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept");
                    exit(1);
                }
                fd[sockfd] = sockfd;
                data[sockfd] = (char *)calloc(4096, sizeof(char));
                ev.data.fd = sockfd;
                ev.events = EPOLLIN;
                // 将新产生的sockfd加入到epoll实例中
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd[sockfd], &ev) < 0) {
                    perror("epoll_ctl2");
                    exit;
                }
            } else {
                // 否则说明是有事件发生（sockfd）
                if (events[i].events & EPOLLIN) {       // 判断是否是感兴趣的事件
                    // 先删除，保证用户对应的空间只有一个线程可以操作
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    // 加入到任务队列中，让线程去取
                    task_queue_push(&taskQueue, events[i].data.fd);
                }
            }
        }
    }

    return 0;
}
