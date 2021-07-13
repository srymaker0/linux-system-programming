/*************************************************************************
	> File Name: m_ls.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Mon 12 Apr 2021 10:03:44 PM CST
 ************************************************************************/

#include "./head.h"

void absolute_path(char *ab_path, char *path, char *filename) {
    strcpy(ab_path, path);
    strcat(ab_path, "/");
    strcat(ab_path, filename);
    return ;
}

void mode2str(mode_t smode, char *mode, char *color) {
    int i = 0;
    char *rwx[] = {
        "---", "--x", "-w-",
        "-wx", "r--", "r-x",
        "rw-", "rwx"
    };
    if (S_ISREG(smode)) mode[0] = '-';
    else if (S_ISDIR(smode)) mode[0] = 'd', strcpy(color, "BLUE_HL");
    else if (S_ISBLK(smode)) mode[0] = 'b';
    else if (S_ISCHR(smode)) mode[0] = 'c';
#ifdef S_ISFIFO
    else if (S_ISFIFO(smode)) mode[0] = 'p';
#endif
#ifdef S_ISLNK
    else if (S_ISLNK(smode)) mode[0] = 'l', strcpy(color, "PURPLE_HL");
#endif
#ifdef S_ISSOCK
    else if (S_ISSOCK(smode)) mode[0] = 's';
#endif
    else mode[i++] = '?';
    strcpy(&mode[1], rwx[(smode >> 6) & 7]);
    strcpy(&mode[4], rwx[(smode >> 3) & 7]);
    strcpy(&mode[7], rwx[smode & 7]);
    if (smode & S_ISUID) mode[3] = (smode & S_IXUSR) ? 's' : 'S';
    if (smode & S_ISGID) mode[6] = (smode & S_IXGRP) ? 's' : 'l';
    if (smode & S_ISVTX) mode[9] = (smode & S_IXOTH) ? 's' : 'T';
    if (mode[0] == '-') {
        if (strstr(mode, "x")) strcpy(color, "GREEN_HL");
        if (strstr(mode, "s") || strstr(mode, "S")) strcpy(color, "YELLOW_BG"); 
    }
    return ;
}

void mtim2str(struct timespec *mtim, char *str, size_t max) {
    struct tm *tmp_time = localtime(&mtim->tv_sec);
    strftime(str, max, "%b %e %H:%M", tmp_time);
    return ;
}

void myls(char *path, int a_flag) {
    //call func scandir to sort the directory
    struct dirent **namelist;
    int n, i = -1;
    if ((n = scandir(path, &namelist, NULL, alphasort)) == -1) {
        perror("scandir");
        exit(1);
    }
    while (i < n - 1) {
        i++;
        struct dirent *dent = namelist[i];     // refer to the directory stream 
        if (dent->d_name[0] == '.' && !a_flag) continue;
        struct stat st;
        char ab_path[128];
        /* get the absolute_path */
        absolute_path(ab_path, path, dent->d_name);  
        if (!lstat(ab_path, &st)) {
            char mode[16], mtime[32], uname[16], gname[16], filename[512], color[16] = {0};
            mode2str(st.st_mode, mode, color);     /* st_mode get file's authority and mode*/
            mtim2str(&st.st_mtim, mtime, sizeof(mtime));   /* change the time format  st_mtim -- time of last modification*/
            strcpy(uname, getpwuid(st.st_uid)->pw_name);
            strcpy(gname, getgrgid(st.st_gid)->gr_name);
            printf("%s %lu %s %s %*lu %s ", mode, st.st_nlink, uname, gname, 5, st.st_size, mtime);
            if (!strcmp(color, "BLUE")) sprintf(filename, BLUE("%s"), dent->d_name);
            else if (!strcmp(color, "BLUE_HL")) sprintf(filename, BLUE_HL("%s"), dent->d_name);
            else if (!strcmp(color, "PURPLE_HL")) sprintf(filename, PURPLE_HL("%s"), dent->d_name);
            else if (!strcmp(color, "GREEN_HL")) sprintf(filename, GREEN_HL("%s"), dent->d_name);
            else if (!strcmp(color, "YELLOW_BG")) sprintf(filename, YELLOW_BG("%s"), dent->d_name);
            else strcpy(filename, dent->d_name); 
            if (mode[0] == 'l') {
                char linkfile[32];
                readlink(ab_path, linkfile, 32);
                strcat(filename, " -> ");
                strcat(filename, linkfile);
            }
            printf("%s\n", filename);
        } else {
            perror("lstat");
            exit(1);
        }
        free(namelist[i]);
    }
    free(namelist);
    return ;
}

int main(int argc, char *argv[]) {
    //NO PATH
    if (argc == 2) {
        if (!strcmp(argv[1], "-al")) myls(".", 1);     //compare the option parameters  
        else if (!strcmp(argv[1], "-l")) myls(".", 0); 
        else {
            fprintf(stderr, "Usage: %s -[a]l [path]\n", argv[0]);
            exit(1);
        }
        return 0;
    }
    int opt, a_flag = 0;
    while ((opt = getopt(argc, argv, "al:")) != -1) {
        switch (opt) {
            case 'a': {
                a_flag = 1;
            } break;
            case 'l': {
                myls(optarg, a_flag);      //the optarg is [al:]'s parameters
            } break;
            default: {
                fprintf(stderr, "Usage: %s -[a]l [path]\n", argv[0]);
                exit(1);
            }
        }
    }
    return 0;
}

