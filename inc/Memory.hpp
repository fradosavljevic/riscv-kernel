#ifndef MEMORY_H
#define MEMORY_H
#include "../lib/hw.h"

//128MB prostora
class Memory {
public:
    static void init() {
        total_memory = reinterpret_cast<uint64>(HEAP_END_ADDR) - reinterpret_cast<uint64>(HEAP_START_ADDR);
        used_memory = 0;
    }

    static uint64 start() {
        return reinterpret_cast<uint64>(HEAP_START_ADDR);
    }

    static uint64 end() {
        return reinterpret_cast<uint64>(HEAP_END_ADDR);
    }

    static uint64 totalSize() {
        return total_memory;
    }

    static uint64 getAvailableMemory() {
        return total_memory - used_memory;
    }

    static void increaseUsage(const uint64 size) {
        used_memory += size;
    }

    static void decreaseUsage(const uint64 size) {
        used_memory -= size;
    }
private:
    static uint64 total_memory;
    static uint64 used_memory;
};

#endif //MEMORY_H
