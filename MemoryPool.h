//
// Created by yifan on 2020/12/24.
//

#ifndef SERVER_MEMORYPOOL_H
#define SERVER_MEMORYPOOL_H

#include "config.h"

struct MemoryBlock {
    ushort totalSize;
    ushort firstFreeUnit;
    ushort freeNum;
    MemoryBlock *nextMemoryBlock;
    char Data[1];

    MemoryBlock(ushort unitNum, ushort unitSize);

    void *operator new(size_t, ushort unitNum, ushort unitSize);

    void operator delete(void *block, size_t);

    ~MemoryBlock() {}
};

class MemoryPool {
private:
    ushort nUnitSize;
    ushort nInitSize;
    ushort nGrowSize;
    ushort alignSize;
    pthread_mutex_t pthreadMutex;
    MemoryBlock *blockList;

    void printMemoryPoolStatus();

    void free(void *curUnit);

    void *alloc();

public:
    MemoryPool(ushort unitSize, ushort initSize, ushort growSize, ushort alignSize = 4);

    ~MemoryPool();

    void *allocByMutex();

    void freeByMutex(void *curUnit);
};

#endif //SERVER_MEMORYPOOL_H
