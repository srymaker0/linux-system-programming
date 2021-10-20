/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Sat 02 Oct 2021 03:20:53 PM CST
 ************************************************************************/

#include "head.h"
#include "./thread_pool.h"
extern char *data[1024];
extern int epollfd;
extern pthread_mutex_t mutex[1024];
void task_queue_init(struct task_queue *taskQueue, int size) {
    taskQueue->size = size;
    taskQueue->head = taskQueue->tail = 0;
    taskQueue->tail = 0;
    taskQueue->fd = calloc(size, sizeof(int *));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return ;
}
void task_queue_push(struct task_queue *taskQueue, int fd) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        printf("task queue is full!\n");
        return ;
    }
    printf("<push> : %d\n", fd);
    taskQueue->fd[taskQueue->tail++] = fd;
    taskQueue->total++;
    if (taskQueue->tail == taskQueue->size) {
        printf("task queue reach end!\n");
        taskQueue->tail = 0;
    }
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
    return ;
}

int task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    // 惊群效应，每次拿到锁需要判断total是否真的不为0（也许被其他人拿走了）
    while (taskQueue->total == 0) {
        printf("task queue is empty!\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    int fd = taskQueue->fd[taskQueue->head++];
    taskQueue->total--;
    if (taskQueue->head == taskQueue->size) {
        printf("task queue reach end!\n");
        taskQueue->head = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    return fd;
}

void do_echo(int sockfd) {
    char buffer[4096] = {0};
    int len = strlen(data[sockfd]);        // 初始化len 
    int nrecv;
    // 从sockfd中收取数据存到buffer中
    if ((nrecv = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) {
        // 若为空，则在实例中删除此fd
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
        // 关闭这个文件描述符
        close(sockfd);
        return ;
    }
    // 为这个sockfd加锁，防止数据竞争
    pthread_mutex_lock(&mutex[sockfd]);
    // 对buffer中的数据进行处理，写到开辟的data空间中
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            data[sockfd][len++] = buffer[i] + 32;
        } else if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            data[sockfd][len++] = buffer[i] - 32;
        } else {
            data[sockfd][len++] = buffer[i];
            // 收到回车需要发送数据
            if (buffer[i] == '\n') {
                // 将在data[sockfd]中处理好的数据发回sockfd中
                send(sockfd, data[sockfd], len, 0);
                // 重新初始化这片空间
                memset(data[sockfd], 0, 4096);    
            }
        }
    }
    // 处理完数据后解锁，避免死锁
    pthread_mutex_unlock(&mutex[sockfd]);
    // 由于sockfd在加入任务队列之前在main中被注销了，所以这里需要重新加进去
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}


void *thread_run(void *arg) {
    struct task_queue *taskQueue = (struct task_queue *)arg;
    pthread_detach(pthread_self());            // 分离自己 其他线程不必等
    while (1) {
        int fd = task_queue_pop(taskQueue);    // 每次从任务队列中取fd，进行操作  
        do_echo(fd);
    }
}











