//
// Created by julian on 1/3/25.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include <cstdint>

#include "level.hpp"

using namespace sc_core;

SC_MODULE(CACHE) {
    sc_in<bool> clk;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;
    sc_in<bool> r;
    sc_in<bool> w;
    sc_in<bool> mem_ready;
    sc_in<uint32_t> mem_rdata;

    sc_out<uint32_t> rdata;
    sc_out<bool> ready;
    sc_out<uint32_t> mem_addr;
    sc_out<uint32_t> mem_wdata;
    sc_out<bool> mem_r;
    sc_out<bool> mem_w;

    // signals for level 1
    sc_signal<uint32_t> l1_addr;
    sc_signal<uint32_t> l1_wdata;
    sc_signal<bool> l1_r;
    sc_signal<bool> l1_w;
    sc_signal<bool> l1_access; // used to change order of LRU

    sc_signal<uint32_t> l1_rdata;
    sc_signal<bool> l1_ready;
    sc_signal<bool> l1_miss;

    LEVEL l1;

    SC_HAS_PROCESS(CACHE);
    CACHE (sc_module_name name, uint8_t numCacheLevels, uint32_t cacheLineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numLinesPerSet);

    void behaviour();

    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index);
};

#endif //MODULES_HPP
