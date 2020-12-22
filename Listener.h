//
// Created by yifan on 2020/11/24.
//

#ifndef SERVER_LISTENER_H
#define SERVER_LISTENER_H

#include "config.h"

class Listener {
private:
    int servSock;
    int port;
    struct sockaddr_in servAddr;

public:
    Listener();

    int getServSock();

    static void printError(const char *msg);

    int run();
};


#endif //SERVER_LISTENER_H
