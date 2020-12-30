//
// Created by yifan on 2020/12/25.
//

#include "Daemon.h"

/*!
 * 服务器后台化
 * @return 成功 0
 */
int Daemon::daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork-1");
        return -1;
    } else if (pid > 0) {
        usleep(1000);
        exit(0);
    }

    /*子进程*/
    pid_t sid = setsid();   //脱离终端
    if (sid < 0) return -1;

    umask(0);

    pid = fork();           //保证不会再打开终端
    if (pid < 0) {
        perror("fork-2");
        return -1;
    }
    else if (pid > 0) {
        usleep(1000);
        exit(0);
    }

    /*孙子进程*/
    if (chdir(YIFAN_WORK_PATH) < 0) {
            perror("chdir error");
            return -1;
    }

    for (int i = 0; i < NOFILE; ++i) {
        close(i);
    }

    int stdfd = open("/dev/null", O_RDWR);
    dup2(stdfd, STDOUT_FILENO);
    dup2(stdfd, STDERR_FILENO);

    return 0;
}


//不用检查服务器是否运行，因为端口号是唯一绑定的
/*
int Daemon::checkExit(char *filename) {
    int lockfd;

    if (NULL == filename) {
        perror("filename error");
        return -1;
    }

    if (-1 == (lockfd = open(filename, O_RDWR|O_CREAT))) {
        perror("file open error");
        return -1;
    }

    if (-1 == flock(lockfd,LOCK_EX|LOCK_NB)){
        perror("process has exited");
        return -1;
    }
    return 0;
}*/
