/*************************************************************************
	> File Name: 1.sem.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sat 24 Apr 2021 03:48:59 PM CST
 ************************************************************************/

#include "head.h"
#define INS 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int created_sem(const char *path, int project_id, int nsems) {
    int sem_id;
    key_t key = ftok(path, project_id);
    // nsems 表示信号量的数量
    if ((sem_id = semget(key, nsems, IPC_CREAT | 0666)) < 0) {
        return -1;
    }
    return sem_id;
}

// sem_id 哪个信号   sem_num 信号集合中的第几个信号量
int init_sem(int sem_id, int sem_num, int value) {
    union semun arg;
    arg.val = value;
    return semctl(sem_id, sem_num, SETVAL, arg);
}

int P(int sem_id, int sem_num) {
    struct sembuf sbuff;
    sbuff.sem_num = sem_num;
    sbuff.sem_op = -1;
    sbuff.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sbuff, 1) < 0) {
        return -1;
    }
    return 0;
}

int V(int sem_id, int sem_num) {
    struct sembuf sbuff;
    sbuff.sem_num = sem_num;
    sbuff.sem_op = 1;
    sbuff.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sbuff, 1) < 0) {
        return -1;
    }
    return 0;
}
int main(int argc, char **argv) {
    int sem_id;
    if ((sem_id = created_sem(".", 2021, 1)) < 0) {
        perror("create_sem");
        exit(1);
    }
    if (argc > 1) {
        if (init_sem(sem_id, 0, INS) < 0) {
            perror("init");
            exit(1);
        }
    }
    int cnt = 0;
    while (1) {
        if (P(sem_id, 0) < 0) {
            perror("P");
            exit(1);
        }
        sleep(3);
        cnt++;
        if (V(sem_id, 0) < 0) {
            perror("V");
            exit(1);
        }
        printf("This is %dth of mine!\n", cnt);
    }
    return 0;
}
