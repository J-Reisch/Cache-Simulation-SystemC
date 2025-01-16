//
// Created by julian on 1/3/25.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include <cstdint>

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

    SC_HAS_PROCESS(CACHE);
    CACHE (sc_module_name name, uint8_t numCacheLevels, uint32_t cacheLineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numLinesPerSet) {
      std::cout << "------ CACHE created ------\ncache levels: " << (int)numCacheLevels << "\ncache line size: " << cacheLineSize << "\nnumLinesL1: " << numLinesL1 << "\nlatencyCacheL1: " << latencyCacheL1 << "\nmapping strategy: " << (int)mappingStrategy << "\nlines per set: " << numLinesPerSet << "\n------------------" << std::endl;
    	// TODO: different for each cache layer in case of fully associative
    	// direct-mapped caches have one line per set, fully associative caches have only one set

        /*

        if (mappingStrategy == 0) { // direct-mapped
      		numLinesPerSet = 1;
    	} else if (mappingStrategy == 1) { // fully associative
         	numLinesPerSet = cachelineSize;
    	}
         */

    	// TODO: initialize signals

        SC_THREAD(behaviour);
        sensitive << clk.pos();
    }

    void behaviour() {
        while(true) {
            std::cout << "Triggered at time: " << sc_time_stamp() << std::endl;

            if (r.read()) {
              ready.write(0);
              for (uint32_t i = 0; i < 3; i++) {
                wait();
              }
                // read from cache

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

    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
        // returns byte in cache-level level and cache-line lineIndex at offset index
        return 0;
    }
};

#endif //MODULES_HPP
