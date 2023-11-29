//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "../IO/bit_input.h"
#include "../IO/bit_input/bit_io.h"
#include "heap_sort.h"

void skipTree(FILE* inputFile) {
    int charCount = consumeInt(inputFile);
    consumeWhiteSpace(inputFile);
    for (int _ = 0; _ < charCount * 2; ++_) {
        consumeInt(inputFile);
        consumeWhiteSpace(inputFile);
    }
}



void outputLineInterval(const LineInterval* lineInterval, const uint64_t* bits, BitOutputHandler* handler) {
    long s = lineInterval->start;
    long e = lineInterval->start + lineInterval->length;

    if (lineInterval->start == 0) {
        printf("!! start = %ld, length = %d\n", lineInterval->start, lineInterval->length);
    }

    int toTake = 64 - (int) (s % 64);
    while (s / 64 != e / 64) {
        if (lineInterval->start == 0) {
            printf("!! in, toTake = %d\n", toTake);
        }
        uint64_t extracted = extractInterval(bits[s / 64], 64 - toTake ,toTake);
        if (lineInterval->start == 0) {
            printUint64t(bits[s / 64]);
            printUint64t(extracted);
        }
        outputNBits(handler, extracted, toTake);
        s += toTake;
        toTake = 64;
    }

    uint64_t extracted = extractInterval(bits[s / 64], (int) s % 64, (int) (e - s));
    outputNBits(handler, extracted, (int) (e - s));
}

void sort(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    // TODO het kan zijn da die consumeWhitespace op het einde van de tree een stuk van de uint64 opeet.
    FILE* inputFile = fopen(inputFilePath, "rb");
    FILE* outputFile = fopen(outputFilePath, "wb");
    FILE* headerPointer = fopen(inputFilePath, "rb");

    long headerPosition = consumeLong(inputFile);
    long amountOfLines = consumeLong(inputFile);
    uint8_t significantBitsCodes = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    uint8_t significantBitsHeader = consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract

    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    skipTree(headerPointer);
    long lineLengthsStart = ftell(headerPointer);

    BitInputHandler inputHandler = createBitInputHandler(inputFile, 8);
    BitInputHandler headerHandler = createBitInputHandler(headerPointer, 8);

    printf("the header position is %lu\n", headerPosition);

    // Stap 1: laadt m geheugen in.
    uint64_t* inputBuffer = malloc(bufferSize);
    size_t uint64sRead = fread(inputBuffer, sizeof(uint64_t), bufferSize / sizeof(uint64_t), inputFile);


    // Read through the lineLengths to determine the required number of Interval structs to allocate.
    // Stop when we have read more than `bufferSize` * 8 bits or there are no more lines to read.
    long start = 0;
    long lines = 0;
    while (lines < amountOfLines && start < (long) uint64sRead * 64) {
        start += readLength(&headerHandler) + 1; // TODO uitleggen waarom +1
        lines += 1;
    }

    LineInterval* lineIntervals = malloc(sizeof(LineInterval) * lines);
    setInputHandlerAt(&headerHandler, lineLengthsStart);

    // Stap 2: maak een struct voor elke lijn.
    start = 0;
    for (int i = 0; i < lines; ++i) {
        int lineLength = readLength(&headerHandler) + 1;
        lineIntervals[i].start = start;
        lineIntervals[i].length = lineLength;
        start += lineLength;
    }

    for (int i = 0; i < lines; ++i) {
        printf("line %d: start: %ld, length: %d\n", i, lineIntervals[i].start, lineIntervals[i].length);
    }

    // stap 3: sorteer de structs
    heapSort(lineIntervals, lines, inputBuffer);

    printf("=========================\n");
    for (int i = 0; i < lines; ++i) {
        printf("line %d: start: %ld, length: %d\n", i, lineIntervals[i].start, lineIntervals[i].length);
    }

    // stap 4: uitschrijven naar de file
    // Fill in padding with info
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerPosition, sizeof(long), 1, outputFile);
    fwrite(&amountOfLines, sizeof(long), 1, outputFile);

    // amount of significant bits in the last byte
    fwrite(&significantBitsCodes, sizeof(uint8_t), 1, outputFile);
    fwrite(&significantBitsHeader, sizeof(uint8_t), 1, outputFile);


    BitOutputHandler outputHandler = createOutputHandler(outputFile, 64);
    for (int i = 0; i < lines; ++i) {
        outputLineInterval(&lineIntervals[i], inputBuffer, &outputHandler);
    }

    flushBits(&outputHandler);
    printf("start = %ld, num lines: %ld\n", start, amountOfLines);

    // Tree
    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    unsigned char* tree = malloc(sizeof(unsigned char) * (lineLengthsStart - headerPosition));
    fread(tree, sizeof(unsigned char), lineLengthsStart - headerPosition, headerPointer);

    for (int i = 0; i < lineLengthsStart - headerPosition; ++i) {
        fprintf(outputFile,"%c", tree[i]);
    }
    printf("\n");

    // Cleanup
    freeBitInputHandler(&headerHandler);
    freeBitInputHandler(&inputHandler);
    freeOutputHandler(&outputHandler);

    free(inputBuffer);
    free(lineIntervals);
    free(tree);

    fclose(inputFile);
    fclose(outputFile);

}