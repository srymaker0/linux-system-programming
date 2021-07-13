/*************************************************************************
	> File Name: 3.sever.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Wed 26 May 2021 09:41:52 AM CST
 ************************************************************************/

#include "head.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage %s -p port!\n", argv[0]);
    }
    int opt, port, sockfd;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p': 
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage %s -p port!\n", argv[0]);
                break;
        }
    }
    
    return 0;
}
