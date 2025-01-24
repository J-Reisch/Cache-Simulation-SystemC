//
// Created by julian on 1/24/25.
//

#include "cache_set.hpp"

CacheSet::CacheSet(uint32_t numLines) {
    this->numLines = numLines;
}

uint32_t CacheSet::read(uint32_t tag, uint32_t offset, bool* miss) {
    auto mapEntry = this->cacheMap.find(tag);
    if (mapEntry != this->cacheMap.end()) {
        *miss = false;
        CacheLine& line = *(mapEntry->second);
        // TODO: call read() of the line
    } else {
        *miss = true;
        std::cout << "Line with this tag not in CacheSet" << std::endl;
    }
}

void CacheSet::write(uint32_t tag, uint32_t offset, uint32_t wdata) {

}
