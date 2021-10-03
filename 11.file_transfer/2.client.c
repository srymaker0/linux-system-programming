/*************************************************************************
	> File Name: 2.client.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 07 Jul 2021 11:36:46 PM CST
 ************************************************************************/

#include "head.h"
struct Packet {
    char filename[50];
    ssize_t size;
    char body[1024];
};

int send_file(int sockfd, const char *file) {
    FILE *fp;
    struct Packet packet;
    memset(&packet, 0, sizeof(packet));
    strcpy(packet.filename, file);
    if ((fp = fopen(file, "r")) == NULL) {
        perror("fopen");
        return -1;
    }
    fseek(fp, 0, SEEK_END);     // 将偏移量指向文件末尾，其实就是获取文件大小 
    packet.size = ftell(fp);    // 获取当前文件指针的偏移量
    fseek(fp, 0, SEEK_SET);     // 重新复位，为了发数据
    while (1) {
        // 读文件存储到packet.body中
        ssize_t nread = fread(packet.body, 1, sizeof(packet.body), fp);
        if (nread <= 0) {
            break;              // 读取失败或读完，退出循环
        }
        // 发送packet
        send(sockfd, (void *)&packet, sizeof(packet), 0);
        // 发送完一次后需要清零，为下一次数据做准备
        memset(packet.body, 0, sizeof(packet.body));
    }
    // 把打开的文件关了
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage : %s ip port file\n", argv[0]);
        exit(1);
    }
    int sockfd, port, ack;
    char ip[20] = {0}, file[50] = {0};
    // init...
    strcpy(ip, argv[1]);
    port = atoi(argv[2]);
    strcpy(file, argv[3]);
    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        exit(1);
    }
    // 1、发送姓名
    send(sockfd, "ZYJ", sizeof("ZYJ"), 0);;
    // 2、同步机制，接收到ack说明服务端收到用户信息
    int rsize = recv(sockfd, (void *)&ack, sizeof(int), 0);
    if (rsize <= 0 || ack != 1) {
        close(sockfd);
        exit(1);
    }
    printf("Waiting for data transfer!\n");
    // 3、发送文件
    send_file(sockfd, file);
    // 4、关闭文件描述符
    close(sockfd);
    return 0;
}

