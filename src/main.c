#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "result.h"
#include "request.h"
#include "csv_parser.h"

extern struct Result run_simulation (uint32_t cycles, const char* tracefile, uint8_t numCacheLevels, uint32_t cachelineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numRequests, struct Request* requests, uint32_t memoryLatency, uint32_t numLinesPerSet);

void print_help(const char* programName) {
      // TODO: update short options (inconsistent)
    const char* msg =
          "Usage: %s [options] <filename>\n"
          "required arguments are:\n"
          "  <filename>                      The path to a csv file containing cache requests\n"
          "options are:\n"
          "  -n, --num-cache-levels <num>    Number of cache levels (1-3)\n"
          "  -s, --cacheline-size <num>      Cache line size (in bytes)\n"
          "  --num-lines-l1 <num>            Number of lines in L1 cache\n"
          "  --num-lines-l2 <num>            Number of lines in L2 cache\n"
          "  --num-lines-l3 <num>            Number of lines in L3 cache\n"
          "  --latency-cache-l1 <num>        Latency of L1 cache\n"
          "  --latency-cache-l2 <num>        Latency of L2 cache\n"
          "  --latency-cache-l3 <num>        Latency of L3 cache\n"
          "  -m, --mapping-strategy <num>    Mapping strategy (0-2)\n" // TODO: better explanation?
          "  -c, --cycles <num>              The number of simulated cycles\n"
          "  -t, --tf <tracefile>            Path to the trace file (no tracefile if the option is not set)\n"
          "  -l, --memory-latency <num>      The latency for RAM (equal for read and write)\n"
          "  -p, --num-lines-per-set <num>   For set-associative caches only\n" // TODO: one parameter per layer?
          "  -h, --help                      Print this help message and exit"
          "  --test                          Run tests and exit\n";

    fprintf(stderr, msg, programName);
}

void print_usage_msg(const char* programName) {
    const char* msg =
            "Usage: %s [options] <filename>\n"
            "Run %s --help for a list of options\n";
    fprintf(stderr, msg, programName, programName);
}

int string_to_uint32_t(const char *str, uint32_t *value) {
    char *endptr;
    *value = strtoul(str, &endptr, 10);
    return (*endptr == '\0') ? 0 : 1;
}

int string_to_uint8_t(const char *str, uint8_t *value) {
    char *endptr;
    *value = strtoul(str, &endptr, 10);
    return (*endptr == '\0') ? 0 : 1;
}

int main(int argc, char *argv[]) {

    const char* programName = argv[0];

    // default values
    uint32_t cycles = 1000;
    uint8_t numCacheLevels = 1;
    uint32_t cacheLineSize = 16;
    uint32_t numLinesL1 = 16, numLinesL2 = 32, numLinesL3 = 64;
    uint32_t latencyCacheL1 = 4, latencyCacheL2 = 8, latencyCacheL3 = 16;
    uint8_t mappingStrategy = 2; // 0 = direct-mapped, 1 = fully associative, 2 = set-associative
    const char *tracefile = NULL;
    char *inputFile = "test/example.csv";
    uint32_t memoryLatency = 100;
    uint32_t numLinesPerSet = 4;

    const struct option long_options[] = {
        {"num-cache-levels", required_argument, NULL, 'n'},
        {"cacheline-size", required_argument, NULL, 's'},
        {"num-lines-l1", required_argument, NULL, 'u'},
        {"num-lines-l2", required_argument, NULL, 'v'},
        {"num-lines-l3", required_argument, NULL, 'w'},
        {"latency-cache-l1", required_argument, NULL, 'x'},
        {"latency-cache-l2", required_argument, NULL, 'y'},
        {"latency-cache-l3", required_argument, NULL, 'z'},
        {"mapping-strategy", required_argument, NULL, 'm'},
        {"cycles", required_argument, NULL, 'c'},
        {"memory-latency", required_argument, NULL, 'l'},
        {"num-lines-per-set", required_argument, NULL, 'p'},
        {"tf", required_argument, NULL, 'e'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "n:s:u:v:w:x:y:z:m:c:l:p:e:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'n':
                if (string_to_uint8_t(optarg, &numCacheLevels) || numCacheLevels < 1 || numCacheLevels > 3) {
                    fprintf(stderr, "Invalid value for --num-cache-levels. Must be between 1 and 3.\n");
                    return 1;
                }
                break;
            case 's':
                if (string_to_uint32_t(optarg, &cacheLineSize)) {
                    return 1;
                }
                break;
            case 'u':
                if (string_to_uint32_t(optarg, &numLinesL1)) {
                    return 1;
                }
                break;
            case 'v':
                if (string_to_uint32_t(optarg, &numLinesL2)) {
                    return 1;
                }
                break;
            case 'w':
                if (string_to_uint32_t(optarg, &numLinesL3)) {
                    return 1;
                }
                break;
            case 'x':
                if (string_to_uint32_t(optarg, &latencyCacheL1)) {
                    return 1;
                }
                break;
            case 'y':
                if (string_to_uint32_t(optarg, &latencyCacheL2)) {
                    return 1;
                }
                break;
            case 'z':
                if (string_to_uint32_t(optarg, &latencyCacheL3)) {
                    return 1;
                }
                break;
            case 'm':
                if (string_to_uint8_t(optarg, &mappingStrategy) || mappingStrategy > 2) {
                    fprintf(stderr, "Invalid value for --mapping-strategy. Must be 0, 1, or 2.\n");
                    return 1;
                }
                break;
            case 'c':
                if (string_to_uint32_t(optarg, &cycles)) {
                    return 1;
                }
                break;
			case 'l':
                if (string_to_uint32_t(optarg, &memoryLatency)) {
                    return 1;
                }
                break;
            case 'p':
                if (string_to_uint32_t(optarg, &numLinesPerSet)) {
                    return 1;
                }
                break;
            case 'e':
                tracefile = optarg;
                break;
            case 'h':
                print_help(programName);
                return 0;
            default:
                print_usage_msg(programName);
                return 1;
        }
    }

    if (optind < argc) {
        inputFile = argv[optind]; // first positional argument is <file>
    } else {
        fprintf(stderr, "Missing input file\n");
        print_usage_msg(programName);
        return 1;
    }

    // Print parsed options
    printf("Cache Levels: %d\n", numCacheLevels);
    printf("Cache Line Size: %u\n", cacheLineSize);
    printf("L1 Cache Lines: %u\n", numLinesL1);
    printf("L2 Cache Lines: %u\n", numLinesL2);
    printf("L3 Cache Lines: %u\n", numLinesL3);
    printf("L1 Cache Latency: %u\n", latencyCacheL1);
    printf("L2 Cache Latency: %u\n", latencyCacheL2);
    printf("L3 Cache Latency: %u\n", latencyCacheL3);
    printf("Mapping Strategy: %u\n", mappingStrategy);
    printf("Cycles: %u\n", cycles);
    printf("Memory Latency: %u\n", memoryLatency);
    printf("Cache Lines Per Set: %u\n", numLinesPerSet);
    printf("Tracefile: %s\n", tracefile ? tracefile : "None");
    printf("Input File: %s\n", inputFile);

    // Check if input file name is legal

    // filename too long
    if (strlen(inputFile) > 255) {
      	fprintf(stderr, "Invalid options %s: input file name too long\n", programName);
        return 1;
    }

    // illegal characters
    const char *illegalCharacters = "\n\r:*?\"<>|";
    for (const char *c = inputFile; *c; ++c) {
        if (strchr(illegalCharacters, *c) != NULL) {
          	fprintf(stderr, "Invalid options %s: illegal characters in input file name\n", programName);
            return 1;
        }
    }

printf("before tracefile\n");
    // Check if tracefile name is legal
    if (tracefile != NULL) {
        char illegalCharacters[] = {'\n' ,':', '"'};
        if (strlen(tracefile) == 0) {
            fprintf(stderr, "Invalid options %s: Trace file name is empty\n", programName);
            return 1;
        }
        else if (tracefile[0] == '.') {
            fprintf(stderr, "Invalid options %s: Trace file name is invalid because filenames starting with dots are reserved for system files\n", programName);
            return 1;
        }
        for (int i = 0; i < 3; i++) {
            if (strchr(tracefile, illegalCharacters[i]) != NULL) {
                fprintf(stderr, "Invalid options %s: Trace file name contains illegal character \"%c\"\n", programName, illegalCharacters[i]);
                return 1;
            }
        }
    }

    // TODO: check if number of lines can be divided by numLinesPerSet for each layer

    // TODO: Check if cache is large enough and if all values make sense

    // TODO: implement test (call seperate function in seperate file and then terminate)
    if (cacheLineSize == 0) {
        fprintf(stderr, "size of cache line can't be zero");
        return 1;
    }

    int numRequests;

    // TODO: check if csv_parser works for edge cases

    struct Request* requests = parse_csv(inputFile, &numRequests);

    printf("--------- REQUESTS ---------\n");
    printf("WRITE\tADDRESS\tDATA\n");
    for (int i = 0; i < numRequests; i++) {
		printf("%u\t0x%x\t0x%x\n", requests[i].w, requests[i].addr, requests[i].data);
    }
    printf("---------- REQUESTS END ---------\n");

    struct Result simu_res = run_simulation(cycles, tracefile, numCacheLevels, cacheLineSize,
                                            numLinesL1, numLinesL2, numLinesL3, latencyCacheL1,
                                            latencyCacheL2, latencyCacheL3, mappingStrategy,
                                            numRequests, requests, memoryLatency, numLinesPerSet);

    printf("Simulation Results:\n");
    printf("Cycles: %u\n", simu_res.cycles);
    printf("Hits: %u\n", simu_res.hits);
    printf("Misses: %u\n", simu_res.misses);
    printf("Primitive Gate Count: %u\n", simu_res.primitiveGateCount);

    free(requests); // TODO
    return 0;
}
