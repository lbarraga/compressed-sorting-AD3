//
// Created by lukasbt on 11/23/23.
//

#include <malloc.h>
#include "decoder.h"
#include "prefix_node.h"

Decoder* createDecoder(PrefixTree* prefixTree) {
    Decoder* decoder = malloc(sizeof(Decoder));
    decoder->prefixTree = prefixTree;
    decoder->current = prefixTree->root;
    decoder->linesDecoded = 0;
    return decoder;
}

void freeDecoder(Decoder* decoder) {
    free(decoder);
}


/****************************************************************************************************
 * This method will decode an bit extra, and set decoded from '\0' to the decoded character if the  *
 * decoder has found a character for the encoding.                                                  *
 ****************************************************************************************************/
void decode(Decoder* decoder, int bit, FILE* outputStream) {
    decoder->current = getChildFromBit(decoder->current, bit); // Go left or right.

    if(isLeafNode(decoder->current)) {
        fputc(decoder->current->character, outputStream);
        decoder->current = decoder->prefixTree->root;
        if (decoder->current->character == '\n') {
            decoder->linesDecoded++;
        }
    }
}