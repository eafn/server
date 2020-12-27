#include "config.h"
#include "PthreadPool.h"
#include "Listener.h"
#include "Task.h"
#include "Daemon.h"

int running = 1;

int main() {
    if (-1 == Daemon::daemonize()) {
        perror("main: run demonize error\n");
        return -1;
    }

    openlog(YIFAN_PROGRAM_NAME, LOG_PID, 0);    //打开syslog

    Task *task;
    HttpResponse *httpResponse;
    int lisSock, connSock;
    socklen_t cliAddrLen;
    struct sockaddr_in cliAddr;
    MemoryPool *memoryPool = NULL;

    signal(SIGPIPE, SIG_IGN);
    cliAddrLen = sizeof(cliAddr);
    running = 1;

    syslog(LOG_INFO, "server start");

    memoryPool = new MemoryPool(sizeof(HttpResponse), 16, 4);//启动内存池
    if (NULL == memoryPool) {
        syslog(LOG_ERR, "main: run MemoryPool error\n");
        syslog(LOG_INFO, "server terminate");
        return -1;
    }


    Listener listener;//开启监听器
    if (-1 == (lisSock = listener.run())) {
        syslog(LOG_ERR, "main: run Listener error\n");
        syslog(LOG_INFO, "server terminate");
        return -1;
    }


    PthreadPool pthreadPool(5);//开启线程池
    if (-1 == pthreadPool.runPthread()) {
        syslog(LOG_ERR, "main: run pthreadpool error\n");
        syslog(LOG_INFO, "server terminate");
        return -1;
    }
    pthreadPool.setMemoryPool(memoryPool);


    while (1 == running) {
        connSock = accept(lisSock, (struct sockaddr *) &cliAddr, &cliAddrLen);
        syslog(LOG_INFO, "http request from %s", inet_ntoa(cliAddr.sin_addr));
#ifdef MEMORY_POOL_MODE
        httpResponse = (HttpResponse *) memoryPool->allocByMutex();
        new(httpResponse) HttpResponse(connSock);
        pthreadPool.addTask(httpResponse);
#else
        task = new HttpResponse(connSock);
        pthreadPool.addTask(task);
#endif
    }
    return 0;
}

