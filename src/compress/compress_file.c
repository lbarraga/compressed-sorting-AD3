//
// Created by lukasbt on 10/23/23.
//

#include <stdint-gcc.h>
#include <stdio.h>
#include <malloc.h>

int findCharIndex(const int* chars, int charToFind) {
    int index = 0;
    while (chars[index] != charToFind) {
        index++;
    }
    return index;
}

unsigned char charToByte(const unsigned char* binaryStr) {
    unsigned char byte = 0;
    for (int i = 0; i < 8; ++i) {
        byte <<= 1;
        byte |= binaryStr[i] == '1';
    }
    return byte;
}

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize,
                  int* chars, uint64_t* frequencies, unsigned char** codes) {
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");

    // Initialize variables
    unsigned char byteBuffer[8];  // Initialize with zeroes
    unsigned char* buffer = malloc(bufferSize);
    int amountInBuffer = 0;
    unsigned char byte;

    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufferSize, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            unsigned char ch = buffer[i];
            unsigned char* code = codes[findCharIndex(chars, ch)];
            for (int j = 0; code[j] != '\0'; ++j) {
                byteBuffer[amountInBuffer] = code[j];
                if (amountInBuffer == 7) {
                    byte = charToByte(byteBuffer);
                    fputc(byte, outputFile);
                }
                amountInBuffer = (amountInBuffer + 1) % 8;
            }
        }
    }

    // Flush remaining bits in buffer, if any
    if (amountInBuffer > 0) {
        byte = charToByte(byteBuffer);
        fputc(byte, outputFile);
    }

    // Cleanup
    fclose(inputFile);
    fclose(outputFile);
    free(buffer);
}