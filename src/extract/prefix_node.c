//
// Created by lukasbt on 11/23/23.
//

#include <malloc.h>
#include "prefix_node.h"

PrefixNode* createPrefixNode() {
    PrefixNode* node = malloc(sizeof(PrefixNode));
    node->left = NULL;
    node->right = NULL;
    node->character = '\0';
    return node;
}

int isLeafNode(PrefixNode* prefixNode) {
    return prefixNode->left == NULL;
}

void freePrefixNodeAndChildren(PrefixNode* root) {
    if (root == NULL) {
        return;
    }

    freePrefixNodeAndChildren(root->left);
    freePrefixNodeAndChildren(root->right);
    free(root);
}


PrefixNode* getChildFromBit(PrefixNode* prefixNode, int bit) {
    if (bit == 0) {
        return prefixNode->left;
    }

    return prefixNode->right;
}

void setChildFromBit(PrefixNode* prefixNode, int bit, PrefixNode* child) {
    if (bit == 0) {
        prefixNode->left = child;
    } else {
        prefixNode->right = child;
    }
}
