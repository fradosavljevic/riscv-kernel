#include "../inc/syscall_cpp.hpp"
#include "../inc/syscall_c.hpp"

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread() : myHandle(nullptr), body(threadWrapper), arg(this) {}

Thread::~Thread() {}

int Thread::start() {
    if (myHandle == nullptr) return thread_create(&myHandle, body, arg);
    return 0;
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::threadWrapper(void* arg)
{
    auto t = static_cast<Thread *>(arg);
    t->run();
}

PeriodicThread::PeriodicThread(const time_t period) : Thread(periodicThreadWrapper, this) {
    this->period = period;
}

void PeriodicThread::periodicThreadWrapper(void* arg) {
    auto thread = static_cast<PeriodicThread *>(arg);
    while (thread->period != -1UL) {
        thread->periodicActivation();
        sleep(thread->period);
    }
}

void PeriodicThread::terminate() {
    this->period = -1UL;
}

Semaphore::Semaphore(const unsigned init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
