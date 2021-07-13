/*************************************************************************
	> File Name: 7.select.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Thu 15 Apr 2021 02:54:09 PM CST
 ************************************************************************/

#include "./common/head.h"

int main() {
    fd_set rfds;
    struct timeval tv;
    int ret;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    ret = select(1, &rfds, NULL, NULL, &tv);
    if (ret == -1) 
        perror("select()");
    else if (ret) {
        printf("Data is available now.\n");
    } else 
        printf("No data within five seconds.\n");
    return 0;
}

