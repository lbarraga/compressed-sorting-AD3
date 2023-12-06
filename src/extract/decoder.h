//
// Created by lukasbt on 11/23/23.
//

#ifndef PROJECTAD3_DECODER_H
#define PROJECTAD3_DECODER_H

#include "prefix_tree.h"
#include <stdio.h>

typedef struct Decoder {
    PrefixTree* prefixTree;
    PrefixNode* current;
    long linesDecoded; // then number of lines decoded.
} Decoder;

Decoder* createDecoder(PrefixTree* prefixTree);
void freeDecoder(Decoder* decoder);
void decode(Decoder* decoder, int bit, FILE* outputStream);

#endif //PROJECTAD3_DECODER_H
