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

void extract(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    PrefixNode* root = initPrefixNode();

    // build the prefixTree
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
    // encode the file
    unsigned char buffer; // To hold each byte read from the file
    PrefixNode* currentPrefixNode = root;

    while (fread(&buffer, 1, 1, inputFile) == 1) { // Read one byte at a time
        for (int bitPosition = 7; bitPosition >= 0; --bitPosition) {
            int bitValue = (buffer >> bitPosition) & 1; // Extract the bit value
            currentPrefixNode = bitValue == 0? currentPrefixNode->left: currentPrefixNode->right;
            if (currentPrefixNode->left == NULL) {
                fprintf(outputFile, "%c", currentPrefixNode->character);
                currentPrefixNode = root;
            }
        }
    }

    freePrefixTree(root);
}