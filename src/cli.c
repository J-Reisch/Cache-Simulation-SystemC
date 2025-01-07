#include "cli.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


void print_help() {
    printf("Usage: cache_simulator [options]\n");
    printf("Options:\n");
    printf("  -c, --cycles <n>              Number of cycles to run the simulation\n");
    printf("  -t, --tf <tracefile>          Path to the trace file\n");
    printf("  -n, --num-cache-levels <n>    Number of cache levels (1-3)\n");
    printf("  -s, --cacheline-size <n>      Cache line size (in bytes)\n");
    printf("  -L1, --num-lines-l1 <n>       Number of lines in L1 cache\n");
    printf("  -L2, --num-lines-l2 <n>       Number of lines in L2 cache\n");
    printf("  -L3, --num-lines-l3 <n>       Number of lines in L3 cache\n");
    printf("  -lat1, --latency-cache-l1 <n> Latency of L1 cache\n");
    printf("  -lat2, --latency-cache-l2 <n> Latency of L2 cache\n");
    printf("  -lat3, --latency-cache-l3 <n> Latency of L3 cache\n");
    printf("  -m, --mapping-strategy <n>    Mapping strategy\n");
    printf("  -h, --help                    Print this help message\n");
}



int read_request_file(const char *filename, struct Request **requests, uint32_t *numRequests) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return -1;
    }
    char line[256];
    *numRequests = 0;
    while (fgets(line, sizeof(line), file)) {
        struct Request req;
        char type;
        int n = sscanf(line, "%c %x %x", &type, &req.addr, &req.data);
        if (n < 2) {
            printf("Error: invalid line in input file\n");
            fclose(file);
            return -1;
        }
        if (type == 'W') {
            req.w = 1;
        } else {
            req.w = 0;
        }
        (*numRequests)++;
        *requests = realloc(*requests, *numRequests * sizeof(struct Request));
        (*requests)[*numRequests - 1] = req;
    }
    fclose(file);
    return 0;
}


int parse_cli(int argc, char *argv[], uint32_t *cycles, const char **tracefile,
              uint8_t *numCacheLevels, uint32_t *cachelineSize, uint32_t *numLinesL1,
              uint32_t *numLinesL2, uint32_t *numLinesL3, uint32_t *latencyCacheL1,
              uint32_t *latencyCacheL2, uint32_t *latencyCacheL3, uint8_t *mappingStrategy, 
              char **input_file) {
    *tracefile = NULL;
    *input_file = NULL;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'c' && i + 1 < argc) {
            *cycles = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 't' && i + 1 < argc) {
            *tracefile = argv[++i];
        } else if (argv[i][0] == '-' && argv[i][1] == 'n' && i + 1 < argc) {
            *numCacheLevels = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 's' && i + 1 < argc) {
            *cachelineSize = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'L' && argv[i][2] == '1' && i + 1 < argc) {
            *numLinesL1 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'L' && argv[i][2] == '2' && i + 1 < argc) {
            *numLinesL2 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'L' && argv[i][2] == '3' && i + 1 < argc) {
            *numLinesL3 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'l' && argv[i][2] == 'a' && argv[i][3] == 't' && argv[i][4] == '1' && i + 1 < argc) {
            *latencyCacheL1 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'l' && argv[i][2] == 'a' && argv[i][3] == 't' && argv[i][4] == '2' && i + 1 < argc) {
            *latencyCacheL2 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'l' && argv[i][2] == 'a' && argv[i][3] == 't' && argv[i][4] == '3' && i + 1 < argc) {
            *latencyCacheL3 = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'm' && i + 1 < argc) {
            *mappingStrategy = atoi(argv[++i]);
        } else if (argv[i][0] == '-' && argv[i][1] == 'i' && i + 1 < argc) {
            *input_file = argv[++i];
        } else if (argv[i][0] == '-' && argv[i][1] == 'h') {
            print_help();
            return 0;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_help();
            return -1;
        }
    }
    if (*input_file == NULL) {
        printf("Error : Missing input file argument\n");
        return -1;
    }

    return 0;
}

struct Result run_simulation(uint32_t cycles, const char* tracefile, uint8_t numCacheLevels,
                             uint32_t cachelineSize, uint32_t numLinesL1, uint32_t numLinesL2,
                             uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2,
                             uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numRequests,
                             struct Request* requests) {
    struct Result res = {0};
    res.cycles = cycles;
    for (uint32_t i = 0; i < numRequests; i++) {
        struct Request req = requests[i];
        int hit;
        if (req.addr % 2 == 0) {
            hit = 1;
        } else {
            hit = 0;
        }
        if (hit) {
            res.hits++;
            res.cycles += latencyCacheL1;
        } else {
            res.misses++;
            res.cycles += latencyCacheL2;
        }
    }
    return res;
}

