//
// Created by lukasbt on 11/13/23.
//

#ifndef PROJECTAD3_BIT_IO_H
#define PROJECTAD3_BIT_IO_H

#include <bits/types/FILE.h>
#include <stdint-gcc.h>
#include "../../compress/compress.h"

typedef struct BitOutputHandler {
    unsigned char* buffer; // the buffer of k bytes
    size_t size;
    size_t bytesWritten; // amount of full bytes written
    int bitsWritten;       // amount of bits written in the current byte
    FILE* outputStream;    // stream to write to when buffer is full
} BitOutputHandler;

BitOutputHandler createOutputHandler(FILE* file, size_t size);
void outputBit(BitOutputHandler* handler, uint64_t bit);
void outputNumber(BitOutputHandler* handler, uint64_t number, int nBits);
void flushBits(BitOutputHandler* handler);
int outputCode(BitOutputHandler* handler, OPC* code);
void freeOutputHandler(BitOutputHandler handler);

#endif //PROJECTAD3_BIT_IO_H
