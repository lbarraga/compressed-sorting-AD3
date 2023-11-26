//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

void sort(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    return;
    FILE* inputFile = fopen(inputFilePath, "w");

    uint64_t* inputBuffer = malloc(bufferSize);
    fread(inputBuffer, sizeof(uint64_t), sizeof(inputBuffer), inputFile);
}
