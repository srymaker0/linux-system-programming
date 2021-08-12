/*************************************************************************
	> File Name: 1.server.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Thu 12 Aug 2021 11:49:07 AM CST
 ************************************************************************/

#include "head.h"
#define MAXUSERS 100

void *worker(void *arg) {
    int sockfd;
    sockfd = *(int *)arg;
    while (1) {
        char buff[1024] = {0};
        size_t rsize = recv(sockfd, buff, sizeof(buff), 0);
        if (rsize <= 0) {
            close(sockfd);
            break;
        } else {
            send(sockfd, buff, strlen(buff), 0);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
        exit(1);
    }
    int opt, port;
    int sockfd;
    pthread_t tid[MAXUSERS + 5];
    int user[MAXUSERS + 5];
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch(opt) {
            case 'p':
                port = atoi(optarg);
                 break;
            default:
                fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
                exit(1); 
        }
    }
    if ((sockfd = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }

    printf("hello world\n");
    while (1) {
        int new_fd;
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept");
            exit(1);
        }
        if (new_fd >= MAXUSERS) {
            close(new_fd);
            printf("Too many uesrs!\n");
            break;
        }
        user[new_fd] = new_fd;
        printf("<Accept> %s:%d!\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        pthread_create(&tid[new_fd], NULL, worker, (void *)&user[new_fd]);
    }
    return 0;
}
