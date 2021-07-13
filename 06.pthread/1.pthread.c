/*************************************************************************
	> File Name: 1.pthread.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Thu 06 May 2021 06:56:35 PM CST
 ************************************************************************/

#include "head.h"
#define INS 100  //并发量 
struct Arg{
    int age;
    char name[20];
};
void *print(void *arg) {
    struct Arg argin;            
    argin = *(struct Arg *)arg;   // 用变量取值
    printf("%s is %d years old!\n", argin.name, argin.age);
}

int main() {
    pthread_t thread;
    pthread_t *tid = calloc(INS + 5, sizeof(pthread_t));
    struct Arg *arg = calloc(INS + 5, sizeof(struct Arg));
    for (int i = 1; i <= INS; i++) {
        strcpy(arg[i].name, "zyj");
        arg[i].age = i;
        // 创建线程 NULL 默认属性 arg 函数参数
        pthread_create(&tid[i], NULL, print, &arg[i]);
    }
    for (int i = 1; i <= 100; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}

