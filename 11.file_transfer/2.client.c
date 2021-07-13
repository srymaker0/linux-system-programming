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
    fseek(fp, 0, SEEK_END);
    packet.size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    while (1) {
        ssize_t nread = fread(packet.body, 1, sizeof(packet.body), fp);
        if (nread <= 0) {
            break;
        }
        send(sockfd, (void *)&packet, sizeof(packet), 0);
        memset(packet.body, 0, sizeof(packet.body));
    }
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage : %s ip port filr\n", argv[0]);
        exit(1);
    }
    int sockfd, port, ack;
    char ip[20] = {0}, file[50] = {0};
    strcpy(ip, argv[1]);
    port = atoi(argv[2]);
    strcpy(file, argv[3]);
    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        exit(1);
    }
    send(sockfd, "suyelu", sizeof("suyelu"), 0);;
    int rsize = recv(sockfd, (void *)&ack, sizeof(int), 0);
    if (rsize <= 0 || ack != 1) {
        close(sockfd);
        exit(1);
    }
    printf("Waiting for data transfer!\n");
    send_file(sockfd, file);
    close(sockfd);
    return 0;
}

