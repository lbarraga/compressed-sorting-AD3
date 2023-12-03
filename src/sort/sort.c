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

    printf("start = %ld, length = %d\n", lineInterval->start, lineInterval->length);
    printf("s / 64 = %ld en e / 64 = %ld\n", s / 64, e / 64);
    int toTake = 64 - (int) (s % 64);
    while (s / 64 != e / 64) {
        printf("in, toTake = %d\n", toTake);

        uint64_t extracted = extractInterval(bits[s / 64], 64 - toTake ,toTake);
        printUint64t(extracted);
        outputNBits(handler, extracted, toTake);
        s += toTake;
        toTake = 64;
    }

    int last = (int) (e - s);
    if (last == 0) {
        return;
    }

    printf("e - s = %lu\n", e-s);
    uint64_t extracted = extractInterval(bits[s / 64], (int) s % 64, last);
    printUint64t(extracted);
    outputNBits(handler, extracted, last);
}

void outputLineInterval2(const LineInterval* lineInterval, uint64_t* bits, BitOutputHandler* handler) {
    long s = lineInterval->start;
    long e = lineInterval->start + lineInterval->length;

    while (s != e) {
        uint64_t bit = getBit(bits[s / 64], s % 64);
        outputNBits(handler, bit, 1);
        s++;
    }
}

// TODO hier kan je ook stoppen wanneer de intervallen over de helft gaan zitten.
BlockCalculation calculateAmountOfLinesInBlock(long totalLinesRead, long startBits, long totalLines, size_t maxBitsInBlock, BitInputHandler* headerHandler) {
    // Read through the lineLengths to determine the required number of Interval structs to allocate.

    InputHandlerPosition position = getInputPosition(headerHandler);

    BlockCalculation calc = {.amountOfLines = 0, .bits = startBits};
    int length = readLength(headerHandler);
    while (totalLinesRead + calc.amountOfLines < totalLines && calc.bits + length < (long) maxBitsInBlock) {
        //printf("readLength = %d\n", length);
        calc.bits += length;
        calc.amountOfLines++;
        length = readLength(headerHandler);
    }

    setAtInputPosition(headerHandler, &position); // Functie moet idempotent blijven

    return calc;
}

long sortBlock(uint64_t* bits, long start, BitInputHandler* headerHandler, BitOutputHandler* outputHandler, long linesInBlock) {

    LineInterval* lineIntervals = malloc(sizeof(LineInterval) * linesInBlock);
    //printf("lines in block = %lu\n", linesInBlock);
    // Maak een struct voor elke lijn.
    for (int i = 0; i < linesInBlock; ++i) {
        int lineLength = readLength(headerHandler);
        lineIntervals[i].start = start;
        lineIntervals[i].length = lineLength;
        start += lineLength;
    }

//    printf("===========================\n");
//    for (int i = 0; i < linesInBlock; ++i) {
//        printf("start = %lu, length = %d\n", lineIntervals[i].start, lineIntervals[i].length);
//    }
//    printf("===========================\n");

    // Sorteer de structs
    heapSort(lineIntervals, linesInBlock, bits);

//    printf("===========================\n");
//    for (int i = 0; i < linesInBlock; ++i) {
//        printf("start = %lu, length = %d\n", lineIntervals[i].start, lineIntervals[i].length);
//    }
//    printf("===========================\n");

    // Schrijf de gesorteerde structs uit naar het tijdelijke blok bestand.
    for (int i = 0; i < linesInBlock; ++i) {
        outputLineInterval2(&lineIntervals[i], bits, outputHandler);
    }

    free(lineIntervals);

    return start;
}

void sort(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    // TODO het kan zijn da die consumeWhitespace op het einde van de tree een stuk van de uint64 opeet.
    FILE* inputFile = fopen(inputFilePath, "rb");
    FILE* headerPointer = fopen(inputFilePath, "rb");
    FILE* blockTempFile = tmpfile();
    FILE* outputFile = fopen(outputFilePath, "wb");

    long headerPosition = consumeLong(inputFile);
    long totalLines = consumeLong(inputFile);
    uint8_t significantBitsCodes = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    uint8_t significantBitsHeader = consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract

    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    skipTree(headerPointer);
    long lineLengthsStart = ftell(headerPointer); // TODO zou uiteindelijk weg moeten

    BitInputHandler inputHandler = createBitInputHandler(inputFile, 8);
    BitInputHandler headerHandler = createBitInputHandler(headerPointer, 8);

    BitOutputHandler outputHandler = createOutputHandler(outputFile, 8);
    BitOutputHandler blokFileOutputHandler = createOutputHandler(blockTempFile, 8);

    printf("the header position is %lu\n", headerPosition);
    long nBlockPointers = 0;
//    BlockPointer* blockPointers = malloc(sizeof(BlockPointer) * 1);

    // uitschrijven naar de file TODO dit moet eigenlijk pas op het einde gebeuren net voor de merge
    // Fill in padding with info
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerPosition, sizeof(long), 1, outputFile);
    fwrite(&totalLines, sizeof(long), 1, outputFile);

    // amount of significant bits in the last byte
    fwrite(&significantBitsCodes, sizeof(uint8_t), 1, outputFile);
    fwrite(&significantBitsHeader, sizeof(uint8_t), 1, outputFile);

    // Stap 1: laad nItems geheugen in.
    uint64_t* inputBuffer = malloc(bufferSize);

    size_t nItems = bufferSize / sizeof(uint64_t);
    size_t maxBitsInBlock = nItems * 64;

    printf("total lines: %ld\n", totalLines);

    long totalLinesRead = 0;
    long start = 0;
    BlockCalculation blockCalc = calculateAmountOfLinesInBlock(totalLinesRead, start, totalLines, maxBitsInBlock, &headerHandler);
    while (blockCalc.amountOfLines > 0) {
        //printf("lines in next block = %lu, bits=%lu, start= %lu\n", blockCalc.amountOfLines, blockCalc.bits, start);
        fread(inputBuffer, sizeof(uint64_t), blockCalc.bits / 64 + 1, inputFile);
        totalLinesRead += blockCalc.amountOfLines;
        sortBlock(inputBuffer, start, &headerHandler, &outputHandler, blockCalc.amountOfLines);
        fseek(inputFile, -8, SEEK_CUR);
        start = blockCalc.bits % 64;
        blockCalc = calculateAmountOfLinesInBlock(totalLinesRead, start, totalLines, maxBitsInBlock, &headerHandler);
    }

    flushBits(&outputHandler);
    printf("start = %ld, num lines: %ld\n", start, totalLines);

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
    freeOutputHandler(&blokFileOutputHandler);

    free(inputBuffer);
    free(tree);

    fclose(inputFile);
    fclose(outputFile);

}