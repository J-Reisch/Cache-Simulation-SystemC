#include "cli.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    uint32_t cycles;
    const char *tracefile;
    uint8_t numCacheLevels;
    uint32_t cachelineSize;
    uint32_t numLinesL1, numLinesL2, numLinesL3;
    uint32_t latencyCacheL1, latencyCacheL2, latencyCacheL3;
    uint8_t mappingStrategy;
    char *input_file;
    struct Request *requests = NULL;
    uint32_t numRequests;


    int result;

    result = parse_cli(argc, argv, &cycles, &tracefile, &numCacheLevels, 
                       &cachelineSize, &numLinesL1, &numLinesL2, &numLinesL3, 
                       &latencyCacheL1, &latencyCacheL2, &latencyCacheL3, 
                       &mappingStrategy, &input_file);
    if (result != 0) {
        printf("Error: Failed to parse command-line arguments\n");
        return -1;
    }

    if (input_file == NULL) {
        printf("Error : Missing input file\n");
        return -1;
    }

    result = read_request_file(input_file, &requests, &numRequests);
    if (result != 0) {
        printf("Error: Unable to read requests from file %s\n", input_file);
        return -1; 
    }

    struct Result simu_res = run_simulation(cycles, tracefile, numCacheLevels, cachelineSize,
                                            numLinesL1, numLinesL2, numLinesL3, latencyCacheL1,
                                            latencyCacheL2, latencyCacheL3, mappingStrategy,
                                            numRequests, requests);

    printf("Simulation Results:\n");
    printf("Cycles: %u\n", simu_res.cycles);
    printf("Hits: %u\n", simu_res.hits);
    printf("Misses: %u\n", simu_res.misses);
    printf("Primitive Gate Count: %u\n", simu_res.primitiveGateCount);

    free(requests);
    return 0;
}
