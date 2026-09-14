#ifndef SCB_HPP
#define SCB_HPP
#include "../inc/list.hpp"
#include "../inc/TCB.hpp"

class SCB {
public:
    ~SCB() = default;
    SCB(int value);

    static SCB* createSemaphore(int);
    static int signalHandle(SCB*, int = 1);
    static int waitHandle(SCB*, int = 1);

    int getValue() const;

    int close();

    int wait(int);
    int signal(int);

    void block();
    TCB* deblock();

    void* operator new(uint64);
    void* operator new[](uint64);

    void operator delete(void*);
    void operator delete[](void*);
private:
    int value;
    bool closed;
    List<TCB> blockedThreads;
    List<SCB> pairedWith;
};

#endif
