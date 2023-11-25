//
// Created by lukasbt on 11/13/23.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bit_io.h"

BitOutputHandler createOutputHandler(FILE* file, size_t size_bytes, size_t elementSize) {
    size_t bufferLength = size_bytes / elementSize;
    BitOutputHandler handler;
    handler.buffer = calloc(bufferLength, elementSize);
    handler.elementSizeBytes = (int) elementSize;
    handler.elementSizeBits = (int) elementSize;
    handler.size = bufferLength;
    handler.bytesWritten = 0;
    handler.bitsWritten = 0;
    handler.outputStream = file;
    return handler;
}

void freeOutputHandler(BitOutputHandler handler) {
    free(handler.buffer);
}

//void printByte(uint64_t byte) {
//    for (int i = N_BITS_UINT64 - 1; i >= 0; --i) {
//        printf("%lu", (byte >> i) & 1);
//    }
//}

void outputNBits(BitOutputHandler* handler, uint64_t bits, int length) {
    if (handler->bitsWritten == handler->elementSizeBits) {
        handler->bytesWritten++;
        handler->bitsWritten = 0;
    }
    if (handler->bytesWritten == handler->size) {
        fwrite(handler->buffer, handler->elementSizeBytes, handler->size, handler->outputStream);
        memset(handler->buffer, 0, handler->size * handler->elementSizeBytes);
        handler->bytesWritten = 0;
    }
    handler->buffer[handler->bytesWritten] |= bits << (handler->elementSizeBits - handler->bitsWritten - length);
    handler->bitsWritten += length;
}

void outputBitString(BitOutputHandler* handler, uint64_t bits, int length) {
    int firstChunkMax = handler->elementSizeBits - handler->bitsWritten;

    if (length <= firstChunkMax) {
        outputNBits(handler, bits, length);
        return;
    }

    outputNBits(handler, bits >> (length - firstChunkMax), firstChunkMax);
    outputNBits(handler, bits, length - firstChunkMax);

}

void flushBits(BitOutputHandler* handler) {
    // [<el size>]
    // [101...011] [001...110] ... [110...???] ... [000...000]
    //                           ^bytes written^
    fwrite(handler->buffer, handler->elementSizeBytes, handler->bytesWritten + 1, handler->outputStream);
}
