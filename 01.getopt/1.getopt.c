/*************************************************************************
	> File Name: 1.getopt.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed Mar 17 20:11:36 2021
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int opt;
    //定义三个参数
    while ((opt = getopt(argc, argv, "ab:c::")) != -1) {         //getopt 返回ACS码，表示成功接收
        switch (opt) {
            case 'a': 
                printf("opt a found!\n");
                printf("ind = %d\n", optind);                    //返回下一个参数的下标
                break;
            case 'b':
                printf("opt b found!\n");
                printf("ind = %d\n", optind);
                printf("arg for b = %s\n", optarg);              //打印b的参数
                break;
            case 'c':
                printf("opt c found!\n");
                printf("ind = %d\n", optind);
                printf("arg for c = %s\n", optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -a -b barg -c[carg]!\n", argv[0]);
                exit(1);
        }
    }
}
