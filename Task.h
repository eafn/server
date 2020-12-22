//
// Created by yifan on 2020/11/23.
//

#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "config.h"

const int MAX_METHOD_SIZE = 1 << 3;
const int MAX_FILENAME_SIZE = 1 << 6;
const int MAX_FILEPATH_SIZE = 1 << 7;
const int MAX_BUFFER_SIZE = 1 << 10;
//const char * path = "/home/gyf";

class Task {
public:
    void run();

    virtual void execute() = 0;

};


class HttpResponse : public Task {
private:
    int connSock = -1;
    char method[MAX_METHOD_SIZE];
    char filename[MAX_FILENAME_SIZE];
    char filepath[MAX_FILEPATH_SIZE];
public:

    HttpResponse(int connSock);

    void responseGet();

    void responseError(int status);

    void responseHeader(int status, int size);

    static void printError(const char *msg);

    int parseHttp(char *buf, int size);

    virtual void execute();
};

class TaskA : public Task {
public:
    virtual void execute();
};


class TaskB : public Task {
public:
    virtual void execute();
};

#endif //SERVER_TASK_H
