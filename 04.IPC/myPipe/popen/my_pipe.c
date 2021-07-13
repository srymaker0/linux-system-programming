/*************************************************************************
	> File Name: my_pipe.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Fri 23 Apr 2021 10:40:36 AM CST
 ************************************************************************/

#include "head.h"

static pid_t *childpid = NULL;       //定义为全局 为了给my_pclose传参
static int maxsize = 0;              //当前进程中能使用popen调用最多的次数

FILE *my_popen(const char *command, const char *type) {
    FILE *fp;
    int pipefd[2];     //0 read 1 write
    pid_t pid;
    
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != '\0') {
        errno = EINVAL;
        return NULL;
    }
    
    if (childpid == NULL) {
        maxsize = sysconf(_SC_OPEN_MAX);
        if ((childpid = (pid_t *)calloc(maxsize, sizeof(pid_t))) == NULL) {
            return NULL;
        }
    }


    if (pipe(pipefd) < 0) {
        return NULL;
    }

    if ((pid = fork()) < 0) {
        return NULL;
    }

    if (pid == 0) {
        if (type[0] == 'r') {      //父进程要读
            close(pipefd[0]);      //子进程关闭读文件， 要写
            if (pipefd[1] != STDOUT_FILENO) {
                dup2(pipefd[1], STDOUT_FILENO); //把写入的复制到标准输出文件
                close(pipefd[1]);   //子进程关闭写文件
            }
        } else {
            close(pipefd[1]);
            if (pipefd[0] != STDIN_FILENO) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
            }
        }
        execl("/bin/sh", "sh", "-c", command, NULL);
    } else {
        if (type[0] == 'r') {      //说明父进程要读
            close(pipefd[1]);      //父进程关闭写文件描述符
            if ((fp = fdopen(pipefd[0], type)) == NULL) {    //将文件描述符转换为一个文件指针
                return NULL;
            }
        } else {
            close(pipefd[0]);
            if ((fp = fdopen(pipefd[1], type)) == NULL) {
                return NULL;
            }
        }
    }
    childpid[fileno(fp)] = pid;  //将fp转化为fd，把pid存到childpid中
    return fp;
}


int my_pclose(FILE *fp) {
    int status, pid, fd;
    fd = fileno(fp);
    pid = childpid[fd];
    if (pid == 0) {
        errno = EINVAL;
        return -1;
    }
    fflush(fp);
    close(fd);
    wait4(pid, &status, 0, NULL);
    return status;
}
