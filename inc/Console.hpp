#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#include "../inc/ConsoleBuffer.hpp"
#include "../lib/hw.h"

class myConsole {
public:
    static void init();
    static uint64 readStatus();
    static uint64 readRX();
    static void sendTX(uint64 c);

    static void putc(char c);
    static char getc();

    static void putCharToBuffer(void*);
    static void getCharFromBuffer();

    static ConsoleBuffer *inputBuffer, *outputBuffer;
};


#endif
