//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "extract.h"
#include "prefix_tree.h"
#include "decoder.h"
#include "../IO/bit_input.h"

PrefixTree* buildPrefixTreeFromHeader(FILE* inputFile, long headerPosition) {
    fseek(inputFile, headerPosition, SEEK_SET); // go to the headerPosition

    // add the code to the tree
    PrefixTree* tree = createPrefixTree();
    int charCount = consumeInt(inputFile);
    consumeWhiteSpace(inputFile);
    for (int i = 0; i < charCount; ++i) {
        int character = consumeInt(inputFile);
        consumeWhiteSpace(inputFile);
        unsigned char* code = consumeString(inputFile);
        addToPrefixTree(tree, character, code);
        free(code);
    }
    consumeWhiteSpace(inputFile);
    fseek(inputFile, 8, SEEK_SET); // Go back to beginning + 8 bytes for header position already read
    return tree;
}

void extract(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    long headerPosition = consumeLong(inputFile);
    PrefixTree* prefixTree = buildPrefixTreeFromHeader(inputFile, headerPosition);

    consumeLong(inputFile); // amount of lines not needed
    uint8_t lastByteBitCount = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract
    consumeUint8_t(inputFile); // if this file is sorted. Does not matter in extraction.

    // encode the file
    long bitsToRead = (headerPosition - ftell(inputFile)) * 8 - (64 - lastByteBitCount);
    BitInputHandler inputHandler = createBitInputHandler(inputFile, bufferSize);
    Decoder* decoder = createDecoder(prefixTree);

    for (int i = 0; i < bitsToRead; ++i) {
        int bit = (int) readNBits(&inputHandler, 1);
        decode(decoder, bit, outputFile);
    }

    freeBitInputHandler(&inputHandler);

    fclose(outputFile);
    fclose(inputFile);

    freePrefixTree(prefixTree);
    freeDecoder(decoder);
}