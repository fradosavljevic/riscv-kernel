#include "../inc/RISCV.hpp"
#include "../inc/Console.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/TCB.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"

struct TrapFrame {
    uint64 GPR[32]; //GPR[10] = a0
};

void RISCV::popSppSpie() {
    TCB::running->getPrivilegeLevel() == TCB::SUPERVISOR ? ms_sstatus(SSTATUS_SPP) : mc_sstatus(SSTATUS_SPP);
    asm volatile ("csrw sepc, ra");
    asm volatile ("sret");
}

void RISCV::kernelMessage(const char* msg) {
    auto status = reinterpret_cast<volatile char *>(CONSOLE_STATUS);
    auto txData = reinterpret_cast<volatile char *>(CONSOLE_TX_DATA);
    while (*msg) {
        while (!(*status & CONSOLE_TX_STATUS_BIT)) {}
        *txData = static_cast<uint64>(*msg);
        msg++;
    }
}

// Arguments must be loaded from the trap frame, not the actual registers
void RISCV::handleSupervisorTrap() {
    uint64* fp; asm volatile ("mv %0, s0" : "=r"(fp));
    uint64 scause = r_scause();
    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) { //nije interrupt, env call from S(9) mode or U(8) mode
        auto TF = reinterpret_cast<TrapFrame *>(fp);

        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        switch (TF->GPR[A0]) {
            case 0x1: { //malloc
                uint64 blocks = TF->GPR[A1];
                void* addr = MemoryAllocator::mallocWrapper(blocks);
                TF->GPR[A0] = reinterpret_cast<uint64>(addr);
                break;
            }
            case 0x2: { //free
                auto mem = reinterpret_cast<void *>(TF->GPR[A1]);
                TF->GPR[A0] = MemoryAllocator::freeWrapper(mem);
                break;
            }
            case 0x11: { //thread create
                auto handle = reinterpret_cast<TCB **>(TF->GPR[A1]);
                auto start_routine = reinterpret_cast<TCB::Body>(TF->GPR[A2]);
                auto arg = reinterpret_cast<void *>(TF->GPR[A3]);
                TCB* newThread = TCB::createThread(start_routine, arg, TCB::USER);

                uint64 status = newThread != nullptr ? 0 : -1;
                if (newThread != nullptr) *handle = newThread;

                TF->GPR[A0] = status;
                break;
            }
            case 0x12:
                TCB::exitThread();
                TF->GPR[A0] = 0;
                break;
            case 0x13:
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
                break;
            case 0x21: {
                auto handle = reinterpret_cast<SCB **>(TF->GPR[A1]);
                unsigned initial = TF->GPR[A2];
                SCB* newSem = SCB::createSemaphore(initial);

                uint64 status = newSem != nullptr ? 0 : -1;
                if (newSem != nullptr) *handle = newSem;

                TF->GPR[A0] = status;
                break;
            }
            case 0x22: { //sem close
                auto handle = reinterpret_cast<SCB *>(TF->GPR[A1]);
                int status = -1;

                if (handle != nullptr) status = handle->close();

                TF->GPR[A0] = status;
                break;
            }
            case 0x23: { //wait
                auto handle = reinterpret_cast<SCB *>(TF->GPR[A1]);
                TF->GPR[A0] = SCB::waitHandle(handle);
                break;
            }
            case 0x24: { //signal
                auto handle = reinterpret_cast<SCB *>(TF->GPR[A1]);
                TF->GPR[A0] = SCB::signalHandle(handle);
                break;
            }
            case 0x25: { //sem wait n
                auto handle = reinterpret_cast<SCB *>(TF->GPR[A1]);
                unsigned n = TF->GPR[A2];
                TF->GPR[A0] = SCB::waitHandle(handle, n);
                break;
            }
            case 0x26: { //sem signal n
                auto handle = reinterpret_cast<SCB *>(TF->GPR[A1]);
                unsigned n = TF->GPR[A2];
                TF->GPR[A0] = SCB::signalHandle(handle, n);
                break;
            }
            case 0x31: { //sleep
                time_t t = TF->GPR[A1];
                TF->GPR[A0] = TCB::threadSleep(t);
                break;
            }
            case 0x41: { //getc
                char c = myConsole::getc();
                TF->GPR[A0] = c;
                break;
            }
            case 0x42: { //putc
                char c = static_cast<char>(TF->GPR[A1]);
                myConsole::putc(c);
                break;
            }
            default: break;
        }
        w_sstatus(sstatus); w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL) { handleTimerInterrupt(); }
    else if (scause == 0x8000000000000009UL) { handleConsoleInterrupt(); }
    else if (scause == 0x0000000000000002UL) {
        kernelMessage("Illegal instruction!\n");
        shutdownEmulator();
    }
    else { //unexpected trap cause
        printInt(r_scause());
        shutdownEmulator();
    }
}

void RISCV::handleTimerInterrupt() {
    mc_sip(SIP_SSIP);
    TCB::timeSliceCounter++;
    Scheduler::updateSleepingThreads();
    if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
        uint64 sepc = r_sepc(); uint64 sstatus = r_sstatus();
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
        w_sstatus(sstatus); w_sepc(sepc);
    }
}

void RISCV::handleConsoleInterrupt() {
    const int n = plic_claim();
    if (n == CONSOLE_IRQ) {
        myConsole::getCharFromBuffer();
    }
    plic_complete(n);
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    w_sstatus(sstatus);
    w_sepc(sepc);
}

void RISCV::shutdownEmulator() {
    asm volatile ("li t6, 0x100000");
    asm volatile ("li t5, 0x5555");
    asm volatile ("sw t5, 0(t6)");
}