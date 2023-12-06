//
// Created by lukasbt on 10/23/23.
//

#ifndef PROJECTAD3_PARSE_TREE_FILE_H
#define PROJECTAD3_PARSE_TREE_FILE_H

#include <stdint-gcc.h>
#include <bits/types/FILE.h>
#include "compress.h"

void parseTreeFile(uint64_t* frequencies, OPC** codes, int charAmount, FILE* treeFile);
unsigned char* consumeString(FILE* file);
int consumeInt(FILE* file);
void consumeWhiteSpace(FILE* file);
long consumeLong(FILE *inputFile);
uint8_t consumeUint8_t(FILE* inputFile);

#endif //PROJECTAD3_PARSE_TREE_FILE_H
