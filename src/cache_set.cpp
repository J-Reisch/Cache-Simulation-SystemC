//
// Created by julian on 1/24/25.
//

#include <iostream>

#include "cache_set.hpp"

CacheSet::CacheSet() {
    this->numLines = 0;
    this->lineSize = 0;
}

CacheSet::CacheSet(uint32_t numLines, uint32_t lineSize) {
    this->numLines = numLines;
    this->lineSize = lineSize;
}

void CacheSet::printCacheSet() {
    for (auto it = cacheLines.begin(); it != cacheLines.end(); ++it) {
        CacheLine &cacheLine = *it;
        cacheLine.printCacheLine();
    }
}

uint32_t CacheSet::read(uint32_t tag, uint32_t offset, bool *miss) {
    auto mapEntry = this->cacheMap.find(tag);
    if (mapEntry != this->cacheMap.end()) {
        // line exists
        *miss = false;
        CacheLine &line = *(mapEntry->second);
        this->access(tag, offset);
        return line.read(offset);
    } else {
        *miss = true;
        return 0;
    }
}

void CacheSet::write(uint32_t tag, uint32_t offset, uint32_t wdata, uint8_t bytes) {
    // check if line exists
    auto mapEntry = this->cacheMap.find(tag);
    if (mapEntry != this->cacheMap.end()) {
        // line exists
        CacheLine &line = *(mapEntry->second);
        line.write(offset, wdata, bytes);
    } else {
        // cache miss (since there is a read() check first, this implies that the value did exist in no cache line and was loaded from RAM)
        if (cacheLines.size() >= numLines) {
            // remove LRU first
            auto lru = cacheLines.begin();
            uint32_t lruTag = (*lru).tag;
            cacheMap.erase(lruTag);
            cacheLines.erase(lru);
        }
        // add new cache line
        cacheLines.emplace_back(lineSize, tag); // in-place creation of CacheLine (no unnecessary copying)
        auto it = std::prev(cacheLines.end());
        it->write(offset, wdata, bytes);
        cacheMap[tag] = it;
    }
}

void CacheSet::access(uint32_t tag, uint32_t offset) {
    // label line as most recently used
    auto mapEntry = this->cacheMap.find(tag);
    if (mapEntry != this->cacheMap.end()) {
        // line exists
        auto listIterator = (mapEntry->second);
        // move line to the end of the list
        cacheLines.splice(cacheLines.end(), cacheLines, listIterator);
        // Update the map to point to the new position
        mapEntry->second = std::prev(cacheLines.end());
    } else {
        std::cerr << "Error: Cache is not inclusive" << std::endl;
    }
}
