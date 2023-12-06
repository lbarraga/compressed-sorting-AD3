//
// Created by lukasbt on 11/13/23.
//

#ifndef PROJECTAD3_BIT_IO_H
#define PROJECTAD3_BIT_IO_H

#include <bits/types/FILE.h>
#include <stdint-gcc.h>
#include <stddef.h>
#include "../../compress/compress.h"

typedef struct BitOutputHandler {
    uint64_t* buffer;
    size_t size;
    size_t elementsWritten; // amount of full elements written
    int bitsWritten;        // amount of bits written in the current element
    FILE* outputStream;     // stream to write to when buffer is full
} BitOutputHandler;

BitOutputHandler createOutputHandler(FILE* file, size_t size);
void outputNBits(BitOutputHandler* handler, uint64_t bits, int length);
void flushBits(BitOutputHandler* handler);
void outputLength(BitOutputHandler* handler, int l);
void printByte(uint8_t byte);
void freeOutputHandler(BitOutputHandler* handler);

#endif //PROJECTAD3_BIT_IO_H
