/*************************************************************************
	> File Name: 3.exec.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 24 Mar 2021 08:32:33 PM CST
 ************************************************************************/

#include "head.h"

int main() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) {
        execl("/bin/cat", "cat", "/etc/hosts", NULL);
        prinf("Hello world\n");
    } else {
        wait(NULL);   // 只负责收尸，不关注子进程如何结束
    }


    return 0;
}
