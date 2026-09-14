#ifndef RISCV_HPP
#define RISCV_HPP
#include "../lib/hw.h"

enum {
    A0 = 10,
    A1 = 11,
    A2 = 12,
    A3 = 13
};

class RISCV {
public:
    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();

    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = 1 << 1,
        SIP_STIP = 1 << 5,
        SIP_SEIP = 1 << 9,
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus
    {
        SSTATUS_SIE = 1 << 1,
        SSTATUS_SPIE = 1 << 5,
        SSTATUS_SPP = 1 << 8,
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // supervisor trap
    static void supervisorTrap();

    //stvec Vector "table"
    static void stvecVectorTable();

    //read a0 register
    static uint64 r_a0();

    static uint64 r_sscratch();

    //write to a0 register
    static void w_a0(uint64 writeValue);

    static void pushRegisters();

    static void popRegisters();

    static void kernelMessage(const char*);

    static void shutdownEmulator();
private:
    //supervisor trap handler
    static void handleSupervisorTrap();

    static void handleTimerInterrupt();

    static void handleConsoleInterrupt();
};

inline uint64 RISCV::r_scause() {
    uint64 volatile scause;
    asm volatile ("csrr %[scause], scause" : [scause] "=r" (scause));
    return scause;
}

inline void RISCV::w_scause(uint64 scause) {
    asm volatile ("csrw scause, %[scause]" : : [scause] "r" (scause));
}

inline uint64 RISCV::r_sepc() {
    uint64 volatile sepc;
    asm volatile ("csrr %[sepc], sepc" : [sepc] "=r" (sepc));
    return sepc;
}

inline void RISCV::w_sepc(uint64 sepc) {
    asm volatile ("csrw sepc, %[sepc]" : : [sepc] "r" (sepc));
}

inline uint64 RISCV::r_stvec() {
    uint64 volatile stvec;
    asm volatile ("csrr %[stvec], stvec" : [stvec] "=r" (stvec));
    return stvec;
}

inline void RISCV::w_stvec(uint64 stvec) {
    asm volatile ("csrw stvec, %[stvec]" : : [stvec] "r" (stvec));
}

inline uint64 RISCV::r_stval() {
    uint64 volatile stval;
    asm volatile ("csrr %[stval], stval" : [stval] "=r" (stval));
    return stval;
}

inline void RISCV::w_stval(uint64 stval) {
    asm volatile ("csrw stval, %[stval]" : : [stval] "r" (stval));
}

inline void RISCV::ms_sip(uint64 mask) {
    asm volatile ("csrs sip, %[mask]" : : [mask] "r" (mask));
}

inline void RISCV::mc_sip(uint64 mask) {
    asm volatile ("csrc sip, %[mask]" : : [mask] "r" (mask));
}

inline uint64 RISCV::r_sip() {
    uint64 volatile sip;
    asm volatile ("csrr %[sip], sip" : [sip] "=r" (sip));
    return sip;
}

inline void RISCV::w_sip(uint64 sip) {
    asm volatile ("csrw sip, %[sip]" : : [sip] "r" (sip));
}

inline void RISCV::ms_sstatus(uint64 mask) {
    asm volatile ("csrs sstatus, %[mask]" : : [mask] "r" (mask));
}

inline void RISCV::mc_sstatus(uint64 mask) {
    asm volatile ("csrc sstatus, %[mask]" : : [mask] "r" (mask));
}

inline uint64 RISCV::r_sstatus() {
    uint64 volatile sstatus;
    asm volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r" (sstatus));
    return sstatus;
}

inline void RISCV::w_sstatus(uint64 sstatus) {
    asm volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r" (sstatus));
}

inline uint64 RISCV::r_a0() {
    uint64 volatile a0;
    asm volatile ("mv %0, a0" : "=r" (a0));
    return a0;
}

inline void RISCV::w_a0(uint64 writeValue) {
    asm volatile ("mv a0, %0" : : "r" (writeValue));
}

inline uint64 RISCV::r_sscratch() {
    uint64 volatile sscratch;
    __asm__ volatile ("csrr %0, sscratch" : "=r" (sscratch));
    return sscratch;
}

#endif
