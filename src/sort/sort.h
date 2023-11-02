//
// Created by lukasbt on 10/14/23.
//

#ifndef PROJECTAD3_SORT_H
#define PROJECTAD3_SORT_H

#include <bits/types/FILE.h>
#include "../compress/parse_tree_file.h"

typedef struct LineInterval {
    long start;
    int length;
} LineInterval;

void sort(const char *inputFile, const char *outputFile, int bufferSize);

#endif //PROJECTAD3_SORT_H
