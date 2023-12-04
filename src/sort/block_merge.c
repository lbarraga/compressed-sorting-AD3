//
// Created by lukasbt on 12/3/23.
//

#include <malloc.h>
#include "../IO/bit_input.h"
#include "../IO/bit_input/bit_io.h"

void initInputHandlers(BitInputHandler* inputHandlersLength, BitInputHandler* inputHandlersBlock,
                       FILE* headerFile, FILE* blockFile, const int* linesInBlocks, int nBlocks) {

    fseek(headerFile, 0, SEEK_SET);
    BitInputHandler tmpLineHandler = createBitInputHandler(headerFile, 8);

    long bit = 0;
    for (int blockNr = 0; blockNr < nBlocks; blockNr++) {

        InputHandlerPosition position = getInputPosition(&tmpLineHandler);
        BitInputHandler handler = createBitInputHandler(headerFile, 8);
        setAtInputPosition(&handler, &position);
        inputHandlersLength[blockNr] = handler;

        fseek(blockFile, bit / 8, SEEK_SET);
        inputHandlersBlock[blockNr] = createBitInputHandler(blockFile, 8);
        readNBits(&inputHandlersBlock[blockNr], bit % 8);

        int linesInBlock = linesInBlocks[blockNr];
        printf("%d lines in block %d\n", linesInBlock, nBlocks);
        for (int _ = 0; _ < linesInBlock; ++_) {
            int length = readLength(&tmpLineHandler);
            printf("\t%d\n", length);
            bit += length;
        }
    }

    freeBitInputHandler(&tmpLineHandler);
}

long calculateTotalLineAmount(const int* linesInBlocks, int nBlocks) {
    long sum = 0;
    for (int i = 0; i < nBlocks; ++i) {
        sum += linesInBlocks[i];
    }
    return sum;
}

int compareLines(const uint64_t* line1, const uint64_t* line2, int length1, int length2) {
    int i = 0;
    while (i < length1 && i < length2) {

        if (line1[i] != line2[i]) {
            return line1[i] < line2[i]? -1 : 1;
        }

        i += 1;
    }

    return (i == length2) - (i == length1);
}

int findSmallestBlock(uint64_t** currentLines, const int* currentLineLengths, const int* linesInBlocks, int nBlocks) {
    int smallestBlockNr = -1;
    for (int blockNr = 0; blockNr < nBlocks; ++blockNr) {
        if (linesInBlocks[blockNr] != 0) {
            if (smallestBlockNr == -1) {
                smallestBlockNr = blockNr;
            }
            uint64_t* line1 = currentLines[blockNr];
            uint64_t* line2 = currentLines[smallestBlockNr];
            int len1 = currentLineLengths[blockNr] / 64 + 1;
            int len2 = currentLineLengths[smallestBlockNr] / 64 + 1;
            printf("l1 = %d, l2 = %d\n", len1, len2);
            if (compareLines(line1, line2, len1, len2) < 0) {
                smallestBlockNr = blockNr;
            }
        }
    }
    return smallestBlockNr;
}

int nElementsForLine(int length) {
    return (length + 63) / 64; // ceil division
}

int calcBitsInLastElement(int length) {
    return length % 64 == 0? 64: length % 64;
}

void mergeBlocks(FILE* blockFile, FILE* headerFile, FILE* outputFile, int* linesInBlocks, int nBlocks) {

    // 0. initialisatie van de lijst met blok pointers en lijst van lengte pointers (kan door positie op te slaan)
    long totalLines = calculateTotalLineAmount(linesInBlocks, nBlocks);
    printf("there are %d blocks and %lu lines in total\n", nBlocks, totalLines);

    BitInputHandler* inputHandlersLength = malloc(sizeof(BitInputHandler) * nBlocks);
    BitInputHandler* inputHandlersBlock = malloc(sizeof(BitInputHandler) * nBlocks);

    BitOutputHandler outputHandler = createOutputHandler(outputFile, 8);

    uint64_t** currentLines = malloc(sizeof(uint64_t*) * nBlocks);
    int* currentLineLengths = malloc(sizeof(int) * nBlocks);

    initInputHandlers(inputHandlersLength, inputHandlersBlock, headerFile, blockFile, linesInBlocks, nBlocks);

    // 2. alle lengtes eens lezen en invullen;
    for (int blockNr = 0; blockNr < nBlocks; ++blockNr) {
        int length = readLength(&inputHandlersLength[blockNr]);
        currentLineLengths[blockNr] = length;

        int nElements = nElementsForLine(length);
        currentLines[blockNr] = malloc(sizeof(uint64_t) * nElements);
        for (int elementNr = 0; elementNr < nElements - 1; ++elementNr) {
            currentLines[blockNr][elementNr] = readNBits(&inputHandlersBlock[blockNr], 64);
        }
        int bitsInLastElement = calcBitsInLastElement(length);
        currentLines[blockNr][nElements - 1] = readNBits(&inputHandlersBlock[blockNr], bitsInLastElement);
    }

    for (int _ = 0; _ < totalLines; ++_) {
        // 3. De kleinste vinden we door de tot nu toe kleinste te vergelijken met de volgende en te swappen wanneer nodig.
        int smallestBlockNr = findSmallestBlock(currentLines, currentLineLengths, linesInBlocks, nBlocks);
        printf("smallest block number is %d\n", smallestBlockNr);

        linesInBlocks[smallestBlockNr]--;

        // 4. Dan schrijven we de kleinste uit naar de file in blokken van 64bit en uiteindelijk < 64 bit
        int smallestLineLength = currentLineLengths[smallestBlockNr];
        int nElements = nElementsForLine(smallestLineLength);
        uint64_t* smallestLine = currentLines[smallestBlockNr];
        for (int i = 0; i < nElements - 1; ++i) {
            outputNBits(&outputHandler, smallestLine[i], 64);
        }
        int bitsInLastElement = calcBitsInLastElement(smallestLineLength);
        outputNBits(&outputHandler, smallestLine[nElements - 1], bitsInLastElement);

        // 5. Dan vullen we een nieuwe lijn in op de plaats die we net hebben weggeschreven.
        int newLength = readLength(&inputHandlersLength[smallestBlockNr]);
        currentLineLengths[smallestBlockNr] = newLength;

        int newNElements = nElementsForLine(newLength);
        currentLines[smallestBlockNr] = realloc(currentLines[smallestBlockNr], sizeof(uint64_t*) * newNElements);
        for (int i = 0; i < newNElements - 1; ++i) {
            currentLines[smallestBlockNr][i] = readNBits(&inputHandlersBlock[smallestBlockNr], 64);
        }
        bitsInLastElement = calcBitsInLastElement(newLength);
        currentLines[smallestBlockNr][newNElements - 1] = readNBits(&inputHandlersBlock[smallestBlockNr], bitsInLastElement);
        currentLines[smallestBlockNr][newNElements - 1] <<= (64 - bitsInLastElement);
    }

    flushBits(&outputHandler);

    // free memory
    for (int blockNr = 0; blockNr < nBlocks; ++blockNr) {
        freeBitInputHandler(&inputHandlersLength[blockNr]);
        freeBitInputHandler(&inputHandlersBlock[blockNr]);
        free(currentLines[blockNr]);
    }

    freeOutputHandler(&outputHandler);
    free(inputHandlersBlock);
    free(inputHandlersLength);
    free(currentLines);
    free(currentLineLengths);

}