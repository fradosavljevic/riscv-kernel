#ifndef TCB_H
#define TCB_H
#include "MemoryAllocator.hpp"
#include "Scheduler.hpp"
#include "../lib/hw.h"

// Thread control block
class TCB {
public:
    ~TCB() { MemoryAllocator::freeWrapper(stack); }

    bool isFinished() const { return finished; }
    void setFinished(bool value) { finished = value; }

    bool isBlocked() const { return blocked; }
    void setBlocked(bool value) { blocked = value; }

    void setSleepTime(time_t time) { sleepTime = time; }
    time_t getSleepTime() const { return sleepTime; }

    void setRelativeSleepTime(time_t time) { relativeSleepTime = time; }
    time_t getRelativeSleepTime() { return relativeSleepTime; }

    enum privilegeLevel {USER, SUPERVISOR};
    privilegeLevel getPrivilegeLevel() const { return level; }

    using Body = void (*)(void*);

    uint64 getTimeSlice() const { return timeSlice; }

    static TCB *createThread(Body body, void *arg, privilegeLevel PRIVILEGE);
    static void exitThread();

    static void yield();
    static TCB *running;

    void* operator new(uint64 size);
    void* operator new[](uint64 size);

    void operator delete(void *ptr);
    void operator delete[](void *ptr);
private:
    explicit TCB(Body body, void *argument, uint64 timeSlice, uint64 *stack, privilegeLevel PRIVILEGE) :
            body(body), stack(stack),
            context({reinterpret_cast<uint64>(&threadWrapper), reinterpret_cast<uint64>(stack + STACK_SIZE)}),
            finished(false),
            timeSlice(timeSlice),
            argument(argument), blocked(false), level(PRIVILEGE)
    {};

    struct Context { uint64 ra; uint64 sp;};
    Body body; uint64 *stack; Context context; bool finished;
    uint64 timeSlice; void* argument;
    bool blocked;

    time_t sleepTime;
    time_t relativeSleepTime;

    int neededSemUnits = 0;

    privilegeLevel level;

    static void contextSwitch(Context *oldContext, Context *runningContext);
    static void dispatch();
    static void threadWrapper();
    static int threadSleep(time_t);

    friend class RISCV;
    friend class SCB;
    friend class Scheduler;

    static uint64 timeSliceCounter;
    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE;
    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;
};

#endif //TCB_H
