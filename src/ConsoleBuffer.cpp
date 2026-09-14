#include "../inc/ConsoleBuffer.hpp"
#include "../inc/MemoryAllocator.hpp"

char ConsoleBuffer::get() {
    charAvailable.wait(1);
    char c = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    spaceAvailable.signal(1);
    return c;
}

void ConsoleBuffer::put(const char c) {
    spaceAvailable.wait(1);
    buffer[tail] = c;
    tail = (tail + 1) % BUFFER_SIZE;
    charAvailable.signal(1);
}

void *ConsoleBuffer::operator new(uint64 size) {
    return MemoryAllocator::mallocWrapper((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
}

void ConsoleBuffer::operator delete(void *ptr) {
    MemoryAllocator::freeWrapper(ptr);
}

bool ConsoleBuffer::empty() const {
    return head == tail;
}
