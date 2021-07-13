/*************************************************************************
	> File Name: 1.msgq.c
	> Author:Yj_Z 
	> Mail: 
	> Created Time: Thu 29 Apr 2021 04:15:42 PM CST
 ************************************************************************/

#include "head.h" 

struct msgbuff {
    long mtype;
    char mtext[512];
};

void send_msg(int qid, int type, char *m_msg, int size) {
    struct msgbuff msg;
    msg.mtype = type;
    strcpy(msg.mtext, m_msg);
    if (msgsnd(qid, (void *)&msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
        perror("msgsnd");
        exit(1);
    }
}

void get_msg(int qid, int type) {
    while (1) {
        struct msgbuff msg;
        bzero(&msg, sizeof(msg));
        if (msgrcv(qid, (void *)&msg, sizeof(msg.mtext), type, MSG_NOERROR) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("抢到了资源: <%d> <%s>\n", type, msg.mtext);
    }
}

int main(int argc, char **argv) {
    int opt, mode = 2, msgtype = 1;
    int msgq_id;
    char mtext[512] = {0};
    while ((opt = getopt(argc, argv, "st:rm:")) != -1) {
        switch (opt) {
            case 's':
                mode = 1;
                break;
            case 'r':
                mode = 2;
                break;
            case 't':
                msgtype = atoi(optarg);
                break;
            case 'm':
                strcpy(mtext, optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -[s|r] -t type -m msg\n", argv[0]);
                exit(1);
        }
    }
    if ((msgq_id = msgget(2021, IPC_CREAT | 0666)) < 0) {
        perror("msgget");
        exit(1);
    }
    if (mode == 1) {
        send_msg(msgq_id, msgtype, mtext, sizeof(mtext));
    } else {
        get_msg(msgq_id, msgtype);
    }
    return 0;
}

