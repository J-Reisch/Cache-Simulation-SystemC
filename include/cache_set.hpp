//
// Created by julian on 1/24/25.
//

#ifndef CACHESET_HPP
#define CACHESET_HPP

#include <unordered_map>
#include <list>
#include <cstdint>
#include "cache_line.hpp"

class CacheSet {
private:
    uint32_t numLines;
    uint32_t lineSize;
    // Map from tag to iterator in LRU list
    std::unordered_map<uint32_t, std::list<CacheLine>::iterator> cacheMap; // amortized constant time complexity

    // CacheLines are in a list for LRU (most recently accessed element will be at the end)
    std::list<CacheLine> cacheLines;

public:
    CacheSet();

    CacheSet(uint32_t numLines, uint32_t lineSize);

    void printCacheSet();

    uint32_t read(uint32_t tag, uint32_t offset, bool *miss);

    void write(uint32_t tag, uint32_t offset, uint32_t wdata, uint8_t bytes);

    void access(uint32_t tag, uint32_t offset);
};

#endif // CACHESET_HPP
