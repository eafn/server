//
// Created by yifan on 2020/11/23.
//

#include "PthreadPool.h"

#define YIFAN_PTHREADPOOL_DEBUG

PthreadPool::PthreadPool(size_t poolSize) {
    pthreadNum = poolSize;
    pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
    pthreadCond = PTHREAD_COND_INITIALIZER;
    running = 0;
}

void PthreadPool::printError(const char *msg, int errorNum) {
    fprintf(stderr, "%s:%s", msg, strerror(errorNum));
}

int PthreadPool::lockMutex() {
    int err = pthread_mutex_lock(&pthreadMutex);
    if (err) printError("PthreadPool::lockMutex", err);
    return err;
}

int PthreadPool::unlockMutex() {
    int err = pthread_mutex_unlock(&pthreadMutex);
    if (err) printError("PthreadPool::unlockMutex", err);
    return err;
}

int PthreadPool::waitCond() {
    int err = pthread_cond_wait(&pthreadCond, &pthreadMutex);
    if (err) printError("PthreadPool::waitCond", err);
    return err;
}


/*
 * //DEBUG时使用
 * void PthreadPool::setPthreadNum(int pthreadNum) {
    if (running) fprintf(stderr, "PthreadPoll is running");
    else if (pthreadNum <= 0) fprintf(stderr, "PthreadNum must be more than 0");
    else this->pthreadNum = pthreadNum;
}

int PthreadPool::getPthreadNum() {
    return pthreadNum;
}

int PthreadPool::getTaskNum() {
    return taskQueue.size();
}
 */

void PthreadPool::addTask(Task *task) {
    lockMutex();
    taskQueue.push(task);
    unlockMutex();
    pthread_cond_signal(&pthreadCond);
}

int PthreadPool::runPthread() {
    if (running) return -1;
    printf("running = 0\n");
    int i, err;
    pthread_t tid;
    running = 1;
    for (i = 0; i < pthreadNum; i++) {
        err = pthread_create(&tid, NULL, pthreadPerform, this);
        if (!err) pthreadList.push_back(tid);
        else {
            printError("PthreadPool::runPthread", err);
            while (i--) {
                pthread_join(pthreadList.front(), NULL);
                pthreadList.pop_front();
            }
            break;
        }
    }
    if (-1 == i) running = 0;
    return err;
}

int PthreadPool::closePthread() {
    if (running) running = 0;
    else return -1;
    int err = 0;
    pthread_cond_broadcast(&pthreadCond);
    auto iter = pthreadList.begin();
    while (iter != pthreadList.end()) {
        if (err = pthread_join(*iter, NULL)) printError("PthreadPool::closePthread", err);
        pthreadList.erase(iter);
        iter++;
    }
    pthread_mutex_destroy(&pthreadMutex);
    pthread_cond_destroy(&pthreadCond);
    return err;
}


void *PthreadPool::pthreadPerform(void *arg) {
    PthreadPool *pool = (PthreadPool *) arg;    //静态函数通过参数获取线程池实例
    pthread_t tid = pthread_self();
    Task *task = NULL;
#ifdef YIFAN_PTHREADPOOL_DEBUG
    printf("pthread[tid==%ld] is running\n", tid);
#endif
    while (1) {
        pool->lockMutex();
        while (pool->running && pool->taskQueue.empty()) pool->waitCond();
        if (!pool->running) {
            pool->pthreadNum--;
            pool->unlockMutex();
            break;
        }
        if (pool->taskQueue.empty()) {
            pool->unlockMutex();
            continue;
        }
        task = pool->taskQueue.front();
        pool->taskQueue.pop();
        pool->unlockMutex();
        if (task) {
            task->run();
            pool->freeTask(task);
#ifdef YIFAN_PTHREADPOOL_DEBUG
            printf("pthread[tid==%ld] is performing\n", tid);
#endif
        }
    }
    return (void *) 0;
}

void PthreadPool::setMemoryPool(MemoryPool *memoryPool) {
    this->memoryPool = memoryPool;
}

void PthreadPool::freeTask(Task *task) {
#ifdef MEMORY_POOL_MODE
    memoryPool->freeByMutex(task);
#else
    delete task;
#endif
}

PthreadPool::~PthreadPool() {
    closePthread();
}

