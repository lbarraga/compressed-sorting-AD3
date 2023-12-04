//
// Created by lukasbt on 12/3/23.
//

#ifndef PROJECTAD3_BLOCK_MERGE_H
#define PROJECTAD3_BLOCK_MERGE_H

#include <bits/types/FILE.h>
#include "../tree/linkedList/linkedlist.h"

void mergeBlocks(FILE* blockFile, FILE* headerFile, FILE* outputFile, int* linesInBlocks, int nBlocks);

#endif //PROJECTAD3_BLOCK_MERGE_H
