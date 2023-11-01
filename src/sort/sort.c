//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "sort.h"

int readNBitsAsInt(int n, unsigned char* byteBuffer, int* bitsReadFromBuffer, FILE* file) {
    int isLastByte = 1; // TODO
    int result = 0;
    for (int _ = 0; _ < n; ++_) {
        if (*bitsReadFromBuffer == 8) {
            fread(byteBuffer, 1, 1, file);
            (*bitsReadFromBuffer) = 0;
        }
        int bit = (*byteBuffer >> (7 - *bitsReadFromBuffer)) & 1;
        result = (result << 1) | bit;
        (*bitsReadFromBuffer)++;
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
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");
    FILE* sortTemp = fopen("/home/lukasbt/projectAD3/data/sort.temp", "rw");

    long headerPosition = consumeLong(inputFile);
    uint8_t lastByteBitCount = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte

    fseek(inputFile, headerPosition, SEEK_SET);
    printf("header pos: %li\n", headerPosition);

    // skip the tree
    skipTree(inputFile);

    unsigned char byteBuffer;
    int bitReadFromBuffer = 8;

    for (int i = 0; i < 5; ++i) {
        int ll = readNBitsAsInt(5, &byteBuffer, &bitReadFromBuffer, inputFile);
        int l = readNBitsAsInt(ll, &byteBuffer, &bitReadFromBuffer, inputFile);

        printf("%d -> %d\n", ll, l + 1);
    }







//    size_t bytesRead;
//    unsigned char* buffer = malloc(bufferSize);
//    while ((bytesRead = fread(buffer, 1, bufferSize, inputFile)) > 0) {
//
//    }


    // Loop over the buffer and make the array of indexes

}
