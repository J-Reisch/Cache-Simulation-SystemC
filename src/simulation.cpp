//
// Created by julian on 1/3/25.
//

#include <systemc>
#include <cstdint>

#include "request.h"
#include "result.h"
#include "cache.hpp"
#include "main_memory.hpp"
#include "simulation.hpp"


void clock_tick(sc_signal<bool> *clk, uint32_t *cycleCount) {
     clk->write(1);
     sc_start(1, SC_NS);
     clk->write(0);
     sc_start(1, SC_NS);
     (*cycleCount)++;
}

struct Result run_simulation (
uint32_t cycles,
const char* tracefile,
uint8_t numCacheLevels,
uint32_t cachelineSize,
uint32_t numLinesL1,
uint32_t numLinesL2,
uint32_t numLinesL3,
uint32_t latencyCacheL1,
uint32_t latencyCacheL2,
uint32_t latencyCacheL3,
uint8_t mappingStrategy,
uint32_t numRequests,
struct Request* requests,
uint32_t memoryLatency,
uint32_t numLinesPerSet
) {
    // signals sc_in
    sc_signal<bool> clk;
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> wdata;
    sc_signal<bool> r;
    sc_signal<bool> w;
    sc_signal<bool> mem_ready;
    sc_signal<uint32_t> mem_rdata;

    // signals sc_out
    sc_signal<uint32_t> rdata;
    sc_signal<bool> ready;
    sc_signal<uint32_t> mem_addr;
    sc_signal<uint32_t> mem_wdata;
    sc_signal<bool> mem_r;
    sc_signal<bool> mem_w;

    // initialize cache and main_memory
    CACHE cache("cache", numCacheLevels, cachelineSize, numLinesL1, numLinesL2, numLinesL3, latencyCacheL1, latencyCacheL2, latencyCacheL3, mappingStrategy, numLinesPerSet);
    MAIN_MEMORY mainMemory("main_memory", memoryLatency);

    // bind cache ports
    cache.clk.bind(clk);
    cache.addr.bind(addr);
    cache.wdata.bind(wdata);
    cache.r.bind(r);
    cache.w.bind(w);
    cache.mem_ready.bind(mem_ready);
    cache.mem_rdata.bind(mem_rdata);

    cache.rdata.bind(rdata);
    cache.ready.bind(ready);
    cache.mem_addr.bind(mem_addr);
    cache.mem_wdata.bind(mem_wdata);
    cache.mem_r.bind(mem_r);
    cache.mem_w.bind(mem_w);

    // bind main memory ports
    mainMemory.clk.bind(clk);
    mainMemory.addr.bind(mem_addr);
    mainMemory.wdata.bind(mem_wdata);
    mainMemory.r.bind(mem_r);
    mainMemory.w.bind(mem_w);

    mainMemory.rdata.bind(mem_rdata);
    mainMemory.ready.bind(mem_ready);

    uint32_t cycleCount = 0;
    Request current;
    // process requests

    for (int i = 0; i < numRequests; i++) {
     	current = requests[i];
        std::cout << "Request " << i << ": " << (current.w ? "write" : "read") << std::endl;

        // write cache input ports
      	addr.write(current.addr);
        wdata.write(current.data);
        w.write(current.w == 1);
        r.write(current.w != 1);

        // wait until cache is done
        do {
        	clock_tick(&clk, &cycleCount);

            if (cycleCount >= cycles) {
          		break;
        	}
        } while (!ready.read());

    }


    sc_stop();
    return {cycleCount, 0, 0, 0};
}

int sc_main(int argc, char* argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}