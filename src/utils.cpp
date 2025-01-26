//
// Created by julian on 1/23/25.
//

#include "utils.hpp"
#include <cstdint>

uint8_t log2_int(uint32_t value) { // calculate log, round up if not power of two
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