//
// Created by julian on 1/3/25.
//

/*
adapted version of the example solution of the MAIN_MEMORY homework which is used to test the cache
 */

#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include <systemc>
#include <map>
using namespace sc_core;

SC_MODULE(MAIN_MEMORY) {
    sc_in<bool> clk;

    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;
    sc_in<bool> r;
    sc_in<bool> w;

    sc_out<uint32_t> rdata;
    sc_out<bool> ready;

    uint32_t latency;

    std::map<uint32_t, uint8_t> memory;

    SC_HAS_PROCESS(MAIN_MEMORY);

    MAIN_MEMORY(sc_module_name name, uint32_t memoryLatency);

    void behaviour();

    void doRead();

    void doWrite();

    uint32_t get(uint32_t address);

    void set(uint32_t address, uint32_t value);
};

#endif // MAIN_MEMORY_HPP
