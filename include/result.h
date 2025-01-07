//
// Created by julian on 1/3/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

    struct Result {
        size_t cycles;
        size_t misses;
        size_t hits;
        size_t primitiveGateCount;
    };

#ifdef __cplusplus
}
#endif