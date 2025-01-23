//
// Created by julian on 1/23/25.
//

#include <systemc>
#include <cstdint>

#include "level.hpp"

uint8_t log2_int(uint32_t value) {
    uint8_t log = 0;
    while (value >>= 1) { // Shift right until the value becomes 0
        log++;
    }
    return log;
}


LEVEL::LEVEL(sc_module_name name, uint32_t cacheLineSize, uint32_t numLines, uint32_t latency, uint8_t mappingStrategy, uint32_t numLinesPerSet)  {
    // calculate number of offset Bits (same for all levels since cacheLineSize is same)
    uint8_t numOffsetBits = log2_int(cacheLineSize); // cacheLineSize is always power of 2
    // calculate number of index Bits for each level (first calculate number of cache-Sets (numLinesLX / numLinesPerSet))
    uint32_t numCacheSets = numLines / numLinesPerSet;
    uint8_t numIndexBits = log2_int(numCacheSets); // TODO: what to do with remainder?
    // calculate number of tag Bits for each level (32 - index Bits of level - offset Bits)
    uint8_t numTagBits = 32 - numOffsetBits - numIndexBits;

    SC_THREAD(behaviour);
    sensitive << clk.pos();
}

void LEVEL::behaviour() {
    while(true) {
        wait();
    }
}