/*************************************************************************
	> File Name: 1.server.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 07 Jul 2021 11:36:39 PM CST
 ************************************************************************/

#include "head.h"
#define MAX 20

// 用户信息
struct User{
    int fd;
    char ip[20];
    char name[20];
};

struct Packet{
    char filename[50];    // 文件名  
    ssize_t size;         // 整个文件大小
    char body[1024];      // 一个packet大小
};

int recv_file(int sockfd, const char *datadir) {
    struct Packet packet, packet_t, packet_pre;
    size_t packet_size = sizeof(struct Packet);
    size_t offset = 0, recv_size = 0, file_cnt = 0, total_size = 0;   // file_cnt文件数量 total_size收到的文件总大小
    FILE *fp;
    // 创建目录
    if (mkdir(datadir, 0755) < 0) {
        if (errno == EEXIST) {
            printf("Dir Exist!\n");
        } else {
            return -1;
        }
    }
    while (1) {
        // 若packet_pre中有数据，需要先拷贝至packet中
        if (offset) {
            memcpy((char *)&packet, &packet_pre, offset);
        }
        while ((recv_size = recv(sockfd, (void *)&packet_t, packet_size, 0)) > 0) {
            // 存在两种情况 1、数据大小刚好等于packet_size 2、粘包之后的offset和recv_size刚好等于packet_size
            if (offset + recv_size == packet_size) {                    
                printf("整包!\n");
                // 拷贝至packet  ｜+offset，对应情况2，考虑到packet中已经有数据(偏移量为offset)
                memcpy((char *)&packet + offset, &packet_t, recv_size);         // 需要强转才可做加运算    
                offset = 0;
                break;          // 退出循环，递交
            } else if (offset + recv_size < packet_size) {
                printf("拆包!\n");
                memcpy((char *)&packet + offset, &packet_t, recv_size);
                offset += recv_size;
            } else {
                printf("粘包!\n");
                // ！注意：这里拷贝的大小为packet_size - offset, 是packet中空余的大小
                memcpy((char *)&packet + offset, &packet_t, packet_size - offset);
                // 将packet_t中剩余的拷贝至packet_pre中
                memcpy((char *)&packet_pre, (char *)&packet_t + (packet_size - offset),\
                       recv_size - (packet_size - offset));
                offset = recv_size - (packet_size - offset);                    // 更新offset
                break;
            }
        }
        if (!file_cnt) {
            printf("Saving file <%s> to ./%s/...\n", packet.filename, datadir);
            char filepath[512] = {0};
            sprintf(filepath, "./%s/%s", datadir, packet.filename);             // 字符串拼接
            // fp指向需要写入的文件，若没有则会自己创建
            if ((fp = fopen(filepath, "wb")) == NULL) {                         // b:用于处理二进制文件，图像，音视频
                perror("fopen");
                return -1;
            }
        }
        file_cnt++;
        size_t write_size;
        // 1、还没写完
        if (packet.size - total_size >= packet_size) {
            write_size = fwrite(packet.body, 1, sizeof(packet.body), fp);
        } else {
            // 2、还剩多少写多少，这样不会出现最后收到的大小>实际文件大小（避免写入多余的空白数据）
            write_size = fwrite(packet.body, 1, packet.size - total_size, fp);
        }
        total_size += write_size;                       // 更新收到的文件大小（即总共写入多少）
        if (total_size >= packet.size) {                // 若写入的>=文件大小，退出大循环    
            printf("File Finished!\n");
            break;
        }
    }
    fclose(fp);                                         // 最后记得关闭文件，防止内存泄露
    return 0;
}

void *do_work(void *arg) {
    struct User *user;
    user = (struct User *)arg;

    printf("Waiting for data: <%s:%s>\n", user->ip, user->name);
    recv_file(user->fd, user->name);
    close(user->fd);
    return NULL;
}

int main(int argc, char **argv) {
    int server_listen, port, sockfd;
    pthread_t tid[MAX + 5] = {0};
    struct User users[MAX + 5];
    bzero(&users, sizeof(users));
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
    }
    port = atoi(argv[1]);
    if ((server_listen = socket_create(port)) < 0) {
        perror("server_listen");
        exit(1);
    }
    printf("After socket_create <%d>\n", server_listen);
    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        if ((sockfd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept");
            exit(1);
        }
        // ！先收到名字之后再收信息
        if (recv(sockfd, &users[sockfd].name, sizeof(users[sockfd].name), 0) <= 0) {
            close(sockfd);
            continue;
        } else {
            users[sockfd].fd = sockfd;
            strcpy(users[sockfd].ip, inet_ntoa(client.sin_addr));
        }
        // ！做同步，防止文件名和数据混在一起
        int ack = 1;
        send(sockfd, (void *)&ack, sizeof(int), 0);
        // 创建线程，接收文件
        pthread_create(&tid[sockfd], NULL, do_work, (void *)&users[sockfd]);
    }
    return 0;
}
