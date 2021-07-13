/*************************************************************************
	> File Name: 6.popen.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Fri 23 Apr 2021 10:15:43 AM CST
 ************************************************************************/

#include "../common/head.h"

int main(int argc, char **argv) {
    FILE *fp;
    if ((fp = popen("cat ./6.popen.c", "r")) == NULL) {
        perror("popen");
        exit(1);
    }
    
    while(1) {
        char buff[512] = {0};
        size_t nread = fread(buff, 1, sizeof(buff) - 1, fp);
        printf("%s", buff);
        if (nread < sizeof(buff - 1)) {
            break;
        }
    }
    pclose(fp);
}

