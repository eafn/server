//
// Created by yifan on 2020/11/23.
//

#include "Task.h"

void Task::run() {
    execute();
}

HttpResponse::HttpResponse(int connSock) {
    this->connSock = connSock;
    const char *pathHeader = "/home/gyf";
    strcpy(filepath, pathHeader);

}

void HttpResponse::printError(const char *msg) {
    fprintf(stderr, "HttpResponse:%s error", msg);
}

int HttpResponse::parseHttp(char *buf, int size) {
    if (size < 0) return -1;
    int i = 0, j = 0;
    while (i < MAX_METHOD_SIZE - 1 && j < size && buf[j] != ' ' && buf[j] != '\0') {
        method[i++] = buf[j++];
    }
    ++j;
    method[i] = '\0';
    i = 0;
    while (i < MAX_FILENAME_SIZE - 1 && j < size && buf[j] != ' ' && buf[j] != '\0') {
        filename[i++] = buf[j++];
    }
    filename[i] = '\0';
    return 0;
}

void HttpResponse::execute() {
    int size;
    char buf[MAX_BUFFER_SIZE];
    if (connSock < 0) {
        printError("connSock");
        return;
    }
    size = read(connSock, buf, MAX_BUFFER_SIZE - 1);
    if (size < 0) {
        printError("read");
        return;
    }
    parseHttp(buf, size);
    if (0 == strcasecmp(method, "GET")) responseGet();
    else responseError(501);
    close(connSock);
    return;
}

void HttpResponse::responseGet() {
    int i = 0, j, k, ret, fd;
    char parameter[1 << 5];
    struct stat fstat;
    while (filename[i] != '?' && filename[i] != '\0') ++i;
    if ('?' == filename[i]) {
        j = i++;
        k = 0;
        while (filename[i] != '\0') parameter[k++] = filename[i++];
        parameter[k] = '\0';
        filename[j] = '\0';
    }
    if (0 == strcmp(filename, "/")) strcat(filepath, "/index.html");
    else strcat(filepath, filename);
    stat(filepath, &fstat);
    //文件打开失败或该文件是目录
    if (ret < 0 || S_ISDIR(fstat.st_mode)) responseError(404);
        //发送响应头静态文件
    else {
        fd = open(filepath, O_RDONLY);
        responseHeader(200,fstat.st_size);
        sendfile(connSock, fd, 0, fstat.st_size);
    }
    return;
}

void HttpResponse::responseHeader(int status, int size) {
    char buf[MAX_BUFFER_SIZE / 4];
    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"
                 "content-length:%d\r\n\r\n", status, size);
    write(connSock, buf, strlen(buf));
}

void HttpResponse::responseError(int status) {
    char buf[MAX_BUFFER_SIZE];
    char msg[MAX_BUFFER_SIZE / 2];
    sprintf(msg, "<html><title>Http Server Error</title>");
    sprintf(msg, "%s<body>\r\n", msg);
    sprintf(msg, "%s %d\r\n", msg, status);
    sprintf(msg, "%s <p>GET: Error", msg);
    sprintf(msg, "%s<hr><h3>The Tiny Web Server<h3></body>", msg);
    sprintf(buf, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"  //响应头
                 "content-length:%ld\r\n\r\n", status, strlen(msg));
    sprintf(buf, "%s%s", buf, msg);
    write(connSock, buf, strlen(buf));
}

void TaskA::execute() {
    printf("the TaskA is execute\n");
    sleep(1);
}

void TaskB::execute() {
    printf("the TaskB is execute\n");
    sleep(1);
}