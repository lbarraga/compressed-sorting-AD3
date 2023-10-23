//
// Created by lukasbt on 10/23/23.
//

#ifndef PROJECTAD3_PARSE_TREE_FILE_H
#define PROJECTAD3_PARSE_TREE_FILE_H

#include <stdint-gcc.h>
#include <bits/types/FILE.h>

void parseTreeFile(uint64_t* chars, uint64_t* frequencies, unsigned char** codes, int charAmount, FILE* treeFile);

#endif //PROJECTAD3_PARSE_TREE_FILE_H
