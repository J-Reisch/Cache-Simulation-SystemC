#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "result.h"
#include "request.h"

extern struct Result run_simulation (uint32_t cycles, const char* tracefile, uint8_t numCacheLevels, uint32_t cachelineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numRequests, struct Request* requests);

void print_help(const char* programName) {
    const char* msg =
          "Usage: %s [options] <filename>\n"
          "required arguments are:\n"
          "  <filename>                      The path to a csv file containing cache requests\n"
          "options are:\n"
          "  -c, --cycles <num>              The number of simulated cycles\n"
          "  -t, --tf <tracefile>            Path to the trace file\n"
          "  -n, --num-cache-levels <num>    Number of cache levels (1-3)\n"
          "  -s, --cacheline-size <num>      Cache line size (in bytes)\n"
          "  --num-lines-l1 <num>            Number of lines in L1 cache\n"
          "  --num-lines-l2 <num>            Number of lines in L2 cache\n"
          "  --num-lines-l3 <num>            Number of lines in L3 cache\n"
          "  --latency-cache-l1 <num>        Latency of L1 cache\n"
          "  --latency-cache-l2 <num>        Latency of L2 cache\n"
          "  --latency-cache-l3 <num>        Latency of L3 cache\n"
          "  -m, --mapping-strategy <num>    Mapping strategy (0-2)\n" // TODO: better explanation?
          "  --memory-latency <num>          The latency for RAM (equal for read and write)\n" // TODO: does this param make sense?
          "  --numLinesPerSet <num>          For set-associative caches only\n" // TODO: one parameter per layer?
          "  --tf=<filename>                 The path to the tracefile (no tracefile if the option is not set)\n"
          "  -h/--help                       Print this help message and exit"
          "  --test                          Run tests and exit\n";

    fprintf(stderr, msg, programName);
}

void print_usage_msg(const char* programName) {
    const char* msg =
            "Usage: %s [options] <filename>\n"
            "Run %s --help for a list of options\n";
    fprintf(stderr, msg, programName, programName);
}

int main(int argc, char *argv[]) {
    // parse parameters

    const char* programName = argv[0];

    // default values
    uint32_t cycles = 100;
    const char *tracefile = NULL;
    uint8_t numCacheLevels = 1;
    uint32_t cachelineSize = 16;
    uint32_t numLinesL1 = 16, numLinesL2 = 32, numLinesL3 = 64;
    uint32_t latencyCacheL1 = 4, latencyCacheL2 = 8, latencyCacheL3 = 16;
    uint8_t mappingStrategy = 1;
    char *inputFile = "test/example.csv";
    uint32_t memoryLatency = 100;
    uint32_t numLinesPerSet = 4;

    struct Request *requests = NULL;
    uint32_t numRequests;

    // TODO: getopt_long parameter parsing



    struct Result simu_res = run_simulation(cycles, tracefile, numCacheLevels, cachelineSize,
                                            numLinesL1, numLinesL2, numLinesL3, latencyCacheL1,
                                            latencyCacheL2, latencyCacheL3, mappingStrategy,
                                            numRequests, requests);

    printf("Simulation Results:\n");
    printf("Cycles: %lu\n", simu_res.cycles);
    printf("Hits: %lu\n", simu_res.hits);
    printf("Misses: %lu\n", simu_res.misses);
    printf("Primitive Gate Count: %lu\n", simu_res.primitiveGateCount);

    free(requests);
    return 0;
}
