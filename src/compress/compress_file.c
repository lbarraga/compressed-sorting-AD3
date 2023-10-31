//
// Created by lukasbt on 10/23/23.
//

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <stdint-gcc.h>

int findCharIndex(const int* chars, int charToFind) {
    int index = 0;
    while (chars[index] != charToFind) {
        index++;
    }
    return index;
}

unsigned char stringToByte(const unsigned char* binaryStr) {
    unsigned char byte = 0;
    for (int i = 0; i < 8; ++i) {
        byte <<= 1;
        byte |= binaryStr[i] == '1';
    }
    return byte;
}

void outputBit(unsigned char bit, unsigned char* byteBuffer, int* currentBitIndexBuffer, FILE* outputFile) {
    byteBuffer[*currentBitIndexBuffer] = bit;
    if (*currentBitIndexBuffer == 7) {
        unsigned char byte = stringToByte(byteBuffer);
        fputc(byte, outputFile);
    }
    *currentBitIndexBuffer = (*currentBitIndexBuffer + 1) % 8;
}

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize, int* chars, unsigned char** codes, int charCount) {
    char* headerTempFileName = "/home/lukasbt/projectAD3/data/header.temp";
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");
    FILE* headerTempFile = fopen(headerTempFileName, "w");

    // Initialize variables
    unsigned char* inputBuffer = malloc(bufferSize);

    unsigned char byteInputBuffer[8];
    unsigned char byteHeaderBuffer[8];

    int currentBitIndexInputBuffer = 0;
    int currentBitIndexHeaderBuffer = 0;

    long padding = 0; // 64 bits for pointer to header, which is placed at the end of the compressed file
    fwrite(&padding, sizeof(long), 1, outputFile);

    // include tree in the header of the file
    fprintf(outputFile, " %d ", charCount);
    for (int i = 0; i < charCount; ++i) {
        fprintf(outputFile, "%d %s ", chars[i], codes[i]);
    }

    // Encode characters and write to file
    int lineLength = 0;
    size_t bytesRead;
    while ((bytesRead = fread(inputBuffer, 1, bufferSize, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            unsigned char ch = inputBuffer[i];
            unsigned char* code = codes[findCharIndex(chars, ch)];
            for (int j = 0; code[j] != '\0'; ++j) {
                lineLength++;
                outputBit(code[j], byteInputBuffer, &currentBitIndexInputBuffer, outputFile);
            }
            if (ch == '\n') {
                // output the 5 bits allocated for the length of the length
                int lineLengthLength = (int) ceil(log2(lineLength)); // will be less than 2^5 = 32
                for (int j = 4; j >= 0; --j) {
                    int bit = (lineLengthLength >> j) & 1; // Isolate the jth bit from the right (0-based index)
                    unsigned char char_bit = bit ? '1' : '0';
                    outputBit(char_bit, byteHeaderBuffer, &currentBitIndexHeaderBuffer, headerTempFile);
                }
                // output [lineLengthLength] amount of bits for the lineLength
                for (int j = lineLengthLength - 1; j >= 0; --j) {
                    int bit = ((lineLength - 1) >> j) & 1;
                    unsigned char char_bit = bit ? '1' : '0';
                    outputBit(char_bit, byteHeaderBuffer, &currentBitIndexHeaderBuffer, headerTempFile);
                }
                lineLength = 0;
            }
        }
    }

    // Flush remaining bits in inputBuffer, if any
    if (currentBitIndexInputBuffer > 0) {
        fputc(stringToByte(byteInputBuffer), outputFile);
    }

    if (currentBitIndexHeaderBuffer > 0) {
        fputc(stringToByte(byteHeaderBuffer), headerTempFile);
    }

    // Append the header to the file
    fclose(headerTempFile);
    headerTempFile = fopen(headerTempFileName, "r");

    long headerBegin = ftell(outputFile);
    printf("%li\n", headerBegin);

    int ch = fgetc(headerTempFile);
    while (ch != EOF) {
        fprintf(outputFile, "%c", ch);
        ch = fgetc(headerTempFile);
    }

    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerBegin, sizeof(long), 1, outputFile);


    // Cleanup
    fclose(inputFile);
    fclose(outputFile);
    free(inputBuffer);
}