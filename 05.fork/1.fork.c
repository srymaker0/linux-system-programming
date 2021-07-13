/*************************************************************************
	> File Name: 1.fork.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 24 Mar 2021 06:39:38 PM CST
 ************************************************************************/

#include "../common/head.h"
int main() {
    pid_t pid;
    if ((pid = fork()) < 0) {     //fork return child's pid
        perror("fork()");
        exit(1);
    }
    if (pid) {
        printf("In Parent Process! <%d>--<%d>--<%d>\n", getppid(), getpid(), pid);
    } else {
        printf("In Child Process! <%d>--<%d>\n", getppid(), getpid());
    }
    //printf("Hello Kaikeba!\n");
    return 0;
}
