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

void setInputHandlerAt(BitInputHandler* handler, long bytePosition) {
    fseek(handler->inputStream, bytePosition, SEEK_SET);
    handler->bitsRead = 0;
    handler->elementsRead = handler->size; // next time bits are read, the buffer is loaded at the new position.
}

int readLength(BitInputHandler* handler) {
    int ll = (int) readNBits(handler, 5);
    int l = (int) readNBits(handler, ll);
    return l;
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