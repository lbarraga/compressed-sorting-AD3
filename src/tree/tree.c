//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>

void fillFrequencyTable(FILE* file, int* frequencyTable) {
    int ch = fgetc(file);
    while (ch != EOF) {
        frequencyTable[ch]++;
        ch = fgetc(file);
    }
}

void tree(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    int MAX_CHAR_COUNT = 128;

    int frequencyTable[MAX_CHAR_COUNT];

    for (int i = 0; i < MAX_CHAR_COUNT; i++) {
        frequencyTable[i] = 0;
    }

    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    fillFrequencyTable(inputFile, frequencyTable);

    for (int i = 0; i < MAX_CHAR_COUNT; i++) {
        if (frequencyTable[i] != 0) {
            fprintf(outputFile, "%d %d\n", i, frequencyTable[i]);
        }
    }
}
