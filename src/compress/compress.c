//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "parse_tree_file.h"
#include "compress_file.h"

int calculateLineAmount(FILE* file) {
    int amount = 0;
    int ch = fgetc(file);
    while (ch != EOF) {
        amount += ch == '\n';
        ch = fgetc(file);
    }
    fseek(file, 0, SEEK_SET);
    return amount;
}

void compress(const char *inputFile, const char *outputFile, int bufferSize, const char *treeFilePath) {
    FILE* file = fopen(treeFilePath, "r");

    int charCount = calculateLineAmount(file);
    uint64_t* chars = malloc(sizeof(uint64_t) * charCount);
    uint64_t* frequencies = malloc(sizeof(uint64_t) * charCount);
    unsigned char** codes = malloc(sizeof(unsigned char*) * charCount);

    parseTreeFile(chars, frequencies, codes, charCount, file);
    compressFile(inputFile, outputFile, bufferSize, chars, frequencies, codes);

    free(chars);
    free(frequencies);
    for (int i = 0; i < charCount; ++i) {
        free(codes[i]);
    }
    free(codes);
}
