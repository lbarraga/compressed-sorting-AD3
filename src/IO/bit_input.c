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
    handler.elementsToRead = 0;
    handler.bitsRead = 0;
    handler.inputStream = file;
    return handler;
}

void freeBitInputHandler(BitInputHandler* handler) {
    free(handler->buffer);
}

int readBit(BitInputHandler* handler) {

}