//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "../IO/bit_input.h"
#include "../IO/bit_input/bit_io.h"

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

    FILE* inputFile = fopen(inputFilePath, "rb");
    FILE* outputFile = fopen(outputFilePath, "wb");
    FILE* headerPointer = fopen(inputFilePath, "rb");


    long headerPosition = consumeLong(inputFile);
    long amountOfLines = consumeLong(inputFile);
    consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract

    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    skipTree(headerPointer);

    BitInputHandler inputHandler = createBitInputHandler(inputFile, 8);
    BitInputHandler headerHandler = createBitInputHandler(headerPointer, 8);

    printf("the header position is %lu\n", headerPosition);

    // Stap 1: laadt m geheugen in.
    uint64_t* inputBuffer = malloc(bufferSize);
    size_t uint64sRead = fread(inputBuffer, sizeof(uint64_t), bufferSize / sizeof(uint64_t), inputFile);


    long start = 0;
    for (int i = 0; i < 4; ++i) {
        uint64_t ll = readNBits(&headerHandler, 5);
        uint64_t l = readNBits(&headerHandler, ll);
        LineInterval interval = {
                .start = start,
                .length = ll
        };

        printf("ll = %lu\n", ll);
        printf("l = %lu\n", l);
    }

    // Stap 2: maak een struct voor elke lijn.



    // Cleanup
    freeBitInputHandler(&headerHandler);
    freeBitInputHandler(&inputHandler);

    fclose(inputFile);
    fclose(outputFile);

}
