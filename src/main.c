#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#include "request.h"
#include "result.h"
#include "csv_parser.h"

extern struct Result run_simulation (uint32_t cycles, const char* tracefile, uint8_t numCacheLevels, uint32_t cachelineSize, uint32_t numLinesL1, uint32_t numLinesL2, uint32_t numLinesL3, uint32_t latencyCacheL1, uint32_t latencyCacheL2, uint32_t latencyCacheL3, uint8_t mappingStrategy, uint32_t numRequests, struct Request* requests);

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
          "  -m, --mapping-strategy <num>    Mapping strategy (0-2): 0 (direct-mapped), 1 (fully associative), or 2 (set-associative)\n"
          "  -c, --cycles <num>              The number of simulated cycles\n"
          "  -e, --tf <tracefile>            Path to the trace file (no tracefile if the option is not set)\n"
          "  -h, --help                      Print this help message and exit\n"
          "  -t, --test                      Run tests and exit\n";

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
    errno = 0;
    unsigned long val = strtoul(str, &endptr, 10);

    if (errno == ERANGE || val > UINT32_MAX) {
        return 1;
    }

    if (*endptr != '\0') {
        return 1;
    }

    *value = (uint32_t)val;
    return 0;
}

int string_to_uint8_t(const char *str, uint8_t *value) {
    char *endptr;
    errno = 0;

    unsigned long val = strtoul(str, &endptr, 10);

    if (errno == ERANGE || val > UINT8_MAX) {
        return 1;
    }

    if (*endptr != '\0') {
        return 1;
    }

    *value = (uint8_t)val;
    return 0;
}

bool is_valid_filename(const char *filename, const char *programName, const char *type) {
    // NULL is okay (tracefile not specified)
    if (filename == NULL) {
        return true;
    }

    // check length
    if (strlen(filename) > 255) {
        fprintf(stderr, "Invalid options %s: %s file name too long\n", programName, type);
        return false;
    }

    // check illegal characters
    const char *illegalCharacters = "\n\r:*?\"<>|";
    for (const char *c = filename; *c; ++c) {
        if (strchr(illegalCharacters, *c) != NULL) {
            fprintf(stderr, "Invalid options %s: %s file name contains illegal character \"%c\"\n", programName, type, *c);
            return false;
        }
    }

    // check if empty string
    if (strlen(filename) == 0) {
        fprintf(stderr, "Invalid options %s: %s file name is empty\n", programName, type);
        return false;
    }

    // don't allow hidden files
    if (filename[0] == '.') {
        fprintf(stderr, "Invalid options %s: %s file name starts with dot\n", programName, type);
        return false;
    }

    return true;
}

void run_tests() {
    printf("Running tests...\n");

    int numRequests;
    struct Request* requests = parse_csv("test/test.csv", &numRequests);

    struct Result test_res = run_simulation(100000, NULL, 2, 16,
                                            4, 8, 16, 10,
                                            20, 40, 2,
                                            numRequests, requests);

    bool correct = true;
    if (requests[1].data != 0xAAAAAAAA) {
        fprintf(stderr, "Simple read/write test failed: Expected request 2 to have data 0xAAAAAAAA, but got %#x\n", requests[1].data);
        correct = false;
        goto cleanup;
    }

    if (requests[4].data != 0xBBBBBBBB) {
        fprintf(stderr, "LRU test failed: Expected request 3 to have data 0xBBBBBBBB, but got %#x\n", requests[4].data);
        correct = false;
        goto cleanup;
    }

    if (requests[8].data != 0xEEDDDDDD) {
        fprintf(stderr, "write to same line test failed: Expected request 9 to have data 0xEEDDDDDD, but got %#x\n", requests[8].data);
        correct = false;
        goto cleanup;
    }

    if (requests[10].data != 0x34567800) {
        fprintf(stderr, "write across cache lines: Expected request 11 to have data 0x34567800, but got %#x\n", requests[10].data);
        correct = false;
        goto cleanup;
    }


    if (test_res.misses != 5) {
        fprintf(stderr, "Expected 4 misses in total, but got %u\n", test_res.misses);
        correct = false;
        goto cleanup;
    }

    cleanup:
    if (requests) {
        free(requests);
    }
    if (!correct) {
        exit(EXIT_FAILURE);
    } else {
        printf("All tests passed!\n");
    }
}

int main(int argc, char *argv[]) {

    const char* programName = argv[0];

    // set default values
    uint32_t cycles = 100000;
    uint8_t numCacheLevels = 1;
    uint32_t cacheLineSize = 16;
    uint32_t numLinesL1 = 16, numLinesL2 = 32, numLinesL3 = 64;
    uint32_t latencyCacheL1 = 4, latencyCacheL2 = 8, latencyCacheL3 = 16;
    uint8_t mappingStrategy = 2; // 0 = direct-mapped, 1 = fully associative, 2 = set-associative
    const char *tracefile = NULL;
    char *inputFile = "test/example.csv";

    int return_code = 0;

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
        {"tf", required_argument, NULL, 'e'},
        {"help", no_argument, NULL, 'h'},
        {"test", no_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "n:s:u:v:w:x:y:z:m:c:e:ht", long_options, NULL)) != -1) {
        switch (opt) {
            case 'n':
                if (string_to_uint8_t(optarg, &numCacheLevels) || numCacheLevels < 1 || numCacheLevels > 3) {
                    fprintf(stderr, "Invalid value for --num-cache-levels. Must be between 1 and 3.\n");
                    return 1;
                }
                break;
            case 's':
                if (string_to_uint32_t(optarg, &cacheLineSize)) {
                    fprintf(stderr, "Invalid value for --cacheline-size. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'u':
                if (string_to_uint32_t(optarg, &numLinesL1)) {
                    fprintf(stderr, "Invalid value for --num-lines-l1. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'v':
                if (string_to_uint32_t(optarg, &numLinesL2)) {
                    fprintf(stderr, "Invalid value for --num-lines-l2. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'w':
                if (string_to_uint32_t(optarg, &numLinesL3)) {
                    fprintf(stderr, "Invalid value for --num-lines-l3. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'x':
                if (string_to_uint32_t(optarg, &latencyCacheL1)) {
                    fprintf(stderr, "Invalid value for --latency-cache-l1. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'y':
                if (string_to_uint32_t(optarg, &latencyCacheL2)) {
                    fprintf(stderr, "Invalid value for --latency-cache-l2. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'z':
                if (string_to_uint32_t(optarg, &latencyCacheL3)) {
                    fprintf(stderr, "Invalid value for --latency-cache-l3. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'm':
                if (string_to_uint8_t(optarg, &mappingStrategy) || mappingStrategy > 2) {
                    fprintf(stderr, "Invalid value for --mapping-strategy. Must be 0 (direct-mapped), 1 (fully associative), or 2 (set-associative).\n");
                    return 1;
                }
                break;
            case 'c':
                if (string_to_uint32_t(optarg, &cycles)) {
                    fprintf(stderr, "Invalid value for --cycles. Must be a decimal number between 0 and 4294967295.\n");
                    return 1;
                }
                break;
            case 'e':
                tracefile = optarg;
                break;
            case 'h':
                print_help(programName);
                return 0;
            case 't':
                run_tests();
                return 0;
            default:
                print_usage_msg(programName);
                return 1;
        }
    }

    if (optind < argc) {
        inputFile = argv[optind]; // first positional argument is <file>
    } else {
        fprintf(stderr, "Invalid options %s: Missing input file\n", programName);
        print_usage_msg(programName);
        return_code = 1;
        goto cleanup;
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

    // check if csv file name is legal
    if (!is_valid_filename(inputFile, programName, "csv file")) {
        return_code = 1;
        goto cleanup;
    }

    // check if tracefile name is legal
    if (!is_valid_filename(tracefile, programName, "Tracefile")) {
        return_code = 1;
        goto cleanup;
    }

    // check if the number of lines in each level can be divided by num-lines-per-set for each layer (for set-associative caches)
    if (mappingStrategy == 2 && (numLinesL1 % NUM_LINES_PER_SET + numLinesL2 % NUM_LINES_PER_SET + numLinesL3 % NUM_LINES_PER_SET != 0)) {
        fprintf(stderr, "Invalid options %s: the number of lines in each level must be a multiple of NUM_LINES_PER_SET\n", programName);
        return_code = 1;
        goto cleanup;
    }

    // check if lines of L1 <= L2 <= L3 (necessary to ensure inclusivity)
    if ((numLinesL1 > numLinesL2 && numCacheLevels >= 2) || (numLinesL2 > numLinesL3 && numCacheLevels >= 3)) {
        fprintf(stderr, "Invalid options %s: L1 should have less cache lines than L2 and L2 less than L3\n", programName);
        return_code = 1;
        goto cleanup;
    }

    // check if all levels have enough lines for at least one set (if set-associative), else at least one line
    if (numLinesL1 < 1 || (mappingStrategy == 2 && numLinesL1 < NUM_LINES_PER_SET) ||
        numLinesL2 < 1 || (mappingStrategy == 2 && numLinesL2 < NUM_LINES_PER_SET) ||
        numLinesL3 < 1 || (mappingStrategy == 2 && numLinesL3 < NUM_LINES_PER_SET)) {
        fprintf(stderr, "Invalid options %s: not all levels have enough cache lines\n", programName);
        return_code = 1;
        goto cleanup;
    }

    if (cacheLineSize == 0) {
        fprintf(stderr, "size of cache line can't be zero");
        return_code = 1;
        goto cleanup;
    }

    int numRequests;

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
                                            numRequests, requests);

    printf("Simulation Results:\n");
    printf("Cycles: %u\n", simu_res.cycles);
    printf("Hits: %u\n", simu_res.hits);
    printf("Misses: %u\n", simu_res.misses);

    cleanup:
        if (requests) {
            free(requests);
        }
    return return_code;
}
