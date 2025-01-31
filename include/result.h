//
// Created by julian on 1/3/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct Result {
    uint32_t cycles;
    uint32_t misses;
    uint32_t hits;
    uint32_t primitiveGateCount;
};

#ifdef __cplusplus
}
#endif
