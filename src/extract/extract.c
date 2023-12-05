//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "extract.h"
#include "prefix_tree.h"
#include "decoder.h"

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
    consumeWhiteSpace(inputFile); // TODO kan weg
    fseek(inputFile, 8, SEEK_SET); // Go back to beginning + 8 bytes for header position already read
    return tree;
}

void extract(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    long headerPosition = consumeLong(inputFile);
    PrefixTree* prefixTree = buildPrefixTreeFromHeader(inputFile, headerPosition);

    long amountOfLines = consumeLong(inputFile); // amount of lines not needed
    uint8_t lastByteBitCount = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract

    // encode the file
    uint64_t buffer; // TODO hier een grote buffer van maken
    Decoder* decoder = createDecoder(prefixTree);

    while (ftell(inputFile) + 8 < headerPosition && fread(&buffer, sizeof(uint64_t), 1, inputFile)) {
        for (int bitPosition = 63; bitPosition >= 0; --bitPosition) {
            int bitValue = (int) (buffer >> bitPosition) & 1; // Extract the bit value
            decode(decoder, bitValue, outputFile);
        }
    }

    // last uint64, does not need to iterate over the whole 64 bit
    if (fread(&buffer, sizeof(uint64_t), 1, inputFile)) {
        for (int bitPosition = 63; bitPosition >= 63 - lastByteBitCount + 1; --bitPosition) {
            int bitValue = (int) (buffer >> bitPosition) & 1; // Extract the bit value
            decode(decoder, bitValue, outputFile);
        }
    } // read byte into buffer

    freePrefixTree(prefixTree);
    freeDecoder(decoder);
}