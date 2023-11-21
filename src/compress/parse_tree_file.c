//
// Created by lukasbt on 10/23/23.
//

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include "parse_tree_file.h"

long consumeLong(FILE *inputFile) {
    long result;
    fread(&result, sizeof(long), 1, inputFile);
    return result;
}

uint8_t consumeUint8_t(FILE* inputFile) {
    uint8_t result;
    fread(&result, sizeof(uint8_t), 1, inputFile);
    return result;
}

void consumeWhiteSpace(FILE* file) {
    while (isspace(fgetc(file)));
    fseek(file, -1, SEEK_CUR);
}

int consumeInt(FILE* file) {
    int number;
    fscanf(file, "%d", &number);
    return number;
}

uint64_t consumeUint64(FILE* file) {
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

OPC* consumeCode(FILE* file) {
    unsigned char* code = consumeString(file);
    uint64_t result = 0;
    int i = 0;
    while (code[i] != '\0') {
        int bit = code[i] == '1'? 1: 0;
        result <<= 1;
        result |= bit;
        i++;
    }
    free(code);
    OPC* opc = malloc(sizeof(OPC));
    opc->code = result;
    opc->length = i;
    return opc;
}

void parseTreeFile(uint64_t* frequencies, OPC** codes, int charCount, FILE* treeFile) {
    for (int i = 0; i < charCount; ++i) {
        int charAsInt = consumeInt(treeFile);
        consumeWhiteSpace(treeFile);
        frequencies[i] = consumeUint64(treeFile);
        consumeWhiteSpace(treeFile);
        codes[charAsInt] = consumeCode(treeFile);
        consumeWhiteSpace(treeFile);
    }
}