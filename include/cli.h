
#ifndef CLI_H
#define CLI_H

#include <stdint.h>

struct Request { 
    uint32_t addr ;
    uint32_t data ;
    uint8_t w; 
};

struct Result { 
    uint32_t cycles ;
    uint32_t misses ;
    uint32_t hits ;
    uint32_t primitiveGateCount ;
};


struct Result run_simulation ( 
    uint32_t cycles ,
    const char* tracefile ,
    uint8_t numCacheLevels ,
    uint32_t cachelineSize ,
    uint32_t numLinesL1 ,
    uint32_t numLinesL2 ,
    uint32_t numLinesL3 ,
    uint32_t latencyCacheL1 ,
    uint32_t latencyCacheL2 ,
    uint32_t latencyCacheL3 ,
    uint8_t mappingStrategy ,
    uint32_t numRequests ,
    struct Request* requests
);

int read_request_file(const char *filename, struct Request **requests, uint32_t *numRequests);

int parse_cli(int argc, char *argv[], uint32_t *cycles, const char **tracefile,
              uint8_t *numCacheLevels, uint32_t *cachelineSize, uint32_t *numLinesL1,
              uint32_t *numLinesL2, uint32_t *numLinesL3, uint32_t *latencyCacheL1,
              uint32_t *latencyCacheL2, uint32_t *latencyCacheL3, uint8_t *mappingStrategy, 
              char **input_file);

void print_help();

#endif