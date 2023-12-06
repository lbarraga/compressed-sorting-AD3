//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "hu_tucker.h"

#define MAX_CHAR_COUNT 128

// TODO geen int maar uint64t
void fillFrequencyTable(FILE* file, int* frequencyTable, int m) {
    long bufferSize = m / 2;
    unsigned char* buffer = malloc(bufferSize);
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufferSize, file)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            unsigned char ch = buffer[i];
            frequencyTable[ch]++;
        }
    }
    free(buffer);
}

int countChars(const int * frequencyTable) {
    int amount = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            amount++;
        }
    }
    return amount;
}

void fillBuffers(uint8_t * charBuffer, uint64_t* freqs, const int* frequencyTable) {
    int index = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            charBuffer[index] = i;
            freqs[index] = frequencyTable[i];
            index++;
        }
    }
}

void treeWithOutputFilePointer(const char *inputFilePath, FILE* outputFile, int bufferSize) {
    int frequencyTable[MAX_CHAR_COUNT];

    for (int i = 0; i < MAX_CHAR_COUNT; i++) {
        frequencyTable[i] = 0;
    }

    FILE* inputFile = fopen(inputFilePath, "r");

    fillFrequencyTable(inputFile, frequencyTable, bufferSize);

    int charCount = countChars(frequencyTable);
    uint8_t * charBuffer = malloc(sizeof(int) * charCount); // all present characters. e.g. ['A', 'B', 'C', 'D']
    uint64_t* freqBuffer = malloc(sizeof(uint64_t) * charCount); // frequencies of all present characters, e.g. [4, 2, 3, 4]
    fillBuffers(charBuffer, freqBuffer, frequencyTable);

    makeHT_OPC(charBuffer, freqBuffer, charCount, outputFile);

    free(charBuffer);
    free(freqBuffer);

    fclose(inputFile);
}

void tree(const char *inputFilePath, const char* outputFilePath, int bufferSize) {
    FILE* outputFile = fopen(outputFilePath, "w");
    treeWithOutputFilePointer(inputFilePath, outputFile, bufferSize);
    fclose(outputFile);
}
