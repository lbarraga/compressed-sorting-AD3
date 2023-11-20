//
// Created by lukasbt on 11/13/23.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bit_io.h"

#define N_BITS_UINT64 64

BitOutputHandler createOutputHandler(FILE* file, size_t size_bytes) {
    size_t bufferLength = size_bytes / sizeof(uint64_t);
    BitOutputHandler handler;
    handler.buffer = calloc(bufferLength, sizeof(uint64_t));
    handler.size = bufferLength;
    handler.bytesWritten = 0;
    handler.bitsWritten = 0;
    handler.outputStream = file;
    return handler;
}

void freeOutputHandler(BitOutputHandler handler) {
    free(handler.buffer);
}

void printByte(uint64_t byte) {
    for (int i = N_BITS_UINT64 - 1; i >= 0; --i) {
        printf("%lu", (byte >> i) & 1);
    }
}

void outputNBits(BitOutputHandler* handler, uint64_t bits, int length) {
    if (handler->bitsWritten == N_BITS_UINT64) {
        handler->bytesWritten++;
        handler->bitsWritten = 0;
    }
    if (handler->bytesWritten == handler->size) {
        fwrite(handler->buffer, sizeof(uint64_t), handler->size, handler->outputStream);
        memset(handler->buffer, 0, handler->size * sizeof(uint64_t));
        handler->bytesWritten = 0;
    }
    handler->buffer[handler->bytesWritten] |= bits << (N_BITS_UINT64 - handler->bitsWritten - length);
    handler->bitsWritten += length;
}

void outputBitString(BitOutputHandler* handler, uint64_t bits, int length) {
    int firstChunkMax = N_BITS_UINT64 - handler->bitsWritten;

    if (length <= firstChunkMax) {
        outputNBits(handler, bits, length);
        return;
    }

    outputNBits(handler, bits >> (length - firstChunkMax), firstChunkMax);
    outputNBits(handler, bits, length - firstChunkMax);

}

void flushBits(BitOutputHandler* handler) {
    // [<- 64  ->]
    // [101...011] [001...110] ... [110...???] ... [000...000]
    //                           ^bytes written^
    fwrite(handler->buffer, sizeof(uint64_t), handler->bytesWritten + 1, handler->outputStream);
}
