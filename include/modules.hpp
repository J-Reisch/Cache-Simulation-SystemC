//
// Created by julian on 1/3/25.
//

#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include <cstdint>

using namespace sc_core;

#define MACRO 3333


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

    SC_CTOR(CACHE) { // add parameters
        SC_THREAD(behaviour);
        sensitive << clk.pos();
    }

    void behaviour() {
        while(true) {
            wait();

            if (r.read()) {
                  // set ready to 0
                // read from cache
            } else if (w.read()) {
                // set ready to 0
                // write to cache
            }
        }
    }

    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
        // returns byte in cache-level level and cache-line lineIndex at offset index
    }
};

#endif //MODULES_HPP
