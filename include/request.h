//
// Created by julian on 1/3/25.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    struct Request {
        uint32_t addr;
        uint32_t data;
        int we;
    };

#ifdef __cplusplus
}
#endif
