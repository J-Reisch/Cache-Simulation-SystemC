//
// Created by julian on 1/23/25.
//

#include <systemc>
#include <cstdint>

#include "level.hpp"
#include "utils.hpp"

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

void LEVEL::printLevel() {
    for (auto it = cacheSets.begin(); it != cacheSets.end(); it++) {
        uint32_t index = it->first;
        CacheSet cacheSet = it->second;

        std::cout << "Cache set " << index << ":" << std::endl;
        cacheSet.printCacheSet();
        std::cout << std::endl;
    }
}

void LEVEL::behaviour() {
    while(true) {
        if (r.read()) {
            ready.write(false);

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

            // access according CacheSet
            auto mapEntry = this->cacheSets.find(index);
            if (mapEntry != this->cacheSets.end()) {
                miss.write(false);
                CacheSet& set = mapEntry->second;
                bool missInSet = false;
                uint32_t data = set.read(tag, offset, &missInSet);
                if (missInSet) {
                    miss.write(true);
                } else {
                    miss.write(false);
                    rdata.write(data);
                }
            } else {
                miss.write(true);
            }

            for (uint32_t i = 0; i < latency; i++) {
                wait();
            }

            ready.write(true);
        } else if (w.read()) {
            ready.write(false);

            // split address into Tag, Index, Offset
            uint32_t address = addr.read();
            uint32_t tag = addr >> (numOffsetBits + numIndexBits);
            uint32_t index = (addr << numTagBits) >> (numOffsetBits + numTagBits);
            uint32_t offset = (addr << (numTagBits + numIndexBits)) >> (numTagBits + numIndexBits);

            // access according CacheSet
            auto mapEntry = this->cacheSets.find(index);
            if (mapEntry != this->cacheSets.end()) {
                CacheSet& set = mapEntry->second;
                set.write(tag, offset, wdata);
            } else {
                // create CacheSet
                cacheSets[index] = CacheSet(numLinesPerSet, cacheLineSize);
                cacheSets[index].write(tag, offset, wdata);
            }


            for (uint32_t i = 0; i < latency; i++) {
                wait();
            }

            ready.write(true);
        } else if (access.read()) {

        }
        wait();
    }
}