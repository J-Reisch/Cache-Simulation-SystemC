#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>

#include "result.h"
#include "request.h"
#include "csv_parser.h"

extern struct Result run_simulation (uint32_t cycles, const char* tracefile, uint8_t numCacheLevels, uint32_t cachelineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numRequests, struct Request* requests);

void print_help(const char* programName) {
      // TODO: update short options (inconsistent)
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
          "  --tf <filename>                 The path to the tracefile (no tracefile if the option is not set)\n"
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
    uint32_t cycles = 100;
    uint8_t numCacheLevels = 1;
    uint32_t cacheLineSize = 16;
    uint32_t numLinesL1 = 16, numLinesL2 = 32, numLinesL3 = 64;
    uint32_t latencyCacheL1 = 4, latencyCacheL2 = 8, latencyCacheL3 = 16;
    uint8_t mappingStrategy = 1;
    const char *tracefile = NULL;
    char *inputFile = "test/example.csv";
    uint32_t memoryLatency = 100;
    uint32_t numLinesPerSet = 4;

    // parse options (TODO: add memory-latency <num> and --numLinesPerSet <num> and test)

    const struct option long_options[] = {
        {"num-cache-levels", required_argument, NULL, 'l'},
        {"cacheline-size", required_argument, NULL, 's'},
        {"num-lines-l1", required_argument, NULL, 'x'},
        {"num-lines-l2", required_argument, NULL, 'y'},
        {"num-lines-l3", required_argument, NULL, 'z'},
        {"latency-cache-l1", required_argument, NULL, 'a'},
        {"latency-cache-l2", required_argument, NULL, 'b'},
        {"latency-cache-l3", required_argument, NULL, 'c'},
        {"mapping-strategy", required_argument, NULL, 'm'},
        {"cycles", required_argument, NULL, 't'},
        {"tf", required_argument, NULL, 'e'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "l:s:x:y:z:a:b:c:m:t:e:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l':
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
            case 'x':
                if (string_to_uint32_t(optarg, &numLinesL1)) {
                    return 1;
                }
                break;
            case 'y':
                if (string_to_uint32_t(optarg, &numLinesL2)) {
                    return 1;
                }
                break;
            case 'z':
                if (string_to_uint32_t(optarg, &numLinesL3)) {
                    return 1;
                }
                break;
            case 'a':
                if (string_to_uint32_t(optarg, &latencyCacheL1)) {
                    return 1;
                }
                break;
            case 'b':
                if (string_to_uint32_t(optarg, &latencyCacheL2)) {
                    return 1;
                }
                break;
            case 'c':
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
            case 't':
                if (string_to_uint32_t(optarg, &cycles)) {
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
        inputFile = argv[optind]; // positional argument is <file>
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
    printf("Tracefile: %s\n", tracefile ? tracefile : "None");
    printf("Input File: %s\n", inputFile);

    const char* filename = argv[optind]; // optind is first positional arg

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

    // TODO: Check if cache is large enough and if all values make sense

    // TODO: check if csv_parser works for edge cases

    // TODO: implement test (call seperate function in seperate file and then terminate)

    int numRequests;
    struct Request* requests = parse_csv(filename, &numRequests);

    struct Result simu_res = run_simulation(cycles, tracefile, numCacheLevels, cacheLineSize,
                                            numLinesL1, numLinesL2, numLinesL3, latencyCacheL1,
                                            latencyCacheL2, latencyCacheL3, mappingStrategy,
                                            numRequests, requests);

    printf("Simulation Results:\n");
    printf("Cycles: %lu\n", simu_res.cycles);
    printf("Hits: %lu\n", simu_res.hits);
    printf("Misses: %lu\n", simu_res.misses);
    printf("Primitive Gate Count: %lu\n", simu_res.primitiveGateCount);

    free(requests); // TODO
    return 0;
}
