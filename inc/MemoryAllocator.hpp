#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include "../lib/hw.h"
#define MEM_CHECK_CODE 0x45544600

class MemoryAllocator {
public:
    static void init();
    static void* mallocWrapper(uint64 blocks);
    static int freeWrapper(const void* memory);
private:
    static void* malloc(uint64 blocks);
    static int free(const void* memory);

    struct alignas(16) MemBlock {
        uint64 size;
        MemBlock* next, *prev;
        uint32 checkCode;
        bool isFree;
    };

    static MemBlock* memHead;
    static uint initialSize;
};

#endif
