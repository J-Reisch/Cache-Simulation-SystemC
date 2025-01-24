//
// Created by julian on 1/24/25.
//

#ifndef CACHE_LINE_HPP
#define CACHE_LINE_HPP

#include <vector>
#include <cstdint>
#include <iostream>

class CacheLine {
private:
    std::vector<uint8_t> bytes; // Storage for the cache line data

public:
    explicit CacheLine(uint32_t size);

    uint32_t read(size_t offset, uint8_t* buffer, size_t length);

    void write(uint32_t offset, const uint8_t* data, size_t length);

    void print();
};

#endif //CACHE_LINE_HPP
