/*************************************************************************
	> File Name: 1.server.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sun 18 Jul 2021 02:33:11 PM CST
 ************************************************************************/

#include "head.h"
#define MAX 100         // 最大用户数量
#define MAXQUEUE 10
#define EPOLL_SIZE 5

/**************In reactor.h***************

// 客户的请求
struct logRequest {
    char name[20];    
    int sex;          // 性别 1 男 0 女
};

// 服务端给客户的回应
struct logResponse {
    int flag;         // 1 正确连接，0 出错
    char msg[512];    // 回应消息
};

 写在reactor.h中  存储用户信息 由两个从反应堆维护
struct User {
    int fd;
    char name[20];
    char ip[20];
    int online;
    int epollfd;   // 用户存在的反应堆
};
******************************************/

// 合伙人处理函数
void *subreactor(void *arg) {
    int epollfd = *(int *)arg;
    struct epoll_event ev, events[5];
    while (1) {
        int nfds = epoll_wait(epollfd, events, 5, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            char buff[512] = {0};
            struct User *user;
            user = (struct User *)events[i].data.ptr;
            int nrecv = recv(user->fd, buff, sizeof(buff), 0);
            if (nrecv < 0) {
                // 客户离线
                epoll_ctl(epollfd, EPOLL_CTL_DEL, user->fd, NULL);
                close(user->fd);
                user->online = 0;               
            }
            printf("<sub_%d_%s> : %s\n", epollfd, user->name, buff);
        }
    }
}

int main(int argc, char **argv) {
    int opt, listener, epollfd, epollfd_boy, epollfd_girl, port;   // 主：epollfd  从：boy&girl
    struct User *users_girl, *users_boy;                           // 用户空间 
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
                exit(1);
        }
    }
    
    if ((listener = socket_create(port)) < 0) {
        perror("socket_create");
        exit(1);
    }

    // 创建epoll实例 反应堆
    epollfd = epoll_create(1);
    epollfd_boy = epoll_create(1);
    epollfd_girl = epoll_create(1);
    
    if (epollfd < 0 || epollfd_boy < 0 || epollfd_girl < 0) {
        perror("epoll_create");
        exit(1);
    }
    
    // 开辟boy和girl空间
    users_boy = (struct User *)calloc(MAX, sizeof(struct User));
    users_girl = (struct User *)calloc(MAX, sizeof(struct User));
    
    // 任务队列，独立维护
    struct task_queue boyQueue;
    struct task_queue girlQueue;
    
    // 初始化任务队列
    task_queue_init(&boyQueue, MAXQUEUE);
    task_queue_init(&girlQueue, MAXQUEUE);

    // 工作线程
    pthread_t tid_boy, tid_girl;
    
    pthread_create(&tid_boy, NULL, subreactor, &epollfd_boy);
    pthread_create(&tid_girl, NULL, subreactor, &epollfd_girl);
   
    printf("<%d> : boy\n<%d> : girl\n", epollfd_boy, epollfd_girl);

    // 创建主反应堆 等待用户接入——将listener加入到反应堆
    struct epoll_event ev, events[EPOLL_SIZE]; 
   
    // 注册epoll实例
    ev.events = EPOLLIN;     // 接收连接
    ev.data.fd = listener;
    
    // 将listener加入到主反应堆中
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    for (;;) {
        // 等待  -1无限期等待
        int nfds = epoll_wait(epollfd, events, EPOLL_SIZE, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            // 返回状态是监听套接字
            if (events[i].data.fd == listener) {
                struct sockaddr_in client;
                int new_fd;
                struct logRequest request;          // 用户名字，性别
                struct logResponse response;        // 服务端的回应
                socklen_t len = sizeof(client);
                if ((new_fd = accept(listener, (struct sockaddr *)&client, &len)) < 0) {
                    perror("accept");
                    exit(1);
                }

                // 此处有BUG：程序走到这里若用户没有输入，则会一直阻塞
                recv(new_fd, (char *)&request, sizeof(request), 0);     // 先收用户名字和性别
                
                // 这里应该由从反应堆去做
                // 对用户的操作需要考虑安全性
                struct User *tmp;
                // 判断性别
                if (request.sex) {
                    tmp = users_boy;
                    tmp[new_fd].epollfd = epollfd_boy;                  // 记录用户在哪个反应堆
                } else {
                    tmp = users_girl;
                    tmp[new_fd].epollfd = epollfd_girl;
                }
                strcpy(tmp[new_fd].name, request.name);                     // 拷贝用户名字
                strcpy(tmp[new_fd].ip, inet_ntoa(client.sin_addr));         // 拷贝用户ip
                tmp[new_fd].online = 1;                                     // 1 已上线
                tmp[new_fd].fd = new_fd;                                 
                
                // 省略的操作：验证用户信息，验证会员状态等等操作
                // 填充，保留用户信息
                response.flag = 1;                                          // 用户正确连接
                strcpy(response.msg, "OK!");                                // 给用户发一个ok
                printf("Msg from %s\n", tmp[new_fd].name);                  // 打印用户名
                send(new_fd, (char *)&response, sizeof(response), 0);       // 发送给用户
                // 假设验证完之后，把用户添加到从反应堆
                add_to_subreactor(&tmp[new_fd]);
            }
        }
    }
    return 0;
}
