//
// Created by lukasbt on 10/23/23.
//

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "../IO/bit_input/bit_io.h"
#include "compress.h"


// TODO codes niet als strings, lines byte aligned
void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize, OPC** codes, int charCount) {
    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");
    FILE* headerTempFile = tmpfile();

    // Initialize variables
    unsigned char* inputBuffer = malloc(bufferSize);

    BitOutputHandler outputHandlerOutput = createOutputHandler(outputFile, bufferSize);
    BitOutputHandler outputHandlerHeader = createOutputHandler(headerTempFile, bufferSize);

    // 8: header pointer, 8: amount of lines, 1: significant bits is last byte of encoding, 1: idem for header
    uint8_t padding[8 + 8 + 1 + 1] = {0};
    fwrite(&padding, sizeof(uint8_t), sizeof(padding), outputFile);

    // include tree in the header of the file
    fprintf(headerTempFile, "%d ", charCount);
    printf("%d ", charCount);
    for (int i = 0; i < 128; ++i) {
        OPC* code = codes[i];
        if (code) {
            fprintf(headerTempFile, "%d ", i);
            printf("%d ", i);
            for (int j = code->length - 1; j >= 0; --j) {
                printf("%lu", (code->code >> j) & 1);
                fprintf(headerTempFile, "%lu", (code->code >> j) & 1);
            }
            fprintf(headerTempFile, " ");
            printf(" ");
        }
    }

    // Encode characters and write to file
    int lineLength = 0;
    long totalLines = 0;
    size_t bytesRead;
    while ((bytesRead = fread(inputBuffer, 1, bufferSize, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            // write the bits of the code to the output file
            unsigned char ch = inputBuffer[i];
            OPC* code = codes[ch];
            lineLength += outputCode(&outputHandlerOutput, code);

            if (ch == '\n') {
                // output the 5 bits allocated for the length of the length
                int lineLengthLength = (int) ceil(log2(lineLength)); // will be less than 2^5 = 32
                outputNumber(&outputHandlerHeader, lineLengthLength, 5);
                outputNumber(&outputHandlerHeader, lineLength - 1, lineLengthLength);
                lineLength = 0;
                totalLines++;
            }
        }
    }

    // cleanup input file
    fclose(inputFile);
    free(inputBuffer);

    // Flush remaining bits in handlers
    flushBits(&outputHandlerOutput);
    flushBits(&outputHandlerHeader);

    // Append the header to the file
    long headerBegin = ftell(outputFile);

    fseek(headerTempFile, 0, SEEK_SET);
    int ch = fgetc(headerTempFile);
    while (ch != EOF) {
        fprintf(outputFile, "%c", ch);
        ch = fgetc(headerTempFile);
    }

    // Fill in padding with info
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&headerBegin, sizeof(long), 1, outputFile);
    fwrite(&totalLines, sizeof(long), 1, outputFile);
    // amount of significant bits in the last byte
    fwrite(&outputHandlerOutput.bitsWritten, sizeof(uint8_t), 1, outputFile);
    fwrite(&outputHandlerHeader.bitsWritten, sizeof(uint8_t), 1, outputFile);

    // cleanup
    fclose(outputFile);
    freeOutputHandler(outputHandlerOutput);
    freeOutputHandler(outputHandlerHeader);

}