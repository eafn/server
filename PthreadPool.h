//
// Created by yifan on 2020/11/23.
//

#ifndef SERVER_PTHREADPOOL_H
#define SERVER_PTHREADPOOL_H


#include "config.h"
#include "Task.h"
#include "MemoryPool.h"


using namespace std;
const int PTHREAD_POOL_SIZE = 5;
const char *const PTHREAD_CREATE_ERROR = "pthread create error";
const char *const PTHREAD_JOIN_ERROR = "pthread join error";

class PthreadPool {
private:
    pthread_mutex_t pthreadMutex;
    pthread_cond_t pthreadCond;
    list<pthread_t> pthreadList;
    queue<Task *> taskQueue;
    MemoryPool *memoryPool;
    int pthreadNum;
    int running;

    static void *pthreadPerform(void *arg);

    void printError(const char *msg, int errorNum);

    int lockMutex();

    int unlockMutex();

    int waitCond();

public:
    PthreadPool(size_t poolSize = PTHREAD_POOL_SIZE);

    ~ PthreadPool();

    void setPthreadNum(int pthreadNum);

    int getPthreadNum();

    void addTask(Task *task);

    int getTaskNum();

    int runPthread();

    int closePthread();

    void freeTask(Task *task);

    void setMemoryPool(MemoryPool *memoryPool);

};


#endif //SERVER_PTHREADPOOL_H
