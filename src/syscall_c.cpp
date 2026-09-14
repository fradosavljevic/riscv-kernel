#include "../inc/syscall_c.hpp"
#include "../test/printing.hpp"

void *mem_alloc(uint64 size) {
    uint64 blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    uint64 addr;
    asm volatile ("mv a1, %0" : : "r"(blocks));
    asm volatile ("li a0, 0x01");
    asm volatile ("ecall");
    asm volatile ("mv %0, a0" : "=r"(addr));
    return reinterpret_cast<void *>(addr);
}

int mem_free(void *addr) {
    int status;
    asm volatile ("mv a1, %0" : : "r"(addr));
    asm volatile ("li a0, 0x02");
    asm volatile ("ecall");
    asm volatile ("mv %0, a0" : "=r"(status));
    return status;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    asm volatile ("mv a3, %0" : : "r"(arg));
    asm volatile ("mv a2, %0" : : "r"(start_routine));
    asm volatile ("mv a1, %0" : : "r"(handle));
    asm volatile ("li a0, 0x11");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r"(status));
    return status;
}

int thread_exit() {
    asm volatile ("li a0, 0x12");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r"(status));
    return status;
}

void thread_dispatch() {
    asm volatile ("li a0, 0x13");
    asm volatile ("ecall");
}

int sem_open(sem_t *handle, unsigned init) {
    asm volatile ("mv a2, %0" : : "r" (init));
    asm volatile ("mv a1, %0" : : "r" (handle));
    asm volatile ("li a0, 0x21");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_close(sem_t handle) {
    asm volatile ("mv a1, %0" : : "r" (handle));
    asm volatile ("li a0, 0x22");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_wait(sem_t id) {
    asm volatile ("mv a1, %0" : : "r" (id));
    asm volatile ("li a0, 0x23");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_signal(sem_t id) {
    asm volatile ("mv a1, %0" : : "r" (id));
    asm volatile ("li a0, 0x24");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_wait_n(sem_t id, unsigned n) {
    asm volatile ("mv a2, %0" : : "r" (n));
    asm volatile ("mv a1, %0" : : "r" (id));
    asm volatile ("li a0, 0x25");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

int sem_signal_n(sem_t id, unsigned n) {
    asm volatile ("mv a2, %0" : : "r" (n));
    asm volatile ("mv a1, %0" : : "r" (id));
    asm volatile ("li a0, 0x26");
    asm volatile ("ecall");
    int status;
    asm volatile ("mv %0, a0" : "=r" (status));
    return status;
}

char getc() {
    asm volatile ("li a0, 0x41");
    asm volatile ("ecall");
    uint64 c;
    asm volatile ("mv %0, a0" : "=r"(c));
    return (char)c;
}

void putc(char c) {
    asm volatile ("mv a1, %0" : : "r" (c));
    asm volatile ("li a0, 0x42");
    asm volatile ("ecall");
}

int time_sleep(time_t t) {
    asm volatile ("mv a1, %0" : : "r" (t));
    asm volatile ("li a0, 0x31");
    asm volatile ("ecall");
    int status;
    asm volatile("mv %0, a0" : "=r" (status));
    return status;
}