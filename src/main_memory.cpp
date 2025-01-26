//
// Created by julian on 1/23/25.
//

/*
adapted version of the example solution of the MAIN_MEMORY homework which is used to test the cache
 */

#include "main_memory.hpp"

MAIN_MEMORY::MAIN_MEMORY (sc_module_name name, uint32_t memoryLatency) {
    latency = memoryLatency;

    SC_THREAD(behaviour);
    sensitive << clk.pos();
}

void MAIN_MEMORY::behaviour() {
    while(true) {
        wait();

        if (r.read()) {
            doRead();
        } else if (w.read()) {
            doWrite();
        }
    }
}

void MAIN_MEMORY::doRead() {
    ready.write(false);

    uint32_t result = get(addr.read());

    for(int i = 0; i < latency; i++) {
        wait();
    }

    rdata.write(result);
    ready.write(true);
}

void MAIN_MEMORY::doWrite() {
    ready.write(false);
    set(addr.read(), wdata.read());

    for(int i = 0; i < latency; i++) {
        wait();
    }

    ready.write(true);
}

uint32_t MAIN_MEMORY::get(uint32_t address) {
    std::cout << "RAM reading: " << std::hex << address << std::dec << std::endl;
    uint32_t result = 0;

    for (int i = 0; i < 4; i++) {
        uint8_t value = 0;
        if(memory.find(address + i) != memory.end()) {
            value = memory[address + i];
        }
        result |= value << (i * 8);
    }

    return result;
}

void MAIN_MEMORY::set(uint32_t address, uint32_t value) {
    std::cout << "RAM writing: " << std::hex << address << " " << value << std::dec << std::endl;
    for (int i = 0; i < 4; i++) {
        memory[address + i] = (value >> (i * 8)) & 0xFF;
        if(address + i == UINT32_MAX) {
            break;
        }
    }
}