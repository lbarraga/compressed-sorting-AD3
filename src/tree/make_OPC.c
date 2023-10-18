//
// Created by lukasbt on 10/17/23.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint-gcc.h>
#include "make_OPC.h"

int countChars2(const int * frequencyTable) {
    int amount = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            amount++;
        }
    }
    return amount;
}

void fillCharBuffer2(int* charBuffer, int* freqs, const int* frequencyTable) {
    int index = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            charBuffer[index] = i;
            freqs[index] = frequencyTable[i];
            index++;
        }
    }
}

Range* generateCombinationsWithRepetition(int n) {
    int totalRanges = (n * (n + 1)) / 2;
    Range* ranges = malloc(totalRanges * sizeof(Range));

    int totalIndex = 0;
    for (int gap = 0; gap < n; ++gap) {
        for (int i = 0; i < n - gap; ++i) {
            Range range = {i, i + gap};
            ranges[totalIndex++] = range;
        }
    }

    return ranges;
}

Division* generateDivisions(Range range, int amountOfDivisions) {
    Division* divisions = malloc(amountOfDivisions * sizeof(Division));

    for (int i = range.start; i < range.end; ++i) {
        Range leftRange = {range.start, i};
        Range rightRange = {i + 1, range.end};
        Division division = {leftRange, rightRange};
        divisions[i - range.start] = division;
    }

    return divisions;
}

uint64_t calculateDivisionCost(Division* division, uint64_t** M) {
    Range rl = division->left;
    Range rr = division->right;
    return M[rl.start][rl.end] + M[rr.start][rr.end];
}

uint64_t calculateMinimumCost(Division* divisions, int amountOfDivisions, uint64_t** M) {
    uint64_t min = UINT64_MAX;
    for (int i = 0; i < amountOfDivisions; ++i) {
        uint64_t divisionCost = calculateDivisionCost(&divisions[i], M);
        if (divisionCost < min) {
            min = divisionCost;
        }
    }
    return min;
}

int calculateMinimumCostIndex(Division* divisions, int amountOfDivisions, uint64_t** M) {
    uint64_t min = UINT64_MAX;
    int index = -1;
    for (int i = 0; i < amountOfDivisions; ++i) {
        uint64_t divisionCost = calculateDivisionCost(&divisions[i], M);
        if (divisionCost < min) {
            min = divisionCost;
            index = i;
        }
    }
    return index;
}

void makePrefixCodesHulp(Range range, int bitIndex, unsigned char currentPrefix[8], unsigned char prefixCodes[][8], int* currentChar, uint64_t** M) {
    int amountOfDivisions = range.end - range.start;
    Division* divisions = generateDivisions(range, amountOfDivisions);

    if (amountOfDivisions == 0) { // Base Case
        currentPrefix[bitIndex] = '\0';
        memcpy(prefixCodes[*currentChar], currentPrefix, 8);
        (*currentChar)++;
        free(divisions);
        return;
    }

    int minDivisionIndex = calculateMinimumCostIndex(divisions, amountOfDivisions, M);
    Division* minDivision = &divisions[minDivisionIndex];

    currentPrefix[bitIndex] = '0';
    makePrefixCodesHulp(minDivision->left, bitIndex + 1, currentPrefix, prefixCodes, currentChar, M);
    currentPrefix[bitIndex] = '1';
    makePrefixCodesHulp(minDivision->right, bitIndex + 1, currentPrefix, prefixCodes, currentChar, M);

    free(divisions);
}

void makePrefixCodes(int charAmount, unsigned char prefixCodes[charAmount][8], uint64_t** M) {
    unsigned char currentPrefixCode[8];

    int currentChar = 0;
    Range fullRange = {0, charAmount - 1};
    makePrefixCodesHulp(fullRange, 0, currentPrefixCode, prefixCodes, &currentChar, M);

}

void calculateM(uint64_t** M, int charCount) {
    int totalRanges = (charCount * (charCount + 1)) / 2;
    Range* ranges = generateCombinationsWithRepetition(charCount);

    for (int i = 0; i < totalRanges; ++i) {
        Range range = ranges[i];
        int amountOfDivisions = range.end - range.start;

        if (amountOfDivisions != 0) {
            Division* divisions = generateDivisions(range, amountOfDivisions);
            uint64_t minimumDivisionCost = calculateMinimumCost(divisions, amountOfDivisions, M);
            M[range.start][range.end] += M[range.end][range.start] + minimumDivisionCost;
            free(divisions);
        }
    }

    free(ranges);
}

uint64_t calculateTreeCost(uint64_t** M, int charCount) {
    return M[0][charCount - 1]; // Top-right
}

int subListSum(const int* list, int startIndex, int endIndex) {
    int sum = 0;
    for (int i = startIndex; i < endIndex; ++i) {
        sum += list[i];
    }
    return sum;
}

uint64_t** initM(int* freqs, int charCount) {
    uint64_t** M = (uint64_t**)calloc(charCount, sizeof(uint64_t *));
    for (int i = 0; i < charCount; ++i) {
        M[i] = (uint64_t *)calloc(charCount, sizeof(uint64_t));
    }

    // Fill elements under the diagonal
    for (int j = 0; j < charCount; ++j) {
        for (int i = j + 1; i < charCount; ++i) {
            M[i][j] = subListSum(freqs, j, i + 1);
        }
    }

    return M;
}

void freeM(uint64_t** M, int charCount) {
    for (int i = 0; i < charCount; ++i) {
        free(M[i]);
    }
    free(M);
}

void makeOPC(int* frequencyTable, FILE* outputFile){
    int charCount = countChars2(frequencyTable);
    int chars[charCount];
    int freqs[charCount];
    fillCharBuffer2(chars, freqs, frequencyTable);

    uint64_t** M = initM(freqs, charCount);
    calculateM(M, charCount);
    unsigned char prefixCodes[charCount][8];
    makePrefixCodes(charCount, prefixCodes, M);

    // Print all strings
    printf("Total bit cost: %lu\n\n", calculateTreeCost(M, charCount));
    for (int i = 0; i < charCount; ++i) {
        fprintf(outputFile, "%d %d %s\n", chars[i], freqs[i], prefixCodes[i]);
    }

    freeM(M, charCount);

}

