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

void outputNumber(int number, int nBits, unsigned char* byteBuffer, int* currentBitIndexBuffer, FILE* outputFile) {
    for (int j = nBits - 1; j >= 0; --j) {
        int bit = (number >> j) & 1; // Isolate the jth bit from the right (0-based index)
        unsigned char char_bit = bit ? '1' : '0';
        outputBit(char_bit, byteBuffer, currentBitIndexBuffer, outputFile);
    }
}

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize, int* chars, unsigned char** codes, int charCount) {
    char* headerTempFileName = "header.temp";
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");
    FILE* headerTempFile = fopen(headerTempFileName, "w");

    // Initialize variables
    unsigned char* inputBuffer = malloc(bufferSize);

    unsigned char byteInputBuffer[8];
    unsigned char byteHeaderBuffer[8];

    int currentBitIndexInputBuffer = 0;
    int currentBitIndexHeaderBuffer = 0;

    long padding = 0;
    fwrite(&padding, sizeof(long), 1, outputFile); // 64 bits for pointer to header, which is placed at the end of the compressed file
    fwrite(&padding, sizeof(long), 1, outputFile); // Amount of lines in the file.
    fwrite(&padding, sizeof(uint8_t), 1, outputFile); // amount of significant bits in last byte of encodings
    fwrite(&padding, sizeof(uint8_t), 1, outputFile); // amount of significant bits in last byte of header

    // include tree in the header of the file
    fprintf(headerTempFile, "%d ", charCount);
    for (int i = 0; i < charCount; ++i) {
        fprintf(headerTempFile, "%d %s ", chars[i], codes[i]);
    }

    // Encode characters and write to file
    int lineLength = 0;
    long totalLines = 0;
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
                outputNumber(lineLengthLength, 5, byteHeaderBuffer, &currentBitIndexHeaderBuffer, headerTempFile);
                outputNumber(lineLength - 1, lineLengthLength, byteHeaderBuffer, &currentBitIndexHeaderBuffer, headerTempFile);
                lineLength = 0;
                totalLines++;
            }
        }
    }

    // Flush remaining bits in inputBuffer, if any
    if (currentBitIndexInputBuffer > 0) {
        fputc(stringToByte(byteInputBuffer), outputFile);
    } else {
        currentBitIndexInputBuffer = 8;
    }

    if (currentBitIndexHeaderBuffer > 0) {
        fputc(stringToByte(byteHeaderBuffer), headerTempFile);
    } else {
        currentBitIndexHeaderBuffer = 8;
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

    // Fill in padding with info
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerBegin, sizeof(long), 1, outputFile);
    fwrite(&totalLines, sizeof(long), 1, outputFile);
    fwrite(&currentBitIndexInputBuffer, sizeof(uint8_t), 1, outputFile);
    fwrite(&currentBitIndexHeaderBuffer, sizeof(uint8_t), 1, outputFile);


    // Cleanup
    fclose(inputFile);
    fclose(outputFile);
    free(inputBuffer);
}