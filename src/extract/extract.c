//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <malloc.h>
#include "extract.h"

PrefixNode* initPrefixNode() {
    PrefixNode* node = malloc(sizeof(PrefixNode));
    node->left = NULL;
    node->right = NULL;
    node->character = '\0';
    return node;
}

void freePrefixTree(PrefixNode* root) {
    if (root == NULL) {
        return;
    }

    freePrefixTree(root->left);
    freePrefixTree(root->right);
    free(root);
}

void addToTree(const unsigned char* code, unsigned char character, PrefixNode* root) {
    int i = 0;
    PrefixNode* prefixNode = root;
    while (code[i] != '\0') {
        unsigned char bit = code[i];
        PrefixNode** pPrefixNode = bit == '0'? &prefixNode->left: &prefixNode->right;
        if (*pPrefixNode == NULL) {
            PrefixNode* newPrefixNode = initPrefixNode();
            *pPrefixNode = newPrefixNode;
        }
        prefixNode = *pPrefixNode;
        i++;
    }
    prefixNode->character = character;
}

void buildPrefixTreeFromHeader(FILE* inputFile, long headerPosition, PrefixNode* root) {
    fseek(inputFile, headerPosition, SEEK_SET);

    int charCount = consumeInt(inputFile);
    consumeWhiteSpace(inputFile);
    for (int i = 0; i < charCount; ++i) {
        int character = consumeInt(inputFile);
        consumeWhiteSpace(inputFile);
        unsigned char* code = consumeString(inputFile);
        addToTree(code, character, root);
        free(code);
    }
    consumeWhiteSpace(inputFile);
    fseek(inputFile, 8, SEEK_SET); // Go back to beginning + 8 bytes for header position already read
}

void extract(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    printf("%d", bufferSize); //TODO
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");
    PrefixNode* root = initPrefixNode();

    long headerPosition = consumeLong(inputFile);
    buildPrefixTreeFromHeader(inputFile, headerPosition, root);

    consumeLong(inputFile); // amount of lines not needed
    uint8_t lastByteBitCount = consumeUint8_t(inputFile); // amount of bits that are not filler bits in the last byte
    consumeUint8_t(inputFile); // same for header, but not line positions are not needed in extract
    printf("%d\n", lastByteBitCount);

    // encode the file
    uint64_t buffer; // To hold each byte read from the file
    PrefixNode* currentPrefixNode = root;

    while (ftell(inputFile) + 8 < headerPosition && fread(&buffer, sizeof(uint64_t), 1, inputFile)) {
        for (int bitPosition = 63; bitPosition >= 0; --bitPosition) {
            uint64_t bitValue = (buffer >> bitPosition) & 1; // Extract the bit value
            currentPrefixNode = bitValue == 0? currentPrefixNode->left: currentPrefixNode->right;
            if (currentPrefixNode->left == NULL) {
                fprintf(outputFile, "%c", currentPrefixNode->character);
                currentPrefixNode = root;
            }
        }
    }

    // last uint64, does not need to iterate over the whole 64 bit
    if (fread(&buffer, sizeof(uint64_t), 1, inputFile)) {
        for (int bitPosition = 63; bitPosition >= 64 - lastByteBitCount; --bitPosition) {
            uint64_t bitValue = (buffer >> bitPosition) & 1; // Extract the bit value
            currentPrefixNode = bitValue == 0? currentPrefixNode->left: currentPrefixNode->right;
            if (currentPrefixNode->left == NULL) {
                fprintf(outputFile, "%c", currentPrefixNode->character);
                currentPrefixNode = root;
            }
        }
    } // read byte into buffer

    freePrefixTree(root);
}