/*************************************************************************
	> File Name: 1.server.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 07 Jul 2021 11:36:39 PM CST
 ************************************************************************/

#include "head.h"
#define MAX 20
struct User{
    int fd;
    char ip[20];
    char name[20];
};

struct Packet{
    char filename[50];
    ssize_t size;
    char body[1024];
};

int recv_file(int sockfd, const char *datadir) {
    struct Packet packet, packet_t, packet_pre;
    size_t packet_size = sizeof(struct Packet);
    size_t offset = 0, recv_size = 0, cnt = 0, total_size = 0;
    FILE *fp;
    if (mkdir(datadir, 0755) < 0) {
        if (errno == EEXIST) {
            printf("Dir Exist!\n");
        } else {
            return -1;
        }
    }
    while (1) {
        while ((recv_size = recv(sockfd, (void *)&packet_t, packet_size, 0)) > 0) {
            if (offset + recv_size == packet_size) {
                printf("整包!\n");
                memcpy((void *)&packet + offset, &packet_t, recv_size);
                offset = 0;
                break;
            } else if (offset + recv_size < packet_size) {
                printf("拆包!\n");
                memcpy((char *)&packet + offset, &packet_t, recv_size);
                offset += recv_size;
            } else {
                printf("粘包!\n");
                memcpy((char *)&packet + offset, &packet_t, packet_size - offset);
                memcpy((char *)&packet_pre, (char *)&packet_t + (packet_size - offset),\
                       recv_size - (packet_size - offset));
                offset = recv_size - (packet_size - offset);
                break;
            }
        }
        if (!cnt) {
            printf("Saving file <%s> to ./%s/...\n", packet.filename, datadir);
            char filepath[512] = {0};
            sprintf(filepath, "./%s/%s", datadir, packet.filename);
            if ((fp = fopen(filepath, "wb")) == NULL) {
                perror("fopen");
                return -1;
            }
        }
        cnt++;
        size_t write_size;
        if (packet.size - total_size >= packet_size) {
            write_size = fwrite(packet.body, 1, sizeof(packet.body), fp);
        } else {
            write_size = fwrite(packet.body, 1, packet.size - total_size, fp);
        }
        total_size += write_size;
        if (total_size >= packet.size) {
            printf("File Finished!\n");
            break;
        }
    }
    fclose(fp);
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
    if ((server_listen = socket_create(port) < 0)) {
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
        // 做同步，收到名字之后再收信息
        if (recv(sockfd, &users[sockfd].name, sizeof(users[sockfd].name), 0) <= 0) {
            close(sockfd);
            continue;
        } else {
            users[sockfd].fd = sockfd;
            strcpy(users[sockfd].ip, inet_ntoa(client.sin_addr));
        }
        int ack = 1;
        send(sockfd, (void *)&ack, sizeof(int), 0);
        pthread_create(&tid[sockfd], NULL, do_work, (void *)&users[sockfd]);

    }


}
