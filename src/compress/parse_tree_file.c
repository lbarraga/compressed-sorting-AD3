//
// Created by lukasbt on 10/23/23.
//

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include "parse_tree_file.h"


void consumeWhiteSpace(FILE* file) {
    while (isspace(fgetc(file)));
    fseek(file, -1, SEEK_CUR);
}

uint64_t consumeNumber(FILE* file) {
    uint64_t number;
    fscanf(file, "%lu", &number);
    return number;
}

int nCharsUntilNewline(FILE* file) {
    long originalPosition = ftell(file);
    int amount = 0;
    while (!isspace(fgetc(file))) {
        amount++;
    }
    fseek(file, originalPosition, SEEK_SET);
    return amount;
}

void readNChars(FILE* file, int nChars, unsigned char* into) {
    for (int i = 0; i < nChars; ++i) {
        into[i] = fgetc(file);
    }
    into[nChars] = '\0';
}

unsigned char* consumeString(FILE* file) {
    int amountUntilNewline = nCharsUntilNewline(file);
    unsigned char* codeRead = malloc(sizeof(unsigned char) * amountUntilNewline + 1);
    readNChars(file, amountUntilNewline, codeRead);
    return codeRead;
}



void parseTreeFile(uint64_t* chars, uint64_t* frequencies, unsigned char** codes, int charCount, FILE* treeFile) {
    for (int i = 0; i < charCount; ++i) {
        chars[i] = consumeNumber(treeFile);
        consumeWhiteSpace(treeFile);
        frequencies[i] = consumeNumber(treeFile);
        consumeWhiteSpace(treeFile);
        codes[i] = consumeString(treeFile);
        consumeWhiteSpace(treeFile);
    }

    for (int i = 0; i < charCount; ++i) {
        printf("%lu %lu %s\n", chars[i], frequencies[i], codes[i]);
    }
}