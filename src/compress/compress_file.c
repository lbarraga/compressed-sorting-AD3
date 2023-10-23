//
// Created by lukasbt on 10/23/23.
//

#include <stdint-gcc.h>
#include <bits/types/FILE.h>
#include <stdio.h>

int findCharIndex(const uint64_t* chars, int charToFind) {
    int index = 0;
    while (chars[index] != (uint64_t) charToFind) {
        index++;
    }
    return index;
}

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize,
                  uint64_t* chars, uint64_t* frequencies, unsigned char** codes) {
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");

    int ch = fgetc(inputFile);
    while (ch != EOF) {
        fprintf(outputFile, "%s", codes[findCharIndex(chars, ch)]);
        ch = fgetc(inputFile);
    }
}