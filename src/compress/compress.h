//
// Created by lukasbt on 10/14/23.
//

#ifndef PROJECTAD3_COMPRESS_H
#define PROJECTAD3_COMPRESS_H

typedef struct OPC {
    uint64_t code;
    uint8_t length;
} OPC;

void compress(const char *inputFile, const char *outputFile, int bufferSize, const char *treeFile);


#endif //PROJECTAD3_COMPRESS_H
