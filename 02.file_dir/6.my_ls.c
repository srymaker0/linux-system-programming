/*************************************************************************
	> File Name: 6.my_ls.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Mon 29 Mar 2021 04:05:12 PM CST
 ************************************************************************/

#include "./common/head.h"

int main(int argc, char **argv) {
    int opt;
    int l_flag = 0, a_flag = 0;
    char dir_name[256] = {0};
    DIR *dir = NULL;
    struct stat statbuf[256] = {0};
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
            case 'a' : {
                a_flag = 1;
            } break;
            case 'l' : {
                l_flag = 1;
            } break;
            default : {
                fprintf(stderr, "Usage : %s -al\n", argv[0]);
                exit(1);
            }
        }
    }
    
    if (a_flag && l_flag) {
        strcpy(dir_name, ".");
    } else {
        fprintf(stderr, "Usage : %s -al\n", argv[0]);
        exit(1);
    }

    if ((dir = opendir(dir_name)) == NULL) {
        perror(dir_name);
        exit(1);
    }
    while (1) {
        struct dirent *dir_ptr;
        if ((dir_ptr = readdir(dir)) == NULL) {
            break;
        }
        if(a_flag == 0) {
            
        } 
        printf("%s\n", dir_ptr->d_name);
    }
    closedir(dir);
    return 0;
}
