//
// Created by julian on 1/13/25.
//

#ifndef PROJECT_CV_PARSER_H
#define PROJECT_CV_PARSER_H

#include <stddef.h>

struct Request* parse_csv(const char* filename, int* numRequests);

#endif //CSV_PARSER_H
