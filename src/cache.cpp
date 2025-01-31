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

    // bind signals to L1
    l1.clk.bind(clk);
    l1.addr.bind(levelSignals[0].addr); // signals of L1 are at index 0
    l1.wdata.bind(levelSignals[0].wdata);
    l1.r.bind(levelSignals[0].r);
    l1.w.bind(levelSignals[0].w);
    l1.access.bind(levelSignals[0].access);
    l1.numBytes.bind(levelSignals[0].numBytes);

    l1.rdata.bind(levelSignals[0].rdata);
    l1.ready.bind(levelSignals[0].ready);
    l1.miss.bind(levelSignals[0].miss);

    // bind signals to L2
    l2.clk.bind(clk);
    l2.addr.bind(levelSignals[1].addr); // signals of L2 are at index 1
    l2.wdata.bind(levelSignals[1].wdata);
    l2.r.bind(levelSignals[1].r);
    l2.w.bind(levelSignals[1].w);
    l2.access.bind(levelSignals[1].access);
    l2.numBytes.bind(levelSignals[1].numBytes);

    l2.rdata.bind(levelSignals[1].rdata);
    l2.ready.bind(levelSignals[1].ready);
    l2.miss.bind(levelSignals[1].miss);

    // bind signals to L3
    l3.clk.bind(clk);
    l3.addr.bind(levelSignals[2].addr); // signals of L3 are at index 2
    l3.wdata.bind(levelSignals[2].wdata);
    l3.r.bind(levelSignals[2].r);
    l3.w.bind(levelSignals[2].w);
    l3.access.bind(levelSignals[2].access);
    l3.numBytes.bind(levelSignals[2].numBytes);

    l3.rdata.bind(levelSignals[2].rdata);
    l3.ready.bind(levelSignals[2].ready);
    l3.miss.bind(levelSignals[2].miss);

    // initialize signals
    for (int i = 0; i < 3; i++) {
        levelSignals[i].addr.write(0);
        levelSignals[i].wdata.write(0);
        levelSignals[i].r.write(false);
        levelSignals[i].w.write(false);
        levelSignals[i].access.write(false);
        levelSignals[i].numBytes.write(0);

        levelSignals[i].rdata.write(0);
        levelSignals[i].ready.write(false);
        levelSignals[i].miss.write(false);
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
    wait();

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

uint32_t CACHE::readFromLevel(uint8_t level, uint32_t addr, bool* miss) { // level is between 1 and 3
    levelSignals[level].addr.write(addr);

    levelSignals[level].r.write(true);
    wait();
    levelSignals[level].r.write(false);
    wait();

    while (!levelSignals[level].ready.read()) {
        wait();
    }

    if (miss) {
        *miss = levelSignals[level].miss.read();
    }

    return levelSignals[level].rdata.read();
}

void CACHE::writeToLevel(uint8_t level, uint32_t addr, uint32_t data, uint8_t bytes) {
    levelSignals[level].addr.write(addr);
    levelSignals[level].wdata.write(data);
    levelSignals[level].numBytes.write(bytes);
    std::cout << "write to level " << (int)level << " data: " << data << " bytes: " << (int)bytes << std::endl;

    levelSignals[level].w.write(true);
    wait();
    levelSignals[level].w.write(false);
    wait();

    while (!levelSignals[level].ready.read()) {
        wait();
    }
}

void CACHE::accessLevel(uint8_t level, uint32_t addr) {
    // This method labels the corresponding line as most recently used
    // This will not cause a delay

    levelSignals[level].addr.write(addr);
    levelSignals[level].access.write(true);
    wait();
    levelSignals[level].access.write(false);
    wait();
}

void CACHE::cacheMiss(uint32_t address, bool read, bool write, uint32_t data, uint8_t bytes) {
    hit.write(false);
    std::cout << "CACHE MISS " <<std::endl;
    if (write) {
        // write new data to RAM before loading the cache line
        writeToRAM(address, data);
    }

    // get start address of cache line
    uint8_t numOffsetBits = log2_int(this->cacheLineSize);
    uint32_t startAddress = (address >> numOffsetBits) << numOffsetBits; // cut off Offset Bits

    // load the cache line from RAM to all levels
    uint32_t word = 0, currentAddr = 0;
    for (int i = 0; i < this->cacheLineSize / 4; i++) {
        currentAddr = startAddress + i * 4;
        word = readFromRAM(currentAddr);

        // write word to all levels
        for (int i = 0; i < this->numCacheLevels; i++) {
            writeToLevel(i, currentAddr, word, bytes);
        }
    }
}

uint32_t CACHE::readFromCache(uint32_t address, uint32_t data, uint8_t bytes) {
    bool miss;
    uint32_t result = 0;
    for (int i = 0; i < numCacheLevels; i++) {
        result = readFromLevel(i, address, &miss);
        if (!miss) {
            // access remaining cache lines
            for (int j = i+1; j < numCacheLevels; j++) {
                accessLevel(j, address);
            }
            break;
        }
        if (i == numCacheLevels - 1) {
            cacheMiss(address, read, write, data, bytes); // load from RAM in L1, L2, L3
            result = readFromLevel(0, address, &miss); // read from L1 after cache miss
            if (miss) {
                std::cout << "something doesn't work" << std::endl;
            }
        }
    }
    return result;
}

void CACHE::writeToCache(uint32_t address, uint32_t data, uint8_t bytes) {
    bool miss;
    for (int i = 0; i < numCacheLevels; i++) {
        readFromLevel(i, address, &miss);
        if (!miss) {
            // found in L_i -> now write to L_i and everything above
            for (int j = i; j < numCacheLevels; j++) {
                writeToLevel(j, address, data, bytes);
            }
            writeToRAM(address, data);
            break;
        }
        if (i == numCacheLevels - 1) {
            cacheMiss(address, read, write, data, bytes); // write data to RAM and then load cache line from RAM in L1, L2, L3
        }
    }
}

void CACHE::printCache() {
    std::cout << "------ CACHE BEGIN ------" << std::endl;
    std::cout << "L1: " << std::endl;
    l1.printLevel();
    std::cout << "L2: " << std::endl;
    l2.printLevel();
    std::cout << "L3: " << std::endl;
    l3.printLevel();
    std::cout << "------ CACHE END --------" << std::endl;
}

void CACHE::behaviour() {
    while(true) {
        // save input signals
        address = addr.read();
        write = w.read();
        read = r.read();
        data = wdata.read();

        uint8_t bytesToRight = (cacheLineSize - (address % cacheLineSize));
        bool acrossLines = bytesToRight < 4;

        //std::cout << "Cache triggered at time: " << sc_time_stamp() << std::endl;

        if (read) {
            ready.write(false);
            hit.write(true);


            uint32_t result;
            if (acrossLines) {
                result = mergeData(readFromCache(address, 0, bytesToRight), readFromCache(address + bytesToRight, 0, 4 - bytesToRight), bytesToRight);
            } else {
                result = readFromCache(address, 0, 4);
            }

            rdata.write(result);

            ready.write(true);
        } else if (write) {
            ready.write(false);
            hit.write(true);

            if (acrossLines) {
                writeToCache(address, data, bytesToRight);
                writeToCache(address + bytesToRight, data >> 8 * bytesToRight, 4 - bytesToRight);
            } else {
                writeToCache(address, data, 4);
            }

            ready.write(true);
        }
        wait();
    }
}

uint8_t CACHE::getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
    // assumption: level = 1 corresponds to L1
    if (level == 1) {
        return l1.getCacheLineContentOfLevel(lineIndex * cacheLineSize + index);
    } else if (level == 2) {
        return l2.getCacheLineContentOfLevel(lineIndex * cacheLineSize + index);
    } else if (level == 3) {
        return l3.getCacheLineContentOfLevel(lineIndex * cacheLineSize + index);
    } else {
        std::cout << "Doesn't exist" << std::endl;
        return 0;
    }
}