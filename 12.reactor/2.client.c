/*************************************************************************
	> File Name: 2.client.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Sun 18 Jul 2021 08:30:12 PM CST
 ************************************************************************/

#include "head.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage : %s ip port sex[1 for boy, 0 for girl] name!\n", argv[0]);
        exit(1);
    }
    int sockfd;
    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        exit(1);
    }
    
    struct logRequest request;
    struct logResponse response;
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    request.sex = atoi(argv[3]);
    strcpy(request.name, argv[4]);

    send(sockfd, (void *)&request, sizeof(request), 0);
    recv(sockfd, (void *)&response, sizeof(response), 0);
    
    printf("Recv : %s\n", response.msg);     // 打印从服务端收到的信息

    while(1) {
        char buff[512] = {0};
        scanf("%[^\n]s", buff);
        getchar();
        if (strlen(buff)) {
            send(sockfd, buff, strlen(buff), 0);
        }
    }
    
    return 0;
}
