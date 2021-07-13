/*************************************************************************
	> File Name: 4.popen.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Fri 02 Apr 2021 03:26:48 PM CST
 ************************************************************************/

#include "head.h"

int main(int argc, char *argv[]) {
    FILE *fp;
    char buff[512] = {0};
    if ((fp = my_popen("cat ./1.popen.c", "r")) == NULL ) {
        perror("popen");
        exit(1);
    }
    while (fgets(buff, sizeof(buff), fp) != NULL) {
        printf("%s", buff);
        memset(buff, 0, sizeof(buff));
    }
    sleep(100);
    my_pclose(fp);
    sleep(1000);
    return 0;
}

