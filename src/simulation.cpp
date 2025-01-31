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
     clk->write(true);
     sc_start(1, SC_NS);
     clk->write(false);
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
struct Request* requests
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
	sc_signal<bool> hit;

	// initialize signals
	clk.write(false);
	addr.write(0);
	wdata.write(0);
	r.write(false);
	w.write(false);
	mem_ready.write(false);
	mem_rdata.write(0);

	rdata.write(0);
	ready.write(false);
	mem_addr.write(0);
	mem_wdata.write(0);
	mem_r.write(false);
	mem_w.write(false);
	hit.write(false);

    // initialize cache and main_memory
    CACHE cache("cache", numCacheLevels, cachelineSize, numLinesL1, numLinesL2, numLinesL3, latencyCacheL1, latencyCacheL2, latencyCacheL3, mappingStrategy, NUM_LINES_PER_SET);
    MAIN_MEMORY mainMemory("main_memory", MEMORY_LATENCY);

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
	cache.hit.bind(hit);

    // bind main memory ports
    mainMemory.clk.bind(clk);
    mainMemory.addr.bind(mem_addr);
    mainMemory.wdata.bind(mem_wdata);
    mainMemory.r.bind(mem_r);
    mainMemory.w.bind(mem_w);

    mainMemory.rdata.bind(mem_rdata);
    mainMemory.ready.bind(mem_ready);

	//Check if a trace file is supposed to be created
	sc_trace_file* traceFilePtr;
	if (tracefile != NULL) {
		traceFilePtr = sc_create_vcd_trace_file(tracefile);
		// sc_trace(traceFilePtr, clk, "Clock signal"); // not tracked for better readability of tracefile
		sc_trace(traceFilePtr, addr, "Cache address");
		sc_trace(traceFilePtr, wdata, "Cache wdata");
		sc_trace(traceFilePtr, r, "Cache r");
		sc_trace(traceFilePtr, w, "Cache w");
		sc_trace(traceFilePtr, rdata, "Cache rdata");
		sc_trace(traceFilePtr, ready, "Cache ready");
		sc_trace(traceFilePtr, mem_ready, "Memory mem_ready");
		sc_trace(traceFilePtr, mem_rdata, "Memory mem_rdata");
		sc_trace(traceFilePtr, mem_addr, "Memory mem_addr");
		sc_trace(traceFilePtr, mem_wdata, "Memory mem_wdata");
		sc_trace(traceFilePtr, mem_r, "Memory mem_r");
		sc_trace(traceFilePtr, mem_w, "Memory mem_w");
	}

	uint32_t cycleCount = 0;
	Request* current;
	uint32_t cacheMisses = 0;
	uint32_t cacheHits = 0;

    for (int i = 0; i < numRequests; i++) {
     	current = &requests[i];

    	if (current->w) {
    		std::cout << "Request " << i << ": WRITE 0x" << std::hex << current->data << " to 0x" << current->addr << std::dec << std::endl;
    	} else {
    		std::cout << "Request " << i << ": READ FROM 0x" << std::hex << current->addr << std::dec << std::endl;
    	}

        // write cache input ports
      	addr.write(current->addr);
        wdata.write(current->data);
        w.write(current->w == 1);
        r.write(current->w != 1);

    	clock_tick(&clk, &cycleCount);

    	w.write(false);
    	r.write(false);

        // wait until cache is ready
        do {
        	clock_tick(&clk, &cycleCount);

            if (cycleCount >= cycles) {
          		break;
        	}
        } while (!ready.read());

    	if (cycleCount >= cycles) {
    		break;
    	}

    	if (current->w != 1) {
    		current->data = rdata.read();
    	}

    	if (cache.hit.read()) {
    		cacheHits++;
    	} else {
    		cacheMisses++;
    	}

    	cache.printCache();

    	if (current->w != 1) {
    		std::cout << "DATA READ: " << std::hex << rdata.read() << std::dec << std::endl;
    	}
    }

    sc_stop();

	if (tracefile != nullptr) {
		sc_close_vcd_trace_file(traceFilePtr);
	}

    return {cycleCount, cacheMisses, cacheHits, 42};
}

int sc_main(int argc, char* argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}