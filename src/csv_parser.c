//
// Created by julian on 1/13/25.
//

#include "csv_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"

// Function to parse a csv file and return the requests
#define MAX_LINE_LENGTH 1024

struct Request* parse_csv(const char* filename, int* numRequests) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    struct Request* requests = NULL;
    *numRequests = 0;

    while (fgets(line, sizeof(line), file)) {
        char* token;
        struct Request request;

        // Parse operation type
        token = strtok(line, ",");
        if (token == NULL) {
            fprintf(stderr, "Invalid format: Missing operation type\n");
            free(requests);
            fclose(file);
            exit(1);
        }
        if (strcmp(token, "R") == 0) {
            request.we = 0;
        } else if (strcmp(token, "W") == 0) {
            request.we = 1;
        } else {
            fprintf(stderr, "Invalid format: Invalid operation type\n");
            free(requests);
            fclose(file);
            exit(1);
        }

        // Parse address
        token = strtok(NULL, ",");
        if (token == NULL) {
            fprintf(stderr, "Invalid format: Missing address\n");
            free(requests);
            fclose(file);
            exit(1);
        }
        request.addr = strtoul(token, NULL, 0);

        // Parse data
        token = strtok(NULL, ",");
        // if it is a write request, data is required
        if (request.we == 1 && token == NULL) {
            fprintf(stderr, "Invalid format: Missing data\n");
            free(requests);
            fclose(file);
            exit(1);
        }
        if (request.we == 1) {
            request.data = strtoul(token, NULL, 0);
        } else {
            request.data = 0;
        }

        // if it is a read request, data has to be 0
        // empty string is also considered as 0
        if (request.we == 0 && (token != NULL && strcmp(token, "") == 0)) {
            fprintf(stderr, "Invalid format: Data has to be 0 for read requests\n");
            free(requests);
            fclose(file);
            exit(1);
        }

        // increase the number of requests
        (*numRequests)++;
        requests = realloc(requests, *numRequests * sizeof(struct Request));
        if (requests == NULL) {
            fprintf(stderr, "Could not allocate memory for requests\n");
            fclose(file);
            exit(1);
        }

        requests[*numRequests - 1] = request;
    }

    fclose(file);
    return requests;
}
