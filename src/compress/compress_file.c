//
// Created by lukasbt on 10/23/23.
//

#include <stdint-gcc.h>
#include <stdio.h>

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
        if (binaryStr[i] == '1') {
            byte |= 1;
        }
    }
    return byte;
}

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize,
                  int* chars, uint64_t* frequencies, unsigned char** codes) {
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");

    // Initialize variables
    unsigned char byteBuffer[8] = {0};  // Initialize with zeroes
    int amountInBuffer = 0;
    unsigned char byte;

    // Read and compress
    int ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        unsigned char* code = codes[findCharIndex(chars, ch)];
        for (int i = 0; code[i] != '\0'; ++i) {
            byteBuffer[amountInBuffer] = code[i];
            if (amountInBuffer == 7) {
                byte = charToByte(byteBuffer);
                fputc(byte, outputFile);
            }
            amountInBuffer = (amountInBuffer + 1) % 8;
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
}