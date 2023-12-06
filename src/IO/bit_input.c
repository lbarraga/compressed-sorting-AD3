//
// Created by lukasbt on 11/22/23.
//

#include <malloc.h>
#include "bit_input.h"
#include <stdio.h>
#include <stdlib.h>

BitInputHandler createBitInputHandler(FILE* file, size_t size_bytes) {
    size_t bufferLength = size_bytes / sizeof(uint64_t);
    BitInputHandler handler;
    handler.buffer = calloc(bufferLength, sizeof(uint64_t));
    handler.size = bufferLength;
    handler.elementsRead = bufferLength;
    handler.bitsRead = 0;
    handler.inputStream = file;
    handler.filePosition = ftell(file);
    return handler;
}

void freeBitInputHandler(BitInputHandler* handler) {
    free(handler->buffer);
}

InputHandlerPosition getInputPosition(BitInputHandler* handler) {
    InputHandlerPosition position = {
            .element = handler->filePosition - (handler->size - handler->elementsRead) * 8,
            .bit = handler->bitsRead,
    };
    return position;
}

void setAtInputPosition(BitInputHandler* handler, InputHandlerPosition* position) {
    long offset = (long) position->element;
    fseek(handler->inputStream, offset, SEEK_SET);
    handler->filePosition = offset;
    handler->bitsRead = 0;
    handler->elementsRead = handler->size; // next time bits are read, the buffer is loaded at the new position.
    readNBits(handler, position->bit);
}

int readLength(BitInputHandler* handler) {
    int ll = (int) readNBits(handler, 5);
    int l = (int) readNBits(handler, ll);
    return l + 1; // Dit is + 1 omdat 0 nooit kan voorkomen en we zo één meer kunnen encoderen.
}

uint64_t readNBits(BitInputHandler* handler, int nBits) {

    if (handler->bitsRead == 64) {
        handler->elementsRead++;
        handler->bitsRead = 0;
    }

    if (handler->elementsRead == handler->size) {
        fseek(handler->inputStream, handler->filePosition, SEEK_SET);
        fread(handler->buffer, sizeof(uint64_t), handler->size, handler->inputStream);
        handler->filePosition += (long) handler->size * 8;
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