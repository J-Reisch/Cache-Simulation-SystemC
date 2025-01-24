//
// Created by julian on 1/24/25.
//

#include <vector>
#include <cstdint>
#include <iostream>

#include "cache_line.hpp"

CacheLine::CacheLine(uint32_t size) {
    // initialize all bytes with zero
    bytes.resize(size, 0);
}

uint32_t CacheLine::read(size_t offset, uint8_t* buffer, size_t length) {
    return 42;
}

void CacheLine::write(uint32_t offset, const uint8_t* data, size_t length) {

}

void CacheLine::print() {
    for (uint32_t i = 0; i < bytes.size(); i++) {
        std::cout << std::hex << (int)bytes[i] << " ";
    }
    std::cout << std::dec << std::endl;
}
