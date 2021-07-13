/*************************************************************************
	> File Name: 4.thread_pool_test.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Fri 07 May 2021 10:21:57 AM CST
 ************************************************************************/

#include "head.h"
#define INS 5
#define QUEUE_SIZE 500

void *do_work(void *arg) {
    pthread_detach(pthread_self());  // 分离自己，从而不需要其他线程join
    struct task_queue *taskQueue = (struct task_queue *)arg;
    // 时刻待命：输出字符串
    while (1) {
        char *str = task_queue_pop(taskQueue);
        usleep(20);
        printf("<%ld> : %s \n", pthread_self(), str);
    }
}

int main() {
    pthread_t tid[INS + 1];
    struct task_queue taskQueue;
    task_queue_init(&taskQueue, QUEUE_SIZE);
    // 使用二维为了避免线程处理数据时，外部改变了数据
    char buff[QUEUE_SIZE][10] = {0};
    // 创建INS个线程【传入任务队列】
    for (int i = 0; i < INS; i++) {
        pthread_create(&tid[i], NULL, do_work, (void *)&taskQueue);
    }
    // 布置任务
    int sub = 0;
    while (1) {
        FILE *fp = fopen("./4.thread_pool_test.c", "r");
        if (fp == NULL) {
            perror("fopen");
            exit(1);
        }
        while (fgets(buff[sub], 1024, fp) != NULL) {
            task_queue_push(&taskQueue, buff[sub]);
            if (++sub == QUEUE_SIZE) {
                sub = 0;
            }
            // 若任务队列满了
            if (taskQueue.total == taskQueue.size) {
                while (1) {
                    // 当有线程取走了任务
                    if (taskQueue.total < taskQueue.size) {
                        break;
                    }
                    usleep(10000);   // 避免while太耗CPU资源
                }
            }
        }
        fclose(fp);
    }
    return 0;
}
