//
// Created by julian on 1/3/25.
//

#include <stdint.h>

#define MEMORY_LATENCY 100
#define NUM_LINES_PER_SET 2

#ifdef __cplusplus
extern "C" {
#endif

struct Request {
    uint32_t addr;
    uint32_t data;
    uint8_t w;
};

#ifdef __cplusplus
}
#endif
