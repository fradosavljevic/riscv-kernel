#include "../inc/MemoryAllocator.hpp"
#include "../inc/Memory.hpp"
#include "../inc/RISCV.hpp"

MemoryAllocator::MemBlock* MemoryAllocator::memHead = nullptr;
uint MemoryAllocator::initialSize = 0;

void MemoryAllocator::init() {
    memHead = reinterpret_cast<MemBlock*>(Memory::start());
    memHead->size = Memory::totalSize() - sizeof(MemBlock);
    Memory::increaseUsage(sizeof(MemBlock));
    initialSize = memHead->size;
    memHead->next = nullptr;
    memHead->prev = nullptr;
    memHead->isFree = true;
}

void *MemoryAllocator::malloc(const uint64 blocks) {
    if(!memHead) return nullptr;
    const uint64 sz = blocks * MEM_BLOCK_SIZE;
    if (sz <= 0) return nullptr;
    MemBlock* cur = memHead;
    MemBlock* best = nullptr;

    for(; cur != nullptr; cur = cur->next) {
        if(cur->size >= sz) {
            if(!best || cur->size < best->size) {
                best = cur;
            }
        }
    }

    if(!best) return nullptr; //nema dovoljno slobodne memorije
    if (best->size >= sz + sizeof(MemBlock) + MEM_BLOCK_SIZE) {
        const auto newBlock = reinterpret_cast<MemBlock *>(reinterpret_cast<uint64>(best) + sizeof(MemBlock) + sz);
        newBlock->size = best->size - sz - sizeof(MemBlock);
        newBlock->isFree = true;
        best->size = sz;
        best->isFree = false;

        Memory::increaseUsage(best->size + sizeof(MemBlock));

        newBlock->next = best->next;
        newBlock->prev = best->prev;

        if (best->next) best->next->prev = newBlock;
        if (best->prev) best->prev->next = newBlock;
        else memHead = newBlock;
    } else {
        if (best->next) best->next->prev = best->prev;
        if (best->prev) best->prev->next = best->next;
        else memHead = best->next;
        Memory::increaseUsage(best->size + sizeof(MemBlock));
    }

    best->next = nullptr;
    best->prev = nullptr;
    best->isFree = false;
    best->checkCode = MEM_CHECK_CODE;

    return reinterpret_cast<void *>(reinterpret_cast<uint64>(best) + sizeof(MemBlock));
}

int MemoryAllocator::free(const void *memory) {
    if (!memory) return -1;
    if (reinterpret_cast<uint64>(memory) > Memory::end() || reinterpret_cast<uint64>(memory) < Memory::start()) return -2;

    const uint64 blockAddr = reinterpret_cast<uint64>(memory) - sizeof(MemBlock);
    auto block = reinterpret_cast<MemBlock *>(blockAddr);
    if (block->checkCode != MEM_CHECK_CODE) return -3;
    if (block->isFree) return 0;
    block->isFree = true;
    block->checkCode = 0xDEADBEEF;

    Memory::decreaseUsage(block->size);

    MemBlock* prev = nullptr;
    for (MemBlock* cur = memHead; cur != nullptr && reinterpret_cast<uint64>(cur) < blockAddr; cur = cur->next) prev = cur;
    MemBlock* next = prev ? prev->next : memHead;

    if (prev && reinterpret_cast<uint64>(prev) + sizeof(MemBlock) + prev->size == blockAddr) {
        prev->size += sizeof(MemBlock) + block->size;
        Memory::decreaseUsage(sizeof(MemBlock));
        block = prev;
    } else {
        block->prev = prev;
        block->next = next;
        if (prev) prev->next = block;
        else memHead = block;
        if (next) next->prev = block;
    }

    if (block->next && reinterpret_cast<uint64>(block) + sizeof(MemBlock) + block->size == reinterpret_cast<uint64>(block->next)) {
        block->size += block->next->size + sizeof(MemBlock);
        Memory::decreaseUsage(sizeof(MemBlock));
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }

    return 0;
}

void* MemoryAllocator::mallocWrapper(uint64 blocks) {
    uint64 sstatus = RISCV::r_sstatus();
    RISCV::mc_sstatus(RISCV::SSTATUS_SIE);
    void* result = malloc(blocks);
    RISCV::w_sstatus(sstatus);
    return result;
}

int MemoryAllocator::freeWrapper(const void* memory) {
    uint64 sstatus = RISCV::r_sstatus();
    RISCV::mc_sstatus(RISCV::SSTATUS_SIE);
    const int result = free(memory);
    RISCV::w_sstatus(sstatus);
    return result;
}