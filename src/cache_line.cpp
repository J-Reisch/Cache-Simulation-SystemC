//
// Created by julian on 1/24/25.
//

#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>

#include "cache_line.hpp"

CacheLine::CacheLine(uint32_t size, uint32_t tag) {
    // initialize all bytes with zero
    bytes.resize(size, 0);
    this->tag = tag;
}

uint32_t CacheLine::read(size_t offset) {
    if (offset + 4 > bytes.size()) {
        std::cout << "READ ADDRESS crosses cache lines" << std::endl; // TODO: easiest: fill with zeros
    }
    uint32_t result = 0;
    std::memcpy(&result, &bytes[offset], 4);
    return result;
}

void CacheLine::write(uint32_t offset, uint32_t wdata) {
    if (offset + 4 > bytes.size()) {
        std::cout << "WRITE ADDRESS crosses cache lines" << std::endl; // TODO: easiest: fill with zeros
    }
    std::memcpy(&bytes[offset], &wdata, 4);
}

void CacheLine::printCacheLine() {
    std::cout << "Tag 0x" << std::hex << tag << ": ";
    for (uint32_t i = 0; i < bytes.size(); i++) {
        std::cout << std::hex << (int)bytes[i] << " ";
    }
    std::cout << std::dec << std::endl;
}
