#include "../inc/TCB.hpp"
#include "../test/printing.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/Scheduler.hpp"
#include "../inc/RISCV.hpp"
#include "../lib/console.h"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

void* TCB::operator new(uint64 size) {
    return MemoryAllocator::mallocWrapper((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
}
void* TCB::operator new[](uint64 size) {
    return MemoryAllocator::mallocWrapper((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
}

void TCB::operator delete(void *ptr) {
    MemoryAllocator::freeWrapper(ptr);
}
void TCB::operator delete[](void *ptr) {
    MemoryAllocator::freeWrapper(ptr);
}

TCB *TCB::createThread(Body body, void *arg, privilegeLevel PRIVILEGE) {
    uint64* stack = static_cast<uint64 *>(MemoryAllocator::mallocWrapper((STACK_SIZE * sizeof(uint64) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE));
    TCB* newThread = new TCB(body, arg, TIME_SLICE, stack, PRIVILEGE);
    if (!newThread) return nullptr;
    Scheduler::put(newThread);
    return newThread;
}

void TCB::exitThread() {
    running->setFinished(true);
    dispatch();
}

void TCB::yield() {
    asm volatile ("li a0, 0x13");
    asm volatile ("ecall");
}

void TCB::dispatch() {
    TCB *old = running;
    timeSliceCounter = 0;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();
    contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RISCV::popSppSpie();
    running->body(running->argument);
    thread_exit();
}

int TCB::threadSleep(time_t t) {
    if (t <= 0) return -1;
    running->setBlocked(true);
    running->setSleepTime(t);
    Scheduler::putToSleep(running);
    dispatch();
    return 0;
}
