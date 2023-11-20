//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

FILE* globalInputFileA = NULL;
FILE* globalInputFileB = NULL;

int readNextBit(unsigned char* byteBuffer, int* bitsReadFromBuffer, FILE* file) {
    if (*bitsReadFromBuffer == 8) {
        if (fread(byteBuffer, 1, 1, file)) {} // TODO
        (*bitsReadFromBuffer) = 0;
    }
    int bit = (*byteBuffer >> (7 - *bitsReadFromBuffer)) & 1;
    (*bitsReadFromBuffer)++;
    return bit;
}

/// TODO dit in een utils file steken
void outputBit2(int bit, unsigned char* byteBuffer, int* currentBitIndexBuffer, FILE* outputFile) {
    if (*currentBitIndexBuffer == 8) {
        fputc(*byteBuffer, outputFile);
        *byteBuffer = 0;
        *currentBitIndexBuffer = 0;
    }
    *byteBuffer |= bit << (7 - *currentBitIndexBuffer);
    (*currentBitIndexBuffer)++;
}

/// TODO per byte gaat eigenlijk veel sneller
int compareLineIntervals(const void *a, const void *b) {
    LineInterval* intervalA = (LineInterval*) a;
    LineInterval* intervalB = (LineInterval*) b;

    long startByteA = intervalA->start / 8;
    long startByteB = intervalB->start / 8;
    long startBitA = intervalA->start % 8;
    long startBitB = intervalB->start % 8;
    int lengthA = intervalA->length;
    int lengthB = intervalB->length;

    int bitReadFromBufferA = 8;
    int bitReadFromBufferB = 8;

    unsigned char byteBufferA;
    unsigned char byteBufferB;

    fseek(globalInputFileA, startByteA, SEEK_SET);
    fseek(globalInputFileB, startByteB, SEEK_SET);

    // Skip to startBit
    for (int k = 0; k < startBitA; ++k) {
        readNextBit(&byteBufferA, &bitReadFromBufferA, globalInputFileA);
    }
    for (int k = 0; k < startBitB; ++k) {
        readNextBit(&byteBufferB, &bitReadFromBufferB, globalInputFileB);
    }

    // Compare the bits
    for (int i = 0; i < lengthA && i < lengthB; ++i) {
        int bitA = readNextBit(&byteBufferA, &bitReadFromBufferA, globalInputFileA);
        int bitB = readNextBit(&byteBufferB, &bitReadFromBufferB, globalInputFileB);

        if (bitA > bitB) return 1;
        if (bitA < bitB) return -1;
    }
    return lengthA - lengthB;
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
    printf("%s%d", outputFilePath, bufferSize);
    FILE* inputFile = fopen(inputFilePath, "r");
    globalInputFileA = fopen(inputFilePath, "r");
    globalInputFileB = fopen(inputFilePath, "r");
    //FILE* outputFile = fopen(outputFilePath, "w");
    FILE* sortTemp = fopen("/home/lukasbt/projectAD3/data/sort.temp", "w");

    long headerPosition = consumeLong(inputFile);
    long amountOfNewLines = consumeLong(inputFile);
    // amount of bits that are not filler bits in the last byte for encoding and for header
    uint8_t lastByteBitCountEncodings = consumeUint8_t(inputFile);
    uint8_t lastByteBitCountHeader = consumeUint8_t(inputFile);

    fseek(inputFile, headerPosition, SEEK_SET);
    skipTree(inputFile);

    unsigned char byteBuffer;
    int bitReadFromBuffer = 8;

    LineInterval* lineIntervals = malloc(sizeof(LineInterval) * amountOfNewLines);

    long startIndex = (8 + 8 + 1 + 1) * 8;
    for (int i = 0; i < amountOfNewLines; ++i) {

        int ll = readNBitsAsInt(5, &byteBuffer, &bitReadFromBuffer, inputFile);
        int l = readNBitsAsInt(ll, &byteBuffer, &bitReadFromBuffer, inputFile) + 1;

        lineIntervals[i].start = startIndex;
        lineIntervals[i].length = l;
        startIndex += l;
    }

    printf("starting sort...\n");
    qsort(lineIntervals, amountOfNewLines, sizeof(LineInterval), compareLineIntervals);
    printf("sorted...\n");

    // Write the lines in order of the sort to the temporary sorted file.
    fwrite(&headerPosition, sizeof(long), 1, sortTemp); // 64 bits for pointer to header, which is placed at the end of the compressed file
    fwrite(&amountOfNewLines, sizeof(long), 1, sortTemp); // Amount of lines in the file.
    fwrite(&lastByteBitCountEncodings, sizeof(uint8_t), 1, sortTemp); // amount of significant bits in last byte of encodings
    fwrite(&lastByteBitCountHeader, sizeof(uint8_t), 1, sortTemp); // amount of significant bits in last byte of header

    unsigned char outputBuffer = 0;
    int bitsWrittenInByte = 0;
    for (int i = 0; i < amountOfNewLines; ++i) {
        LineInterval interval = lineIntervals[i];
        fseek(inputFile, interval.start / 8, SEEK_SET);
        bitReadFromBuffer = 8;
        for (int j = 0; j < interval.start % 8; ++j) {
            readNextBit(&byteBuffer, &bitReadFromBuffer, inputFile);
        }
        for (int j = 0; j < interval.length; ++j) {
            int bit = readNextBit(&byteBuffer, &bitReadFromBuffer, inputFile);
            outputBit2(bit, &outputBuffer, &bitsWrittenInByte, sortTemp);
        }
    }

    if (bitsWrittenInByte > 0) {
        fputc(outputBuffer, sortTemp); // flush
    }

    // Copy the header
    fseek(inputFile, headerPosition, SEEK_SET);
    int ch = fgetc(inputFile);
    while (ch != EOF) {
        fputc(ch, sortTemp);
        ch = fgetc(inputFile);
    }

    free(lineIntervals);
}
