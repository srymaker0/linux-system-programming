/*************************************************************************
	> File Name: 2.fork2.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 24 Mar 2021 06:53:45 PM CST
 ************************************************************************/

#include "../common/head.h"

int main() {
    pid_t pid;
    int i = 1;
    for (; i <= 10; i++) {
        sleep(1);
        if ((pid = fork()) < 0) {
            perror("fork()");
            exit(1);
        }
        if (pid == 0) {
            break;
        }
    }
    if (pid == 0) {
        printf("I'm the %d Child Process!\n", i);
    } else {
        sleep(2);
        printf("I'm Parent Process!\n");
    }

    return 0;
}
