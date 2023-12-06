//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "../IO/bit_input.h"
#include "../IO/bit_input/bit_io.h"
#include "heap_sort.h"
#include "../tree/linkedList/linkedlist.h"
#include "block_merge.h"

void copyFile(FILE *inputFile, FILE *outputFile, size_t bufferSize) {
    unsigned char* buffer = malloc(bufferSize);

    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufferSize, inputFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outputFile);
    }

    free(buffer);
}

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

    int toTake = 64 - (int) (s % 64);
    while (s / 64 != e / 64) {
        uint64_t extracted = extractInterval(bits[s / 64], 64 - toTake ,toTake);
        outputNBits(handler, extracted, toTake);
        s += toTake;
        toTake = 64;
    }

    int last = (int) (e - s);
    if (last == 0) {
        return;
    }

    uint64_t extracted = extractInterval(bits[s / 64], (int) s % 64, last);
    outputNBits(handler, extracted, last);
}

void outputLineIntervalBitPerBit(const LineInterval* lineInterval, uint64_t* bits, BitOutputHandler* handler) {
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
        calc.bits += length;
        calc.amountOfLines++;
        length = readLength(headerHandler);
    }

    setAtInputPosition(headerHandler, &position); // Functie moet idempotent blijven

    return calc;
}

Node* calculateLinesInBlocks(BitInputHandler* headerHandler, long totalLines, size_t maxBitsInBlock) {
    Node* linesInBlocks = initEmptyLinkedList();

    long totalLinesRead = 0;
    long bits = 0;

    while (totalLinesRead < totalLines) {
        int length = readLength(headerHandler);
        int nLinesInBlock = 0;
        while (totalLinesRead + nLinesInBlock < totalLines && bits + length < (long) maxBitsInBlock) {
            bits += length;
            nLinesInBlock++;
            length = readLength(headerHandler);
        }
        append(&linesInBlocks, nLinesInBlock);
        totalLinesRead += nLinesInBlock;
        bits %= 64;
    }

    return linesInBlocks;
}

long sortBlock(uint64_t* bits, long start, BitInputHandler* headerInputHandler, BitOutputHandler* outputHandler, BitOutputHandler* headerOutputHandler, long linesInBlock) {

    LineInterval* lineIntervals = malloc(sizeof(LineInterval) * linesInBlock);

    // Maak een struct voor elke lijn.
    for (int i = 0; i < linesInBlock; ++i) {
        int lineLength = readLength(headerInputHandler);
        lineIntervals[i].start = start;
        lineIntervals[i].length = lineLength;
        start += lineLength;
    }

    // Sorteer de structs
    heapSort(lineIntervals, linesInBlock, bits);

    // Schrijf de gesorteerde structs uit naar het tijdelijke blok bestand.
    for (int i = 0; i < linesInBlock; ++i) {
        outputLineIntervalBitPerBit(&lineIntervals[i], bits, outputHandler);
        outputLength(headerOutputHandler, lineIntervals[i].length);
    }

    free(lineIntervals);

    return start;
}

void sort(const char *inputFilePath, const char *outputFilePath, int m) {
    // TODO het kan zijn da die consumeWhitespace op het einde van de tree een stuk van de uint64 opeet.
    FILE* inputFile = fopen(inputFilePath, "rb");
    FILE* headerPointer = fopen(inputFilePath, "rb");
    FILE* outputFile = fopen(outputFilePath, "wb");
    FILE* blockTempFile = tmpfile();
    FILE* headerTempFile = tmpfile();

    long headerPosition = consumeLong(inputFile);
    long totalLines = consumeLong(inputFile);
    uint8_t significantBitsCodes = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    uint8_t significantBitsHeader = consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract
    uint8_t isFileSorted = consumeUint8_t(inputFile);

    if (isFileSorted) {
        rewind(inputFile);
        copyFile(inputFile, outputFile, m);
        fclose(inputFile);
        fclose(headerPointer);
        fclose(outputFile);
        fclose(blockTempFile);
        fclose(headerTempFile);
        return;
    }

    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    skipTree(headerPointer);
    long lineLengthsStart = ftell(headerPointer); // TODO zou uiteindelijk weg moeten

    BitInputHandler inputHandler = createBitInputHandler(inputFile, m / 5);
    BitInputHandler headerInputHandler = createBitInputHandler(headerPointer, m / 5);

    BitOutputHandler blokFileOutputHandler = createOutputHandler(blockTempFile, m / 5);
    BitOutputHandler headerOutputHandler = createOutputHandler(headerTempFile, m / 5);

    // uitschrijven naar de file
    // Fill in padding with info
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerPosition, sizeof(long), 1, outputFile);
    fwrite(&totalLines, sizeof(long), 1, outputFile);

    // amount of significant bits in the last byte
    uint8_t isSorted = 1;
    fwrite(&significantBitsCodes, sizeof(uint8_t), 1, outputFile);
    fwrite(&significantBitsHeader, sizeof(uint8_t), 1, outputFile);
    fwrite(&isSorted, sizeof(uint8_t), 1, outputFile); // the file is sorted.

    // Laad nItems geheugen in.
    int bufferSize = m / 5;
    uint64_t* inputBuffer = malloc(bufferSize);

    size_t nItems = bufferSize / sizeof(uint64_t);
    size_t maxBitsInBlock = nItems * 64;

    Node* linesInBlocks = initEmptyLinkedList();
    long totalLinesRead = 0;
    long start = 0;
    BlockCalculation blockCalc = calculateAmountOfLinesInBlock(totalLinesRead, start, totalLines, maxBitsInBlock, &headerInputHandler);
    while (blockCalc.amountOfLines > 0) {
        fread(inputBuffer, sizeof(uint64_t), blockCalc.bits / 64 + 1, inputFile);
        totalLinesRead += blockCalc.amountOfLines;
        append(&linesInBlocks, (int) blockCalc.amountOfLines);
        printf("%lu\n", totalLinesRead);
        sortBlock(inputBuffer, start, &headerInputHandler, &blokFileOutputHandler, &headerOutputHandler, blockCalc.amountOfLines);
        fseek(inputFile, -8, SEEK_CUR);
        start = blockCalc.bits % 64;
        blockCalc = calculateAmountOfLinesInBlock(totalLinesRead, start, totalLines, maxBitsInBlock, &headerInputHandler);
    }

    flushBits(&blokFileOutputHandler);
    flushBits(&headerOutputHandler);

    // Cleanup block sorts
    freeBitInputHandler(&headerInputHandler);
    freeBitInputHandler(&inputHandler);

    freeOutputHandler(&headerOutputHandler);
    freeOutputHandler(&blokFileOutputHandler);

    free(inputBuffer);

    fclose(inputFile);


    // merge the blocks
    int* linesInBlocksArray = toArray(linesInBlocks);
    int nBlocks = getLength(linesInBlocks);
    freeLinkedList(linesInBlocks);
    printf("merging\n");
    mergeBlocks(blockTempFile, headerTempFile, outputFile, linesInBlocksArray, nBlocks, bufferSize);

    // Merge cleanup
    free(linesInBlocksArray);
    fclose(blockTempFile);
    fclose(headerTempFile);

    // Tree
    fseek(headerPointer, headerPosition, SEEK_SET); // set headerPointer
    long treeLength = lineLengthsStart - headerPosition;
    unsigned char* tree = malloc(sizeof(unsigned char) * treeLength);
    fread(tree, sizeof(unsigned char), treeLength, headerPointer);

    for (int i = 0; i < treeLength; ++i) {
        fprintf(outputFile,"%c", tree[i]);
    }

    // Cleanup tree
    free(tree);
    fclose(outputFile);

}