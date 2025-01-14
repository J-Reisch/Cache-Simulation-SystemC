//
// Created by julian on 1/3/25.
//

#include <systemc>
#include <cstdint>

#include "request.h"
#include "result.h"
#include "modules.hpp"

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
    sc_clock clk("clk", 1, SC_SEC); // change time

    // signals
    sc_signal<int> result;

    // initialize cache and main_memory
    //ADDER adder("adder");

    // bind
    //adder.clk.bind(clk);
    //adder.sum.bind(result);

    // start simulation
    sc_start(5, SC_SEC); //
    return {0, 0, 0, 0};
}

int sc_main(int argc, char* argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}