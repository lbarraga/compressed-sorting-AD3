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

typedef struct BlockPointer {
    size_t index; // In welke uint64 het blok begint.
    int bit; // Op welke bit in de index uint64 het blok begint
} BlockPointer;

typedef struct BlockCalculation {
    long bits; // the full amount of bits contained in the block (can differ because last uint64 may not contain full line)
    long amountOfLines; // The max amount of lines that could fit in the block
} BlockCalculation;

void sort(const char *inputFile, const char *outputFile, int bufferSize);

#endif //PROJECTAD3_SORT_H
