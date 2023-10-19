//
// Created by lukasbt on 10/18/23.
//

#ifndef PROJECTAD3_HU_TUCKER_H
#define PROJECTAD3_HU_TUCKER_H

typedef enum TreeNodeType{
    TERMINAL, // 0
    INTERNAL, // 1
    DELETED,  // 2
} TreeNodeType;

typedef struct Pair {
    int first;
    int second;
} Pair;

#include <stdio.h>
#include <stdint-gcc.h>

typedef struct TreeNode{
    TreeNodeType type;
    struct TreeNode* left;
    struct TreeNode* right;
    uint64_t frequency;
} TreeNode;

void makeHT_OPC(int* frequencyTable, FILE* outputFile);

#endif //PROJECTAD3_HU_TUCKER_H
