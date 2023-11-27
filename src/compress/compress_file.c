//
// Created by lukasbt on 10/23/23.
//

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "../IO/bit_input/bit_io.h"
#include "compress.h"

void addTreeToHeader(FILE* file, int charCount, OPC** codes) {
    fprintf(file, "%d ", charCount);
    for (int i = 0; i < 128; ++i) {
        OPC* code = codes[i];
        if (code) {
            fprintf(file, "%d ", i);
            for (int j = code->length - 1; j >= 0; --j) {
                fprintf(file, "%lu", (code->code >> j) & 1); // TODO hier ook uint64 voor gebruiken
            }
            fprintf(file, " ");
        }
    }
}

int ceilLog2(const int* lineLength) {
    return (int) sizeof(int) * 8 - __builtin_clz(*lineLength);
}


void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize, OPC** codes, int charCount) {

    FILE* inputFile = fopen(inputFileName, "r");
    FILE* outputFile = fopen(outputFileName, "w");
    FILE* headerTempFile = tmpfile();

    // Initialize variables
    unsigned char* inputBuffer = malloc(bufferSize / 2);

    BitOutputHandler outputHandlerOutput = createOutputHandler(outputFile, bufferSize / 2);
    BitOutputHandler outputHandlerHeader = createOutputHandler(headerTempFile, 8);

    // 8: header pointer, 8: amount of lines, 1: significant bits is last byte of encoding, 1: idem but for header
    uint8_t padding[8 + 8 + 1 + 1] = {0};
    printf("padding: %lu\n", sizeof(padding));
    fwrite(&padding, sizeof(uint8_t), sizeof(padding), outputFile); // TODO waarom is dit in het begin en niet ook in de footer.

    // include tree in the header of the file
    addTreeToHeader(headerTempFile, charCount, codes);

    clock_t start = clock();
    // Encode characters and write to file
    int lineLength = 0;
    long totalLines = 0;
    size_t bytesRead;
    while ((bytesRead = fread(inputBuffer, 1, bufferSize / 2, inputFile)) > 0) {
        for (size_t i = 0; i < bytesRead; ++i) {
            // write the bits of the code to the output file
            unsigned char ch = inputBuffer[i];
            OPC* code = codes[ch];
            outputNBits(&outputHandlerOutput, code->code, code->length);
            lineLength += code->length;

            if (ch == '\n') {
                // output the 5 bits allocated for the length of the length
                int lineLengthLength = ceilLog2(&lineLength); // will be less than 2^5 = 32
                outputNBits(&outputHandlerHeader, lineLengthLength, 5);
                outputNBits(&outputHandlerHeader, lineLength - 1, lineLengthLength);
                lineLength = 0;
                totalLines++;
            }
        }
    }
    clock_t end = clock();

    printf("time taken: %f\n", (double) (end - start) / 1000000);
    start = clock();

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
    freeOutputHandler(&outputHandlerOutput);
    freeOutputHandler(&outputHandlerHeader);

    end = clock();
    printf("time taken: %f\n", (double) (end - start) / 1000000);

}
