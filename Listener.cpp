//
// Created by yifan on 2020/11/24.
//

#include "Listener.h"

/*!
 * 构造器
 */
Listener::  Listener() {
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(YIFAN_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servSock = -1;
}

/*!
 * 打印错误信息
 * @param msg
 */
void Listener::printError(const char *msg) {
      syslog(LOG_ERR,"Listener:: %s error,%s\n",msg,strerror(errno));
      syslog(LOG_INFO,"server terminate");
}

/*!
 * 获取监听套接字
 * @return  监听套接字
 */
int Listener::getServSock() {
    return servSock;
}

/*!
 * 启动监听器
 * @return 返回监听套接字
 */
int Listener::run() {
    int on = 1;
    if ((servSock = socket(servAddr.sin_family, SOCK_STREAM, 0)) < 0) {
        printError("run:socket");
        return -1;
    }
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if ((bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr))) < 0) {
        printError("run:bind");
        return -1;
    }
    if (listen(servSock, 1 << 6) < 0) {
        printError("run:listen");
        return -1;
    }
    return servSock;
}
