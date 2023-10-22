//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
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

int countChars(const int * frequencyTable) {
    int amount = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            amount++;
        }
    }
    return amount;
}

void fillBuffers(int* charBuffer, int* freqs, const int* frequencyTable) {
    int index = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            charBuffer[index] = i;
            freqs[index] = frequencyTable[i];
            index++;
        }
    }
}

void tree(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    int frequencyTable[MAX_CHAR_COUNT];

    for (int i = 0; i < MAX_CHAR_COUNT; i++) {
        frequencyTable[i] = 0;
    }

    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    fillFrequencyTable(inputFile, frequencyTable);

    int charCount = countChars(frequencyTable);
    int* charBuffer = malloc(sizeof(int) * charCount); // all present characters. e.g. ['A', 'B', 'C', 'D']
    int* freqBuffer = malloc(sizeof(int) * charCount); // frequencies of all present characters, e.g. [4, 2, 3, 4]
    fillBuffers(charBuffer, freqBuffer, frequencyTable);

    makeHT_OPC(charBuffer, freqBuffer, charCount, outputFile);

    free(charBuffer);
    free(freqBuffer);

}
