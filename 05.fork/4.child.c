/*************************************************************************
	> File Name: 4.child.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sat 17 Apr 2021 03:46:01 PM CST
 ************************************************************************/

#include "../common/head.h"

int main() {
    pid_t pid;
    int i;
    for (i = 0; i < 10; i++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) break;
    }
    printf("我是第 %d 个孩子\n", i);
    sleep(10);
    printf("我还是第 %d 个孩子\n", i);
    return 0;
}

