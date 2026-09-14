#include "../inc/Console.hpp"

#include "../test/printing.hpp"

ConsoleBuffer* myConsole::inputBuffer = nullptr;
ConsoleBuffer* myConsole::outputBuffer = nullptr;

void myConsole::init() {
    inputBuffer = new ConsoleBuffer();
    outputBuffer = new ConsoleBuffer();
}

uint64 myConsole::readStatus() {
    uint64 status;
    asm volatile ("lbu %0, 0(%1)" : "=r"(status) : "r"(CONSOLE_STATUS));
    return status;
}

uint64 myConsole::readRX() {
    uint64 RX;
    asm volatile ("lbu %0, 0(%1)" : "=r"(RX) : "r"(CONSOLE_RX_DATA));
    return RX;
}

void myConsole::sendTX(uint64 c) {
    asm volatile ("sb %0, 0(%1)" : : "r"(c), "r"(CONSOLE_TX_DATA));
}

char myConsole::getc() {
    char c = inputBuffer->get();
    if (c == 0xd) c = 0xa;
    return c;
}

void myConsole::putc(char c) {
    outputBuffer->put(c);
}

void myConsole::putCharToBuffer(void*) {
    while (true) {
        char c = outputBuffer->get();
        while (!(readStatus() & CONSOLE_TX_STATUS_BIT)) {}
        sendTX(static_cast<uint64>(c));
    }
}

void myConsole::getCharFromBuffer() {
    while (readStatus() & CONSOLE_RX_STATUS_BIT) {
        char c = static_cast<char>(readRX());
        inputBuffer->put(c);
    }
}
