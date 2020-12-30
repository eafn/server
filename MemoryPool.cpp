//
// Created by yifan on 2020/12/24.
//

#include "MemoryPool.h"
//#define YIFAN_MEMORYPOOL_DEBUG

/*!
 * 构造器
 * @param unitNum 最小分配单元数量
 * @param unitSize 最小分配字大小
 */
MemoryBlock::MemoryBlock(ushort unitNum, ushort unitSize)
        : totalSize(unitNum * unitSize), freeNum(unitNum - 1), firstFreeUnit(1), nextMemoryBlock(NULL) {
    char *pData = Data;
    // 利用每个单元的前两个字节记录下一个单元的编号，将单元连接起来
    for (ushort i = 1; i < unitNum; i++) {
        *(ushort *) pData = i;
        pData += unitSize;
    }
}

/*!
 * 内存块重载new
 * @param unitNum
 * @param unitSize
 * @return
 */
void *MemoryBlock::operator new(size_t, ushort unitNum, ushort unitSize) {
    return ::operator new(sizeof(MemoryBlock) + unitNum * unitSize);
}

/*!
 * 内存块重载delete
 * @param block
 */
void MemoryBlock::operator delete(void *block, size_t) {
    ::operator delete(block);
}

/*!
 * 构造器
 * @param unitSize 最小分配单元大小
 * @param initSize 第一个内存块单元数
 * @param growSize 增长内存块单元数
 * @param alignSize 字节对齐大小
 */
MemoryPool::MemoryPool(ushort unitSize, ushort initSize, ushort growSize, ushort alignSize)
        : nUnitSize(unitSize), nInitSize(initSize), nGrowSize(growSize), alignSize(alignSize), blockList(NULL) {
    pthreadMutex = PTHREAD_MUTEX_INITIALIZER;//初始化锁
    if (unitSize > 4) nUnitSize = (unitSize + (alignSize) - 1) & ~(alignSize - 1);//字节对齐
    else if (unitSize <= 2) nUnitSize = 2;
    else nUnitSize = 4;
    alloc();
    blockList->firstFreeUnit = 0;
    blockList->freeNum = nInitSize;
}

/*!
 * 析构函数
 */
MemoryPool::~MemoryPool() {
    while (NULL != blockList) {
        MemoryBlock *pMemoryBlock = blockList;
        blockList = blockList->nextMemoryBlock;
        delete pMemoryBlock;
    }
}

/*!
 * 分配内存单元
 * @return 内存单元地址
 */
void *MemoryPool::alloc() {
    MemoryBlock *block;

    //如果内存池为空，创建内存块
    if (NULL == blockList) {
        blockList = new(nInitSize, nUnitSize) MemoryBlock(nInitSize, nUnitSize);
        if (NULL == blockList) return NULL;
        else return blockList->Data;
    }

    //若干内存池不为空则，获取链表里第一个内存块(链表头)
    block = blockList;

    //查找有空闲单元的内存块
    while (NULL != block && 0 == block->freeNum) block = block->nextMemoryBlock;

    //如果找到内存块，则直接分配
    if (NULL != block) {
        char *freeUnit = block->Data + block->firstFreeUnit * nUnitSize;
        block->firstFreeUnit = *((ushort *) freeUnit);
        block->freeNum--;
        return (void *) freeUnit;
    } else {
        // nGrowSize为0，不允许内存池增大
        if (0 == nGrowSize) return NULL;
        block = new(nGrowSize, nUnitSize) MemoryBlock(nGrowSize, nUnitSize);
        if (NULL == block) return NULL;
        block->nextMemoryBlock = blockList;
        blockList = block;
        return (void *) block->Data;
    }
}

/*!
 * 分配内存单元(线程安全)
 * @return 内存单元地址
 */
void *MemoryPool::allocByMutex() {
    void *result;
    pthread_mutex_lock(&pthreadMutex);
    result = alloc();
#ifdef YIFAN_MEMORYPOOL_DEBUG
    printMemoryPoolStatus();
#endif
    pthread_mutex_unlock(&pthreadMutex);
    return result;
}

void MemoryPool::freeByMutex(void *curUnit) {
    pthread_mutex_lock(&pthreadMutex);
    free(curUnit);
#ifdef YIFAN_MEMORYPOOL_DEBUG
    printMemoryPoolStatus();
#endif
    pthread_mutex_unlock(&pthreadMutex);
}

/*!
 * 释放内存单元
 * @param curUnit  单元地址
 */
void MemoryPool::free(void *curUnit) {
    //block为当前块，preBlock为前一个块
    MemoryBlock *block = blockList;
    MemoryBlock *preBlock = blockList;
    //查找当前单元所在内存块
    while (block && ((ulong) curUnit < (ulong) block || (ulong) curUnit >= (ulong) (block->Data) + block->totalSize)) {
        preBlock = block;
        block = block->nextMemoryBlock;
    }
    //未找到内存块
    if (NULL == block) return;
    //找到内存块
    block->freeNum++;
    *(ushort *) curUnit = block->firstFreeUnit;
    block->firstFreeUnit = ((ulong) curUnit - (ulong) block->Data) / nUnitSize;
    //如果内存块是空的则删除内存块
    if (block->freeNum * nUnitSize == block->totalSize) {
        if (block == blockList) {
            blockList = blockList->nextMemoryBlock;
            delete block;
        } else {
            preBlock->nextMemoryBlock = block->nextMemoryBlock;
            delete block;
        }
    }
        //如果内存块不在链表头，则放到链表头
    else {
        if (block != blockList) {
            preBlock->nextMemoryBlock = block->nextMemoryBlock;
            block->nextMemoryBlock = blockList;
            blockList = block;
        }
    }
}

/*!
 * Debug,打印内存状态
 */
void MemoryPool::printMemoryPoolStatus() {
    int freeUnitNum = 0;
    int blockNum = 0;
    MemoryBlock *block = blockList;
    while (NULL != block) {
        blockNum++;
        freeUnitNum += block->freeNum;
        block = block->nextMemoryBlock;
    }
    printf("Total Block Number = %d,Free Unit Number= %d\n", blockNum, freeUnitNum);
}