//
// Created by julian on 1/23/25.
//

#include <systemc>
#include <cstdint>

#include "cache.hpp"
#include "level.hpp"


CACHE::CACHE (sc_module_name name, uint8_t numCacheLevels, uint32_t cacheLineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numLinesPerSet): sc_module(name), l1("l1", cacheLineSize, numLinesL1, latencyCacheL1, mappingStrategy, numLinesPerSet) {
    std::cout << "------ CACHE created ------\ncache levels: " << (int)numCacheLevels << "\ncache line size: " << cacheLineSize << "\nnumLinesL1: " << numLinesL1 << "\nlatencyCacheL1: " << latencyCacheL1 << "\nmapping strategy: " << (int)mappingStrategy << "\nlines per set: " << numLinesPerSet << "\n------------------" << std::endl;
    // TODO: different for each cache level in case of fully associative
    // direct-mapped caches have one line per set, fully associative caches have only one set

    /*

    if (mappingStrategy == 0) { // direct-mapped
          numLinesPerSet = 1;
    } else if (mappingStrategy == 1) { // fully associative
         numLinesPerSet = cachelineSize;
    }
     */

    // TODO: initialize signals

    // initialize level 1
    l1.clk.bind(clk);
    l1.addr.bind(l1_addr);
    l1.wdata.bind(l1_wdata);
    l1.r.bind(l1_r);
    l1.w.bind(l1_w);
    l1.access.bind(l1_access);

    l1.rdata.bind(l1_rdata);
    l1.ready.bind(l1_ready);
    l1.miss.bind(l1_miss);

    SC_THREAD(behaviour);
    sensitive << clk.pos();
}

void CACHE::behaviour() {
    while(true) {
        std::cout << "Cache triggered at time: " << sc_time_stamp() << std::endl;

        if (r.read()) {
            ready.write(0);
            for (uint32_t i = 0; i < 3; i++) {
                wait();
            }
            // read from L1

            // ready
            ready.write(1);
        } else if (w.read()) {
            ready.write(0);
            // write to cache
            for (uint32_t i = 0; i < 5; i++) {
                wait();
            }
            // ready
            ready.write(1);
        }
        wait(); // necessary???
    }
}

uint8_t CACHE::getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
    // returns byte in cache-level level and cache-line lineIndex at offset index
    return 0;
}