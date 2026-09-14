#ifndef CONSOLEBUFFER_HPP
#define CONSOLEBUFFER_HPP
#include "SCB.hpp"

class ConsoleBuffer {
public:
    ConsoleBuffer() : head(0), tail(0), charAvailable(0), spaceAvailable(BUFFER_SIZE) {};
    void put(char c);
    char get();
    bool empty() const;

    void* operator new(uint64 size);
    void operator delete(void* ptr);
private:
    int head, tail;
    SCB charAvailable, spaceAvailable;
    static int constexpr BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
};

#endif
