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
    MAIN_MEMORY (sc_module_name name, uint32_t memoryLatency) {
      	latency = memoryLatency;

		SC_THREAD(behaviour);
        sensitive << clk.pos();
	}

    void behaviour() {
        while(true) {
            wait();

            if (r.read()) {
                doRead();
            } else if (w.read()) {
                doWrite();
            }
        }
    }

    void doRead() {
        ready.write(false);

        uint32_t result = get(addr.read());

        for(int i = 0; i < latency; i++) {
            wait();
        }

        rdata.write(result);
        ready.write(true);
    }

    void doWrite() {
        ready.write(false);
        set(addr.read(), wdata.read());

        for(int i = 0; i < latency; i++) {
            wait();
        }

        ready.write(true);
    }

    uint32_t get(uint32_t address) {
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

    void set(uint32_t address, uint32_t value) {
        for (int i = 0; i < 4; i++) {
            memory[address + i] = (value >> (i * 8)) & 0xFF;
            if(address + i == UINT32_MAX) {
                break;
            }
        }
    }
};

#endif // MAIN_MEMORY_HPP