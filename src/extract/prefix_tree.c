//
// Created by lukasbt on 11/22/23.
//

#include <malloc.h>
#include "prefix_node.h"
#include "prefix_tree.h"

PrefixTree* createPrefixTree() {
    PrefixTree* tree = malloc(sizeof(PrefixTree));
    tree->root = createPrefixNode();
    return tree;
}

void freePrefixTree(PrefixTree* prefixTree) {
    freePrefixNodeAndChildren(prefixTree->root);
    free(prefixTree);
}

void addToPrefixTree(PrefixTree* prefixTree, unsigned char character, const unsigned char* code) {
    int i = 0;
    PrefixNode* prefixNode = prefixTree->root;
    while (code[i] != '\0') {
        int bit = code[i] == '1'? 1 : 0;
        PrefixNode* child = getChildFromBit(prefixNode, bit);
        if (child == NULL) {
            PrefixNode* newChild = createPrefixNode();
            setChildFromBit(prefixNode, bit, newChild);
            child = newChild;
        }
        prefixNode = child;
        i++;
    }
    prefixNode->character = character;
}
