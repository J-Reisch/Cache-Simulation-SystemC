//
// Created by julian on 1/23/25.
//

#include <systemc>
#include <cstdint>

#include "cache.hpp"
#include "level.hpp"
#include "utils.hpp"


CACHE::CACHE(sc_module_name name, uint8_t numCacheLevels, uint32_t cacheLineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numLinesPerSet): sc_module(name),
    l1("l1", cacheLineSize, numLinesL1, latencyCacheL1, mappingStrategy, numLinesPerSet),
    l2("l2", cacheLineSize, numLinesL2, latencyCacheL2, mappingStrategy, numLinesPerSet),
    l3("l3", cacheLineSize, numLinesL3, latencyCacheL3, mappingStrategy, numLinesPerSet) {
    //std::cout << "------ CACHE created ------\ncache levels: " << (int)numCacheLevels << "\ncache line size: " << cacheLineSize << "\nnumLinesL1: " << numLinesL1 << "\nlatencyCacheL1: " << latencyCacheL1 << "\nmapping strategy: " << (int)mappingStrategy << "\nlines per set: " << numLinesPerSet << "\n------------------" << std::endl;

    // initialize level 1
    l1.clk.bind(clk);
    l1.addr.bind(levelSignals[0].addr); // signals of L1 are at index 0
    l1.wdata.bind(levelSignals[0].wdata);
    l1.r.bind(levelSignals[0].r);
    l1.w.bind(levelSignals[0].w);
    l1.access.bind(levelSignals[0].access);

    l1.rdata.bind(levelSignals[0].rdata);
    l1.ready.bind(levelSignals[0].ready);
    l1.miss.bind(levelSignals[0].miss);

    // initialize level 2
    l2.clk.bind(clk);
    l2.addr.bind(levelSignals[1].addr); // signals of L1 are at index 0
    l2.wdata.bind(levelSignals[1].wdata);
    l2.r.bind(levelSignals[1].r);
    l2.w.bind(levelSignals[1].w);
    l2.access.bind(levelSignals[1].access);

    l2.rdata.bind(levelSignals[1].rdata);
    l2.ready.bind(levelSignals[1].ready);
    l2.miss.bind(levelSignals[1].miss);

    // initialize level 3
    l3.clk.bind(clk);
    l3.addr.bind(levelSignals[2].addr); // signals of L1 are at index 0
    l3.wdata.bind(levelSignals[2].wdata);
    l3.r.bind(levelSignals[2].r);
    l3.w.bind(levelSignals[2].w);
    l3.access.bind(levelSignals[2].access);

    l3.rdata.bind(levelSignals[2].rdata);
    l3.ready.bind(levelSignals[2].ready);
    l3.miss.bind(levelSignals[2].miss);

    for (int i = 0; i < 3; i++) {
        levelSignals[i].addr = 0;
        levelSignals[i].wdata = 0;
        levelSignals[i].r = false;
        levelSignals[i].w = false;
        levelSignals[i].access = false;

        levelSignals[i].rdata = false;
        levelSignals[i].ready = false;
        levelSignals[i].miss = false;
    }

    this->numCacheLevels = numCacheLevels;
    this->cacheLineSize = cacheLineSize;

    SC_THREAD(behaviour);
    sensitive << clk.pos();
}

void CACHE::writeToRAM(uint32_t addr, uint32_t data) {
    mem_addr.write(addr);
    mem_wdata.write(data);
    mem_w.write(true);

    wait();
    mem_w.write(false);

    while (!mem_ready.read()) {
        wait();
    }
}

uint32_t CACHE::readFromRAM(uint32_t addr) {
    mem_addr.write(addr);
    mem_r.write(true);

    wait();
    mem_r.write(false);
    wait();

    while (!mem_ready.read()) {
        wait();
    }

    uint32_t result = mem_rdata.read();

    return result;
}

void CACHE::cacheMiss(uint32_t address, bool read, bool write, uint32_t data) {
    if (write) {
        // write 4 Bytes to RAM
        writeToRAM(address, data);
    }

    // get start address of cache line
    uint8_t numOffsetBits = log2_int(cacheLineSize);
    uint32_t startAddress = (address >> numOffsetBits) << numOffsetBits; // cut off Offset Bits

    // load the cache line from RAM
    uint32_t word, currentAddr;
    for (int i = 0; i < cacheLineSize / 4; i++) {
        currentAddr = startAddress + i * 4;
        std::cout << "Copying address: " << std::hex << currentAddr << std::dec << std::endl;
        word = readFromRAM(currentAddr);
        // write word to L1
        levelSignals[0].addr.write(currentAddr);
        levelSignals[0].wdata.write(word);

        levelSignals[0].w.write(true);
        wait();
        levelSignals[0].w.write(false);
        wait();

        while (!levelSignals[0].ready.read()) {
            wait();
        }

        // write word to L2
        if (numCacheLevels > 1) {
            // TODO
        }

        // write word to L3
        if (numCacheLevels > 2) {
            // TODO
        }
    }
}

void CACHE::printCache() {
    std::cout << "------ CACHE BEGIN ------" << std::endl;
    std::cout << "L1: " << std::endl;
    l1.printLevel();
    std::cout << "------ CACHE END --------" << std::endl;
}

void CACHE::behaviour() {
    while(true) {
        //std::cout << "Cache triggered at time: " << sc_time_stamp() << std::endl;

        if (r.read()) {
            ready.write(false);

            // try to read from level 1
            // read from L1
            levelSignals[0].addr.write(addr.read());
            levelSignals[0].r.write(true);
            wait();
            levelSignals[0].r.write(false);
            wait();

            while (!levelSignals[0].ready.read()) {
                wait();
            }

            if (levelSignals[0].miss.read()) {
                // TODO: also check L2 and L3
                std::cout << "value not in L1" << std::endl;

                // CACHE MISS
                cacheMiss(addr, r, w, wdata);
            } else {
                rdata.write(levelSignals[0].rdata.read());
            }

            // ready
            ready.write(true);
        } else if (w.read()) {
            ready.write(false);
            // write to cache

            // read from L1 to see if line exists, if it doesn't exist, go to L2, ..., if in no level, get from RAM
            levelSignals[0].addr.write(addr.read());

            levelSignals[0].r.write(true);
            wait();
            levelSignals[0].r.write(false);
            wait();

            while (!levelSignals[0].ready.read()) {
                wait();
            }

            if (levelSignals[0].miss.read()) {
                // TODO: also check L2 and L3
                std::cout << "value not in L1" << std::endl;

                // CACHE MISS
                cacheMiss(addr, r, w, wdata);
            }

            // ready
            ready.write(true);
        }
        wait(); // necessary???
    }
}

uint8_t CACHE::getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
    // TODO: returns byte in cache-level level and cache-line lineIndex at offset index
    return 0;
}