/*************************************************************************
	> File Name: 5.nonblock.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Tue 23 Mar 2021 08:48:58 PM CST
 ************************************************************************/

#include "./common/head.h"

int main() {
    int age1 = 0, age2 = 0;
    /*
    int flag = fcntl(0, F_GETFL);     //获取标准输出的文件标志
    flag |= O_NONBLOCK;               //按位或 设置非阻塞  
    fcntl(0, F_SETFL, flag);          //将flag设置回标准输出  
    sleep(2);                         //等待2s  
    */
    make_nonblock(0);
    sleep(2);
    int ret = scanf("%d", &age1);
    if (ret < 0) {
        if (errno == EAGAIN) {
            printf("EAGIN\n");
        }
        perror("scanf");
    }
    make_block(0);
    scanf("%d", &age2);
    printf("Zyj is %d <%d> years old!\n", age1, age2);
    return 0;
}
