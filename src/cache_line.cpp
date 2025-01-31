//
// Created by julian on 1/24/25.
//

#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <iomanip>

#include "cache_line.hpp"

CacheLine::CacheLine(uint32_t size, uint32_t tag) {
    // initialize all bytes with zero
    bytes.resize(size, 0);
    this->tag = tag;
}

uint32_t CacheLine::read(size_t offset) {
    uint32_t result = 0;

    if (offset >= bytes.size()) {
        std::cerr << "Error: CacheLine read out of bounds for offset " << offset << "\n";
        return result;
    }

    size_t readableBytes = std::min((size_t) 4, bytes.size() - offset);
    std::memcpy(&result, &bytes[offset], readableBytes);

    return result;
}

void CacheLine::write(uint32_t offset, uint32_t wdata, uint8_t numBytes) {
    if (offset >= bytes.size()) {
        std::cerr << "Error: CacheLine write out of bounds for offset " << offset << "\n";
        return;
    }

    size_t writableBytes = std::min((size_t) numBytes, bytes.size() - offset);
    std::memcpy(&bytes[offset], &wdata, writableBytes);
}

void CacheLine::printCacheLine() {
    std::cout << "Tag 0x" << std::hex << tag << ": ";
    for (uint32_t i = 0; i < bytes.size(); i++) {
        // prints leading zeros and upper case HEX-letters
        std::cout << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int) bytes[i] << " ";
    }
    std::cout << std::nouppercase << std::dec << std::endl;
}
