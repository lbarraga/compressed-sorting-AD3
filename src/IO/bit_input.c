//
// Created by lukasbt on 11/22/23.
//

#include <malloc.h>
#include "bit_input.h"

BitInputHandler createBitInputHandler(FILE* file, size_t size_bytes) {
    size_t bufferLength = size_bytes / sizeof(uint64_t);
    BitInputHandler handler;
    handler.buffer = calloc(bufferLength, sizeof(uint64_t));
    handler.size = bufferLength;
    handler.elementsRead = bufferLength;
    handler.bitsRead = 0;
    handler.inputStream = file;
    return handler;
}

void freeBitInputHandler(BitInputHandler* handler) {
    free(handler->buffer);
}

void printUint64t(uint64_t byte) {
    for (int i = 64 - 1; i >= 0; --i) {
        printf("%lu", (byte >> i) & 1);
    }
    printf("\n");
}


uint64_t readNBits(BitInputHandler* handler, int nBits) {

    if (handler->bitsRead == 64) {
        handler->elementsRead++;
        handler->bitsRead = 0;
    }

    if (handler->elementsRead == handler->size) {
        fread(handler->buffer, sizeof(uint64_t), handler->size, handler->inputStream);
        handler->elementsRead = 0;
    }

    int bitsRemaining = 64 - handler->bitsRead;
    if (bitsRemaining < nBits) {
        uint64_t firstHalf = readNBits(handler, bitsRemaining);
        uint64_t secondHalf = readNBits(handler, nBits - bitsRemaining);
        return firstHalf << (nBits - bitsRemaining) | secondHalf;
    }

    uint64_t read = (handler->buffer[handler->elementsRead] << handler->bitsRead) >> (64 - nBits);
    handler->bitsRead += nBits;
    return read;
}