/*************************************************************************
	> File Name: 2.File.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Mon 22 Mar 2021 03:52:07 PM CST
 ************************************************************************/

#include "./common/head.h"

int main(int argc, char **argv) {
    int opt;
    char rfile[50] = {0}, wfile[50] = {0};
    FILE *r, *w;
    while ((opt = getopt(argc, argv, "r:w:")) != -1) {
        switch (opt) {
            case 'r': {
                strcpy(rfile, optarg);
            } break;
            case 'w': {
                strcpy(wfile, optarg);
            } break;
            default: {
                fprintf(stderr, "Usage : %s -r rfile -w wfile\n", argv[0]);
                exit(1);
            }
        }
    }
    
    if ((strlen(rfile) == 0) || (strlen(wfile) == 0)) {
        fprintf(stderr, "Usage : %s -r rfile -w wfile\n", argv[0]);
        exit(1);
    }
    if ((r = fopen(rfile, "r")) == NULL) {
        perror(rfile);
        exit(1);
    }
    if ((w = fopen(wfile, "w")) == NULL) {
        perror(wfile);
        exit(1);
    }
    while (1) {
        char buff[512] = {0};
        size_t size = fread(buff, 1, sizeof(buff), r);
        if (size <= 0) {
            break;
        }
        fwrite(buff, 1, size, w);
    }
    fclose(r);
    fclose(w);
    return 0;
}
