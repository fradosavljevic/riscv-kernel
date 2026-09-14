#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include "../inc/list.hpp"

class TCB;

class Scheduler {
public:
    static List<TCB> readyThreadQueue;
    static List<TCB> sleepingThreadQueue;
    static TCB* get();
    static void put(TCB* tcb);
    static void putToSleep(TCB* tcb);
    static void updateSleepingThreads();
};

#endif
