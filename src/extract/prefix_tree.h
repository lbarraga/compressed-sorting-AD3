//
// Created by lukasbt on 11/22/23.
//

#ifndef PROJECTAD3_PREFIX_TREE_H
#define PROJECTAD3_PREFIX_TREE_H

#include "prefix_node.h"

typedef struct PrefixTree {
    PrefixNode* root;
} PrefixTree;

PrefixTree* createPrefixTree();
void freePrefixTree(PrefixTree* prefixTree);
void addToPrefixTree(PrefixTree* prefixTree, unsigned char character, const unsigned char* code);


#endif //PROJECTAD3_PREFIX_TREE_H
