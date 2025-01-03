//
// Created by julian on 1/3/25.
//

#ifndef LAYER_HPP
#define LAYER_HPP

#include <systemc>
#include <cstdint>

using namespace sc_core;

#define MACRO 3333


SC_MODULE(LAYER) {
    sc_in<bool> w;
    sc_in<bool> r;
    sc_in<uint32_t> wd;
    sc_in<bool> force;

    sc_out<bool> miss;
    sc_out<uint32_t> rd;

    SC_CTOR(LAYER) {
        SC_THREAD(behaviour);
        sensitive << clk.pos(); // sensitive to sc_in changes
    }

    void behaviour() {
        while(true) {
            wait();

            if (r.read()) {
                // read from cache
            } else if (w.read()) {
                // write to cache
            }
        }
    }

    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
        // returns byte in cache-level level and cache-line lineIndex at offset index
    }
};

#endif //LAYER_HPP
