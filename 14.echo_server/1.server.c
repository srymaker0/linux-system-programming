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
#define MAXQUEUE 20
#define MAXTHREAD 4

char *data[MAXUSER];
pthread_mutex_t mutex[MAXUSER];     // data race
int fd[MAXUSER];

int main(int argc, char **argv) {
    int opt, port;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        case 'p': {
            port = atoi(optarg);   
        } break;
        default : {
            fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
            exit(1); 
        }
    }
    
    int sockfd, server_listen;
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_created");
        exit(1);
    }
    int epollfd;
    if ((epollfd = epoll_create(1)) < 0) {
        perror("epollfd_create");
        exit(1);
    }

    for (int i = 0; i < MAXUSER; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    struct task_queue taskQueue;
    task_queue_init(&taskQueue, MAXQUEUE);
    
    // 存储工作线程
    pthread *tid = (pthread_t *)calloc(MAXTHREAD, sizeof(thread_t));
    for (int i = 0; i < MAXTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_run, NULL);
    }

    struct epoll_event ev, events[MAXEVENTS];
    
    ev.data.fd = server_listen;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror(epoll_ctl);
        exit(1);
    }
    
    for (;;) {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds < 0) {
            perror("epollfd_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_listen && (events[i].events & EPOLLIN)) {
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept");
                    exit(1);
                }
                fd[sockfd] = sockfd;
                data[sockfd] = (char *)calloc(4096, sizeof(char));
            }
        }
    }

    return 0;
}
