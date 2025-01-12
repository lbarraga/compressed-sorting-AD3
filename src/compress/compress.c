//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "parse_tree_file.h"
#include "compress_file.h"
#include "../tree/tree.h"

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

void compress(const char *inputFile, const char *outputFile, int bufferSize, char *treeFilePath) {

    FILE* file;
    if (treeFilePath == NULL) {
        file = tmpfile();
        treeWithOutputFilePointer(inputFile, file, bufferSize);
        fseek(file, 0, SEEK_SET);
    } else {
        file = fopen(treeFilePath, "r");
    }


    int charCount = calculateLineAmount(file);
    uint64_t* frequencies = malloc(sizeof(uint64_t) * charCount);
    OPC** codes = calloc(128, sizeof(OPC*));

    parseTreeFile(frequencies, codes, charCount, file);
    compressFile(inputFile, outputFile, bufferSize, codes, charCount);

    fclose(file);

    free(frequencies);
    for (int i = 0; i < 128; ++i) {
        free(codes[i]);
    }
    free(codes);
}
