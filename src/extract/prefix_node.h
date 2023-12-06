//
// Created by lukasbt on 11/23/23.
//

#ifndef PROJECTAD3_PREFIX_NODE_H
#define PROJECTAD3_PREFIX_NODE_H

typedef struct PrefixNode {
    struct PrefixNode* left;
    struct PrefixNode* right;
    unsigned char character;

} PrefixNode;

PrefixNode* createPrefixNode();
void freePrefixNodeAndChildren(PrefixNode* root);
int isLeafNode(PrefixNode* prefixNode);
PrefixNode* getChildFromBit(PrefixNode* prefixNode, int bit);
void setChildFromBit(PrefixNode* prefixNode, int bit, PrefixNode* child);

#endif //PROJECTAD3_PREFIX_NODE_H
