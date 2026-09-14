#include "../inc/Scheduler.hpp"
#include "../lib/console.h"
#include "../inc/TCB.hpp"
#include "../test/printing.hpp"
#include "../inc/RISCV.hpp"
List<TCB> Scheduler::readyThreadQueue;
List<TCB> Scheduler::sleepingThreadQueue;

TCB *Scheduler::get() {
    uint64 sstatus = RISCV::r_sstatus();
    RISCV::mc_sstatus(RISCV::SSTATUS_SIE);
    TCB* t = readyThreadQueue.removeFirst();
    RISCV::w_sstatus(sstatus);
    return t;
}

void Scheduler::put(TCB *tcb) {
    uint64 sstatus = RISCV::r_sstatus();
    RISCV::mc_sstatus(RISCV::SSTATUS_SIE);
    readyThreadQueue.addLast(tcb);
    RISCV::w_sstatus(sstatus);
}

void Scheduler::putToSleep(TCB *tcb) {
    sleepingThreadQueue.addSorted(tcb);
}

void Scheduler::updateSleepingThreads() {
    TCB* tcb = sleepingThreadQueue.peekFirst();
    if (!tcb) return;
    tcb->relativeSleepTime--;

    while (tcb && tcb->relativeSleepTime <= 0) {
        tcb = sleepingThreadQueue.removeFirst();

        tcb->setRelativeSleepTime(0);
        tcb->setBlocked(false);

        put(tcb);

        tcb = sleepingThreadQueue.peekFirst();
    }
}
