#include "../inc/SCB.hpp"

#include "../inc/RISCV.hpp"
#include "../lib/console.h"

SCB::SCB(int value) : value(value), closed(false) {}

SCB *SCB::createSemaphore(int value) {
    return new SCB(value);
}

int SCB::signal(int n) {
    if (closed) return -1;
    value += n;
    while (blockedThreads.peekFirst()) {
        TCB* t = blockedThreads.peekFirst();
        if (t->neededSemUnits <= value) {
            value -= t->neededSemUnits;
            deblock();
        } else break;
    }
    return 0;
}

int SCB::wait(int n) {
    if (closed) return -1;
    if (value >= n) value -= n;
    else {
        TCB::running->neededSemUnits = n;
        block();
        if (closed) return -1;
    }
    return 0;
}

void SCB::block() {
    TCB::running->setBlocked(true);
    blockedThreads.addLast(TCB::running);
    TCB::dispatch();
}

TCB *SCB::deblock() {
    TCB* t = blockedThreads.removeFirst();
    if (!t) return nullptr;
    t->setBlocked(false);
    t->neededSemUnits = 0;
    Scheduler::put(t);
    return t;
}

int SCB::close() {
    if (closed) return -1;
    closed = true;
    while (blockedThreads.peekFirst() != nullptr) {
        TCB* t = blockedThreads.removeFirst();
        t->setBlocked(false);
        t->neededSemUnits = 0;
        Scheduler::put(t);
    }
    return 0;
}

int SCB::signalHandle(SCB* semHandle, int n) {
    if (semHandle == nullptr) return -1;
    int result = semHandle->signal(n);
    return result;
}

int SCB::waitHandle(SCB* semHandle, int n) {
    if (semHandle == nullptr) return -1;
    int result = semHandle->wait(n);
    return result;
}

int SCB::getValue() const {
    return value;
}

void* SCB::operator new(uint64 size) {
    return MemoryAllocator::mallocWrapper((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
}
void* SCB::operator new[](uint64 size) {
    return MemoryAllocator::mallocWrapper((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
}

void SCB::operator delete(void *ptr) {
    MemoryAllocator::freeWrapper(ptr);
}
void SCB::operator delete[](void *ptr) {
    MemoryAllocator::freeWrapper(ptr);
}