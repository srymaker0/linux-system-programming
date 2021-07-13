/*************************************************************************
	> File Name: 1.fd.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Fri Mar 19 18:15:28 2021
 ************************************************************************/

#include "./common/head.h"

int main(int argc, char **argv) {
    int fd, fd2;                 //define a Filedirector 
    
    if (argc != 3) {
        fprintf(stderr, "Usage : %s file1 file2\n", argv[0]);
        exit(1);
    }
    //open a file , if fd < 0 means open failed
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        //fprintf(stderr, "Erro\n");   //打印Erro到stderr标准错误输出文件中
        perror("open");              //打印错误信息，如果出错，会打印open:..... 这个冒号会自动补
        exit(1);                     //exit the process aka exit the main() 
    }

    if ((fd2 = open(argv[2], O_CREAT | O_RDWR, 0666)) < 0) {    //创建一个文件，可读可写 0-二进制  666，给所有人可读可写的权限
        perror("open");
        exit(1);
    }

    while (1) {
        char buff[512] = {0};       //clear the buff each time 
        ssize_t size = read(fd, buff, sizeof(buff));       //read a file from fd to the buff which size is sizeof(buff)
        printf("%s", buff);
        write(fd2, buff, size);
        if (size <= 0) {
            break;
        }
    }
    //ok, we open a file, now we could read the file.
    close(fd);
    close(fd2);
    //perror("open2");         //to hear , Succeed to open the file    
    return 0;
}
