//
// Created by yifan on 2020/11/24.
//

#include "Listener.h"

Listener::Listener() {
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(10010);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSock = -1;
}

void Listener::printError(const char *msg) {
    fprintf(stderr, "Listener::run: %s error,%s\n", msg, strerror(errno));
}

int Listener::getServSock() {
    return servSock;
}

int Listener::run() {
    int on = 1;
    if ((servSock = socket(servAddr.sin_family, SOCK_STREAM, 0)) < 0) {
        printError("socket");
        return -1;
    }
    printf("sock:%d\n",servSock);
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if ((bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr))) < 0) {
        printError("bind");
        return -1;
    }
    if (listen(servSock, 1 << 6) < 0) {
        printError("listen");
        return -1;
    }
    return servSock;
}
