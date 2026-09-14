#include "../inc/Console.hpp"
#include "../inc/Memory.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/RISCV.hpp"
#include "../inc/syscall_c.hpp"
#include "../test/printing.hpp"

extern void userMain();

void userMainWrapper(void*) {
    userMain();
}

int main() {
    Memory::init();
    MemoryAllocator::init();
    myConsole::init();

    TCB* idleThread = TCB::createThread(nullptr, nullptr, TCB::SUPERVISOR);
    TCB::running = idleThread;
    TCB* userThread = TCB::createThread(userMainWrapper, nullptr, TCB::USER);
    TCB* consoleThread = TCB::createThread(myConsole::putCharToBuffer, nullptr, TCB::SUPERVISOR);

    RISCV::w_stvec(reinterpret_cast<uint64>(&RISCV::supervisorTrap));
    RISCV::ms_sstatus(RISCV::SSTATUS_SIE);

    while (!userThread->isFinished() || !myConsole::outputBuffer->empty()) {
        thread_dispatch();
    }

    delete idleThread;
    delete userThread;
    delete consoleThread;
    TCB::running = nullptr;

    RISCV::shutdownEmulator();
    return 0;
}