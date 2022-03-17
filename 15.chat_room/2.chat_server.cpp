/*************************************************************************
	> File Name: chat_server.cpp
	> Author: 
	> Mail: 
	> Created Time: Thu 17 Mar 2022 10:39:49 AM CST
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define _GNU_SOURCE 1
#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535

struct client_data {
    sockaddr_in address;
    char *write_buf;
    char buf[BUFFER_SIZE];
};

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage : %s ip -p port!\n", argv[0]);
        exit(1);
    }
    int opt, port;
    const char* ip = argv[1];
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch(opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s ip -p port!\n", argv[0]);
                exit(1);
        }
    }
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    address.sin_addr.s_addr = inet_addr("0.0.0.0");

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert( listenfd >= 0 );
    
    int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert( ret != -1 );
    
    ret = listen(listenfd, 5);
    assert( ret != -1 );
    
    client_data *users = new client_data[FD_LIMIT];
    pollfd fds[USER_LIMIT];
    int user_counter = 0;
    
    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    for (int i = 1; i <= USER_LIMIT; i++) {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    
    while (1) {
        ret = poll(fds, user_counter + 1, -1);
        if (ret < 0) {
            printf("poll fail\n");
            break;
        }

        for (int i = 0; i < user_counter + 1; i++) {
            if (fds[i].fd == listenfd && fds[i].revents & POLLIN) {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
                if (connfd < 0) {
                    printf("errno is : %d\n", errno);
                    continue;
                }
                if (user_counter >= USER_LIMIT) {
                    const char *info = "too many users\n";
                    printf("%s", info);
                    send(connfd, info, strlen(info), 0);
                    close(connfd);
                    continue;
                }

                user_counter += 1;
                users[connfd].address = client_address;
                setnonblocking(connfd);

                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;
                printf("Comes a new user, now have %d users\n", user_counter);

            } else if (fds[i].revents & POLLERR) {
                printf("Get an errno from %d\n", fds[i].fd);
                char errors[100];
                memset(errors, 0, 100);
                continue;
                
            } else if (fds[i].revents & POLLRDHUP) {
                users[fds[i].fd] = users[fds[user_counter].fd];
                close(fds[i].fd);
                fds[i] = fds[user_counter];
                i--;
                user_counter--;
                printf("a client left\n");

            } else if (fds[i].revents & POLLIN) {
                int connfd = fds[i].fd;
                memset(users[connfd].buf, 0, BUFFER_SIZE);
                ret = recv(connfd, users[connfd].buf, BUFFER_SIZE - 1, 0);
                printf("Get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd);
                if (ret < 0) {
                    if (errno != EAGAIN) {
                        close(connfd);
                        users[fds[i].fd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                } else if (ret == 0) {
                    // close connection 
                } else {
                    for (int j = 1; j <= user_counter; j++) {
                        if (fds[j].fd == connfd) continue;
                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
            } else if (fds[i].revents & POLLOUT) {
                int connfd = fds[i].fd;
                if (!users[connfd].write_buf) continue;
                ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
                users[connfd].write_buf = NULL;
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
    delete [] users;
    close(listenfd);

    return 0;
}
