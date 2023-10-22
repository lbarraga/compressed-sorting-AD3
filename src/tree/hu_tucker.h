//
// Created by lukasbt on 10/18/23.
//

#ifndef PROJECTAD3_HU_TUCKER_H
#define PROJECTAD3_HU_TUCKER_H

#include <stdio.h>
#include <stdint-gcc.h>
#include "linkedList/linkedlist.h"

typedef enum TreeNodeType{
    TERMINAL, // 0
    INTERNAL, // 1
    DELETED,  // 2
} TreeNodeType;

typedef struct Pair {
    int first;
    int second;
} Pair;

typedef struct TreeNode{
    TreeNodeType type;
    uint64_t frequency;
    Node* dependentChars;
} TreeNode;

void makeHT_OPC(int* chars, int* freqs, int charCount, FILE* outputFile);

#endif //PROJECTAD3_HU_TUCKER_H
