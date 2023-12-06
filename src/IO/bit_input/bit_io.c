//
// Created by lukasbt on 11/13/23.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bit_io.h"

#define BITS_IN_UINT64_T 64

BitOutputHandler createOutputHandler(FILE* file, size_t size_bytes) {
    size_t bufferLength = size_bytes / sizeof(uint64_t);
    BitOutputHandler handler;
    handler.buffer = calloc(bufferLength, sizeof(uint64_t));
    handler.size = bufferLength;
    handler.elementsWritten = 0;
    handler.bitsWritten = 0;
    handler.outputStream = file;
    return handler;
}

void freeOutputHandler(BitOutputHandler* handler) {
    free(handler->buffer);
}

void outputNBits(BitOutputHandler* handler, uint64_t bits, int length) {
    // The number of bits exceeds the element's capacity, requiring multiple writes.
    int freeBits = BITS_IN_UINT64_T - handler->bitsWritten;
    if (length > freeBits) {
        outputNBits(handler, bits >> (length - freeBits), freeBits); // recursive call
        length -= freeBits;
    }

    // current element is full
    if (handler->bitsWritten == BITS_IN_UINT64_T) {
        handler->elementsWritten++;
        handler->bitsWritten = 0;
    }

    // Whole buffer is full; output buffer to file
    if (handler->elementsWritten == handler->size) {
        fwrite(handler->buffer, sizeof(uint64_t), handler->size, handler->outputStream);
        memset(handler->buffer, 0, handler->size * sizeof(uint64_t));
        handler->elementsWritten = 0;
    }

    handler->buffer[handler->elementsWritten] |= bits << (BITS_IN_UINT64_T - handler->bitsWritten - length);
    handler->bitsWritten += length;
}


void flushBits(BitOutputHandler* handler) {
    // [<el size>]
    // [101...011] [001...110] ... [110...???] ... [000...000]
    //                           ^bytes written^
    fwrite(handler->buffer, sizeof(uint64_t), handler->elementsWritten + 1, handler->outputStream);
}

int ceilLog22(const int* lineLength) {
    return (int) sizeof(int) * 8 - __builtin_clz(*lineLength);
}

void outputLength(BitOutputHandler* handler, int l) {
    int ll = ceilLog22(&l);
    outputNBits(handler, ll, 5);
    outputNBits(handler, l - 1, ll);
}
