//
// Created by julian on 1/23/25.
//

#include <systemc>
#include <cstdint>

#include "level.hpp"

uint8_t log2_int(uint32_t value) { // calculate log, round up if not power of two
    uint8_t log = 0;
    if (value <= 1) {
        return 0;
    }

    value--;
    while (value >>= 1) {
        log++;
    }
    return log + 1;
}


LEVEL::LEVEL(sc_module_name name, uint32_t cacheLineSize, uint32_t numLines, uint32_t latency, uint8_t mappingStrategy, uint32_t numLinesPerSet)  {

    // direct-mapped caches have one line per set, fully associative caches have only one set
    if (mappingStrategy == 0) { // direct-mapped
        numLinesPerSet = 1;
    } else if (mappingStrategy == 1) { // fully associative
        numLinesPerSet = numLines;
    }

    this->cacheLineSize = cacheLineSize;
    this->numLines = numLines;
    this->latency = latency;
    this->mappingStrategy = mappingStrategy;
    this->numLinesPerSet = numLinesPerSet;

    this->numCacheSets = numLines / numLinesPerSet; // 1 in case of fully associative
    // calculate number of offset Bits (same for all levels since cacheLineSize is same)
    this->numOffsetBits = log2_int(cacheLineSize); // cacheLineSize is always power of 2
    // calculate number of index Bits for each level (first calculate number of cache-Sets (numLinesLX / numLinesPerSet))
    this->numIndexBits = log2_int(numCacheSets); // TODO: what to do with remainder? (there should be no remainder when parameters are specified correctly)
    // calculate number of tag Bits for each level (32 - index Bits of level - offset Bits)
    this->numTagBits = 32 - numOffsetBits - numIndexBits;


    /*
    std::cout << "NEW LAYER CREATED" << std::endl;
    std::cout << "numOffsetBits = " << (int)numOffsetBits << std::endl;
    std::cout << "numIndexBits = " << (int)numIndexBits << std::endl;
    std::cout << "numTagBits = " << (int)numTagBits << std::endl;
    */

    SC_THREAD(behaviour);
    sensitive << clk.pos();
}

void LEVEL::behaviour() {
    while(true) {
        if (r.read()) {
            ready.write(0);

            // split address into Tag, Index, Offset
            uint32_t address = addr.read();
            uint32_t tag = addr >> (numOffsetBits + numIndexBits);
            uint32_t index = (addr << numTagBits) >> (numOffsetBits + numTagBits);
            uint32_t offset = (addr << (numTagBits + numIndexBits)) >> (numTagBits + numIndexBits);

            /*
            std::cout << "ADDRESS: 0x" << std::hex << address << std::dec << std::endl;
            std::cout << "numTagBits:" << (int)numTagBits << std::endl;
            std::cout << "numIndexBits:" << (int)numIndexBits << std::endl;
            std::cout << "numOffsetBits:" << (int)numOffsetBits << std::endl;
            std::cout << "Tag: 0x" << std::hex << tag << std::endl;
            std::cout << "Index: 0x" << std::hex << index << std::endl;
            std::cout << "Offset: 0x" << std::hex << offset << std::dec << std::endl;
            */

            // TODO: access according CacheSet

            for (uint32_t i = 0; i < latency; i++) {
                wait();
            }

            ready.write(1);
        } else if (w.read()) {

        } else if (access.read()) {

        }
        wait();
    }
}