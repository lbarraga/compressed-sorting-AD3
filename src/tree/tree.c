//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include "make_OPC.h"
#include "hu_tucker.h"

#define MAX_CHAR_COUNT 128

void fillFrequencyTable(FILE* file, int* frequencyTable) {
    int ch = fgetc(file);
    while (ch != EOF) {
        frequencyTable[ch]++;
        ch = fgetc(file);
    }
}

void tree(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    int frequencyTable[MAX_CHAR_COUNT];

    for (int i = 0; i < MAX_CHAR_COUNT; i++) {
        frequencyTable[i] = 0;
    }

    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    printf("%s\n", inputFilePath);
    fillFrequencyTable(inputFile, frequencyTable);
    makeHT_OPC(frequencyTable, outputFile);

}
