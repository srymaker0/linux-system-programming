/*************************************************************************
	> File Name: 1.shm.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sun 28 Mar 2021 03:51:09 PM CST
 ************************************************************************/

#include "../common/head.h"

int main() {
    pid_t pid;
    int shmid;
    char *share_memory = NULL;
    // 约定共享内存的位置
    key_t key = ftok("./1.shm.c", 328);   // 获得key值 第二个参数不变生成的key值也不会变
    // 通过key获得id，0666设置权限:终端输入ipcs——perms字段
    if ((shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }
    // 将share_memory通过shmid附着在调用进程上
    if ((share_memory = shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat");
        exit(1);
    }
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }
    // father process input and child process output
    // 父子进程共享同一块存储空间
    if (pid) {
        while (1) {
            scanf("%s", share_memory);
            getchar();
        }
    } else {
        while (1) {
            sleep(2);
            if (strlen(share_memory)) printf("<Child> : %s\n", share_memory);
            memset(share_memory, 0, 4096);
        }
    } 
    return 0;
}
