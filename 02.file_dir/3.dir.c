/*************************************************************************
	> File Name: 3.dir.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Mon 22 Mar 2021 07:24:44 PM CST
 ************************************************************************/

#include "./common/head.h"

int main(int argc, char **argv) {
    char dir_name[256] = {0};
    DIR *dir = NULL;
    if (argc == 1) {
        strcpy(dir_name, "/home/zyj/LINUX/file_dir");
    } else {
        strcpy(dir_name, argv[1]);
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
        printf("%s\n", dir_ptr->d_name);
    }
    closedir(dir);
    return 0;
}

