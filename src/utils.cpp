//
// Created by julian on 1/23/25.
//

#include "utils.hpp"
#include <cstdint>

uint8_t log2_int(uint32_t value) {
    // calculate log, round up if not power of two
    uint8_t log = 0;
    if (value <= 1) {
        return 0;
    }

    value--;
    while (value >>= 1) {
        log++;
    }
    return log + 1;
}

uint32_t mergeData(uint32_t data1, uint32_t data2, uint8_t bytesToRight) {
    // takes "bytesToRight" bytes from data1 and the rest from data2
    uint32_t result = (data1 & ((1 << (bytesToRight * 8)) - 1))
                      | (data2 << (bytesToRight * 8));
    return result;
}
