/*************************************************************************
	> File Name: 4.multi_thread_add.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sat 26 Jun 2021 08:26:13 PM CST
 ************************************************************************/

#include "head.h"
#define INS 5

struct Msg {
    int now, sum;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct Msg *share_memory;   // 共享信息的地址

void do_add(int x) {
    while (1) {
        pthread_mutex_lock(&share_memory->mutex);
        // 等待，需要有加锁的mutex
        pthread_cond_wait(&share_memory->cond, &share_memory->mutex); 
        // 每个进程一次计算100次
        for (int i = 0; i < 100; i++) {
            if (++share_memory->now > 10000) {
                pthread_mutex_unlock(&share_memory->mutex);
                usleep(10);
                // 记得发信号，否则其他线程陷入无限等待
                pthread_cond_signal(&share_memory->cond);
                exit(0);  // 有两个循环，直接exit，而不能break
            }
            share_memory->sum += share_memory->now;
            printf("<%d> now = %d, sum = %d\n", x, share_memory->now, share_memory->sum);
            fflush(stdout);
        }
        // 先解锁，再发信号
        pthread_mutex_unlock(&share_memory->mutex);
        usleep(10);
        pthread_cond_signal(&share_memory->cond);
    }
    exit(0);
}

int main() {
    pid_t pid;
    int x = 0, shmid;
    key_t key = ftok(".", 2021);
    if ((shmid = shmget(key, sizeof(struct Msg), IPC_CREAT | IPC_EXCL | 0600)) < 0) {
        perror("shmget");
        exit(1);
    }
    share_memory = (struct Msg *)shmat(shmid, NULL, 0); // 使用结构体指针指向共享地址
    if (share_memory == (void *)-1) {
        perror("share_memory");
        exit(1);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, 1);

    pthread_mutex_init(&(share_memory->mutex), &attr);

    pthread_condattr_t c_attr;
    pthread_condattr_init(&c_attr);
    pthread_condattr_setpshared(&c_attr, 1);

    pthread_cond_init(&share_memory->cond, &c_attr);

    // 创建INS个子进程
    for (int i = 0; i <= INS; i++) {
        if ((pid = fork()) < 0) {
            perror("fork");
        }
        x = i;                                      // 标识孩子编号
        if (pid == 0) break;
    }
    
    if (pid == 0) {
        // 子进程做累加计算
        do_add(x);
        shmdt(share_memory);                        // 分离共享内存
    } else {
        usleep(10);
        pthread_cond_signal(&share_memory->cond);   // 最先发送信号，激活条件变量
        // 父进程等待孩子终止
        for (int i = 1; i <= INS; i++) {
            wait(NULL);
        }
        printf("share_memory->sum = %d\n", share_memory->sum);
        shmdt(share_memory);                        // 分离共享内存
        shmctl(shmid, IPC_RMID, NULL);              // 销毁共享内存
    }
    return 0;
}
