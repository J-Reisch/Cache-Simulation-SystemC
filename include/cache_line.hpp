//
// Created by julian on 1/24/25.
//

#ifndef CACHE_LINE_HPP
#define CACHE_LINE_HPP

#include <vector>
#include <cstdint>
#include <iostream>

class CacheLine {
    std::vector<uint8_t> bytes; // Storage for the cache line data

public:
    uint32_t tag;

    CacheLine(uint32_t size, uint32_t tag);

    uint32_t read(size_t offset);

    void write(uint32_t offset, uint32_t wdata, uint8_t numBytes);

    void printCacheLine();
};

#endif //CACHE_LINE_HPP
