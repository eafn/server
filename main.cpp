#include "config.h"
#include "PthreadPool.h"
#include "Listener.h"
#include "Task.h"

int running = 1;

void signal_handler(int signalNum) {
    running = 0;
}


int main() {
    Task *task;
    int lisSock, connSock;
    socklen_t cliAddrLen;
    struct sockaddr_in cliAddr;

    cliAddrLen = sizeof(cliAddr);
    running = 1;

    signal(SIGPIPE,SIG_IGN) ;

    Listener listener;
    if (-1 == (lisSock = listener.run())) {
        fprintf(stderr, "main: run listener error\n");
        return -1;
    }

    PthreadPool pthreadPool(20);
    if (-1 == pthreadPool.runPthread()) {
        fprintf(stderr, "main: run pthreadpool error\n");
        return -1;
    }

    while (running && 1) {
        connSock = accept(lisSock, (struct sockaddr *) &cliAddr, &cliAddrLen);
        task = new HttpResponse(connSock);
        pthreadPool.addTask(task);
    }
    return 0;
}
