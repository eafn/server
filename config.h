//
// Created by yifan on 2020/11/23.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H
// 头文件
#include <cstdio>
#include <cstring>
#include <queue>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/file.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <list>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>
#include <arpa/inet.h>
//宏定义
#define YIFAN_PORT (10010)                  /*端口号*/
#define MEMORY_POOL_MODE                    /*开启内存池模式*/
#define YIFAN_DEBUG_ENVIRONMENT             /*开启DEBUG模式*/
#define YIFAN_PATH_HEADER ("/home/gyf")     /*资源目录*/
#define YIFAN_WORK_PATH ("/")               /*工作目录*/
#define YIFAN_PROGRAM_NAME ("yifan-server") /*程序名称*/
//命名
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;
using namespace std;
#endif //SERVER_CONFIG_H
