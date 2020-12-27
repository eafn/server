//
// Created by yifan on 2020/12/25.
//

#ifndef SERVER_DAEMON_H
#define SERVER_DAEMON_H
#include "config.h"

class Daemon {
private:
    Daemon();
    ~Daemon();
public:
    static int daemonize();
    static int checkExit(char * filename);
};


#endif //SERVER_DAEMON_H
