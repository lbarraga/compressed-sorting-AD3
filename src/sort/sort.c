//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "../IO/bit_input.h"
#include "../IO/bit_input/bit_io.h"
#include "heap_sort.h"

int readNextBit(unsigned char* byteBuffer, int* bitsReadFromBuffer, FILE* file) {
    if (*bitsReadFromBuffer == 8) {
        fread(byteBuffer, 1, 1, file);
        (*bitsReadFromBuffer) = 0;
    }
    int bit = (*byteBuffer >> (7 - *bitsReadFromBuffer)) & 1;
    (*bitsReadFromBuffer)++;
    return bit;
}

int readNBitsAsInt(int n, unsigned char* byteBuffer, int* bitsReadFromBuffer, FILE* file) {
    int result = 0;
    for (int _ = 0; _ < n; ++_) {
        int bit = readNextBit(byteBuffer, bitsReadFromBuffer, file);
        result = (result << 1) | bit;

    }
    return result;
}

void skipTree(FILE* inputFile) {
    int charCount = consumeInt(inputFile);
    consumeWhiteSpace(inputFile);
    for (int _ = 0; _ < charCount * 2; ++_) {
        consumeInt(inputFile);
        consumeWhiteSpace(inputFile);
    }
}



void sort(const char *inputFilePath, const char *outputFilePath, int bufferSize) {

    testHeap();

    return;
    FILE* inputFile = fopen(inputFilePath, "rb");
    FILE* outputFile = fopen(outputFilePath, "wb");
    FILE* headerPointer = fopen(inputFilePath, "rb");


    long headerPosition = consumeLong(inputFile);
    long amountOfLines = consumeLong(inputFile);
    consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract

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
    while (lines < amountOfLines && start < bufferSize * 8) {
        start += readLength(&headerHandler);
        lines += 1;
    }

    LineInterval* lineIntervals = malloc(sizeof(LineInterval) * lines);
    setInputHandlerAt(&headerHandler, lineLengthsStart);

    // Stap 2: maak een struct voor elke lijn.
    start = 0;
    for (int i = 0; i < lines; ++i) {
        int lineLength = readLength(&headerHandler);
        lineIntervals[i].start = start;
        lineIntervals[i].length = lineLength;
        start += lineLength;
    }

    for (int i = 0; i < lines; ++i) {
        printf("line %d: start: %ld, length: %d\n", i, lineIntervals[i].start, lineIntervals[i].length);
    }

    // stap 3: sorteer de structs



    printf("start = %ld, num lines: %ld\n", start, amountOfLines);





    // Cleanup
    freeBitInputHandler(&headerHandler);
    freeBitInputHandler(&inputHandler);
    free(inputBuffer);

    fclose(inputFile);
    fclose(outputFile);

}
