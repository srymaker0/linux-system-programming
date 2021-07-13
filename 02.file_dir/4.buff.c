/*************************************************************************
	> File Name: 4.buff.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 23 Mar 2021 10:48:22 AM CST
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>

int main() {
    fprintf(stderr, "hello world!");
    printf("hello world!");
    fflush(stdout);
    sleep(2);
    printf("\n");
    sleep(5);
    return 0;
}
