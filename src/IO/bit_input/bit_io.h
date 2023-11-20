//
// Created by lukasbt on 11/13/23.
//

#ifndef PROJECTAD3_BIT_IO_H
#define PROJECTAD3_BIT_IO_H

#include <bits/types/FILE.h>
#include <stdint-gcc.h>
#include "../../compress/compress.h"

typedef struct BitOutputHandler {
    uint64_t* buffer; // the buffer of k bytes
    size_t size;
    size_t bytesWritten; // amount of full bytes written
    int bitsWritten;       // amount of bits written in the current byte
    FILE* outputStream;    // stream to write to when buffer is full
} BitOutputHandler;

BitOutputHandler createOutputHandler(FILE* file, size_t size);
void outputBitString(BitOutputHandler* handler, uint64_t bits, int length);
void flushBits(BitOutputHandler* handler);
void freeOutputHandler(BitOutputHandler handler);

#endif //PROJECTAD3_BIT_IO_H
