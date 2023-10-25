//
// Created by lukasbt on 10/14/23.
//

#ifndef PROJECTAD3_EXTRACT_H
#define PROJECTAD3_EXTRACT_H

#include "../compress/parse_tree_file.h"

typedef struct PrefixNode {
    unsigned char character;
    struct PrefixNode* left;
    struct PrefixNode* right;

} PrefixNode;

void extract(const char *inputFile, const char *outputFile, int bufferSize);

#endif //PROJECTAD3_EXTRACT_H
