//
// Created by lukasbt on 10/14/23.
//


#include <stdio.h>
#include <malloc.h>
#include "extract.h"

void addToTree(unsigned char* code, unsigned char character, PrefixNode* root) {
    int i = 0;
    while (code[i] != '\0') {
        unsigned char bit = code[i];
        PrefixNode** pPrefixNode = bit == '0'? &root->left: &root->right;
        if (*pPrefixNode == NULL) {
            PrefixNode* prefixNode = malloc(sizeof(PrefixNode));
            prefixNode->left = NULL;
            prefixNode->right = NULL;
            *pPrefixNode = prefixNode;
            (*pPrefixNode).
        }
    }
}

void extract(const char *inputFilePath, const char *outputFilePath, int bufferSize) {
    FILE* inputFile = fopen(inputFilePath, "r");
    FILE* outputFile = fopen(outputFilePath, "w");

    int charCount = consumeInt(inputFile);
    consumeWhiteSpace(inputFile);

    for (int i = 0; i < charCount; ++i) {
        int character = consumeInt(inputFile);
        consumeWhiteSpace(inputFile);
        unsigned char* code = consumeString(inputFile);
        printf("%d (%c): %s\n", character, character, code);
        free(code);
    }

}